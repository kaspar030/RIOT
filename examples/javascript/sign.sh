#!/bin/sh

{
    cat $1
    echo -n "// "
    ../../dist/tools/firmware/bin/firmware sign $1 ../../dist/tools/firmware/priv.key
} > $1.signed
