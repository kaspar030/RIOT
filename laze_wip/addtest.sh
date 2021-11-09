#!/bin/sh

[ -n "$1" ] || { echo usage!; exit 1 ;}

set -e

cp tests-clif/laze.yml tests-$1
sed -i "s/clif/$1/g" tests-$1/laze.yml
