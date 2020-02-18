#!/usr/bin/env python3

#
# Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
#               2020 Inria
#               2020 Freie Universität Berlin
    

#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import sys

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.serialization import Encoding, PrivateFormat, NoEncryption

def main():
    if len(sys.argv) != 2:
        print("usage: gen_key.py <secret filename>")
        print("... creates an SECP384R1 elliptic curve key in PKCS8 PEM format.")
        sys.exit(1)

    pk = ec.generate_private_key(ec.SECP256R1(), default_backend())
    pem = pk.private_bytes(encoding=Encoding.PEM,
                           format=PrivateFormat.PKCS8,
                           encryption_algorithm=NoEncryption()
                           )

    with open(sys.argv[1], "wb") as f:
        f.write(pem)


if __name__ == '__main__':
    main()
