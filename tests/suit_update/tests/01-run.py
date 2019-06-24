#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys
import tempfile

from testrunner import run

# Default test over loopback interface
COAP_HOST = "[fd00:dead:beef::1]"

UPDATE_TIMEOUT = 120
MANIFEST_TIMEOUT = 15

USE_ETHOS = int(os.getenv("USE_ETHOS", "1"))
TAP = os.getenv("TAP", "tap0")


def start_aiocoap_fileserver():
    tmpdirname = tempfile.TemporaryDirectory()
    aiocoap_process = subprocess.Popen(
        "exec aiocoap-fileserver %s" % tmpdirname.name, shell=True
    )

    return tmpdirname, aiocoap_process


def cleanup(tmpdir, aiocoap_process):
    try:
        aiocoap_process.terminate()
        aiocoap_process.wait(1)
    except subprocess.TimeoutExpired:
        aiocoap_process.kill()
        aiocoap_process.wait()

    tmpdir.cleanup()


def notify(coap_server, client_url, version=None):
    cmd = [
        "make",
        "suit/notify",
        "SUIT_COAP_SERVER={}".format(coap_server),
        "SUIT_CLIENT={}".format(client_url),
    ]
    if version is not None:
        cmd.append("SUIT_NOTIFY_VERSION={}".format(version))
    assert not subprocess.call(cmd)


def publish(server_dir, server_url, app_ver, latest_name=None):
    cmd = [
        "make",
        "suit/publish",
        "SUIT_COAP_FSROOT={}".format(server_dir),
        "SUIT_COAP_SERVER={}".format(server_url),
        "APP_VER={}".format(app_ver),
        "RIOTBOOT_SKIP_COMPILE=1",
    ]
    if latest_name is not None:
        cmd.append("SUIT_MANIFEST_SIGNED_LATEST={}".format(latest_name))

    assert not subprocess.call(cmd)


def testfunc(child):
    """For one board test if specified application is updatable"""

    # Initial Setup and wait for address configuration
    child.expect_exact("main(): This is RIOT!")

    if USE_ETHOS == 0:
        # Get device global address
        child.expect(
            r"inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
            "  scope: global  VAL"
        )
        client = "[{}]".format(child.match.group("gladdr").lower())
    else:
        # Get device local address
        client = "[fe80::2%{}]".format(TAP)

    for version in [1, 2]:
        # Wait for suit_coap thread to start
        child.expect_exact("suit_coap: started.")
        # Trigger update process, verify it validates manifest correctly
        notify(COAP_HOST, client, version)
        child.expect_exact("suit_coap: trigger received")
        child.expect_exact("suit: verifying manifest signature...")
        child.expect(
            r"riotboot_flashwrite: initializing update to target slot (\d+)",
            timeout=MANIFEST_TIMEOUT,
        )
        target_slot = int(child.match.group(1))
        # Wait for update to complete
        child.expect_exact(
            "riotboot_flashwrite: riotboot flashing completed successfully",
            timeout=UPDATE_TIMEOUT,
        )
        # Verify running slot
        child.expect(r"running from slot (\d+)")
        assert target_slot == int(child.match.group(1)), "BOOTED FROM SAME SLOT"

    print("TEST PASSED")


if __name__ == "__main__":
    try:
        res = 1
        tmpdir, aiocoap_process = start_aiocoap_fileserver()
        # TODO: wait for coap port to be available

        publish(tmpdir.name, COAP_HOST, 1)
        publish(tmpdir.name, COAP_HOST, 2)

        res = run(testfunc, echo=True)

    except Exception as e:
        print(e)
    finally:
        cleanup(tmpdir, aiocoap_process)

    sys.exit(res)
