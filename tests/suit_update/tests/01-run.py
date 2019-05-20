#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys
from testrunner import run


# Default test over loopback interface
COAP_HOST = '[fd00:dead:beef::1]'

UPDATE_TIMEOUT = 120
MANIFEST_TIMEOUT = 15

# If available use user defined tags for latest, Default: latest-1/2
MANIFESTS = os.getenv('MANIFESTS', 'latest-1 latest-2').split(' ')

def notify(server_url, client_url, manifest):
    cmd = ['make', 'SUIT_MANIFEST_SIGNED_LATEST={}'.format(manifest), 'suit/notify',
           'SUIT_COAP_SERVER={}'.format(server_url),
           'SUIT_CLIENT={}'.format(client_url)]
    assert not subprocess.call(cmd)


def testfunc(child):
    """For one board test if specified application is updatable"""

    # Initial Setup and wait for address configuration
    child.expect_exact('main(): This is RIOT!')
    # Get device global address
    child.expect(r'inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)'
                '  scope: global  VAL')
    client = "[{}]".format(child.match.group("gladdr").lower())

    for manifest in MANIFESTS:
        # Wait for suit_coap thread to start
        child.expect_exact('suit_coap: started.')
        # Trigger update process, verify it validates manifest correctly
        notify(COAP_HOST, client, manifest)
        child.expect_exact('suit_coap: trigger received')
        child.expect_exact('suit: verifying manifest signature...')
        child.expect(
            r'riotboot_flashwrite: initializing update to target slot (\d+)',
            timeout=MANIFEST_TIMEOUT)
        target_slot = int(child.match.group(1))
        # Wait for update to complete
        child.expect_exact(
            'riotboot_flashwrite: riotboot flashing completed successfully',
            timeout=UPDATE_TIMEOUT)
        # Verify running slot
        child.expect(r'running from slot (\d+)')
        assert target_slot == int(child.match.group(1)), "BOOTED FROM SAME SLOT"

    print("TEST PASSED")

if __name__ == "__main__":
    sys.exit(run(testfunc, echo=True))
