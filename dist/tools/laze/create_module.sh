#!/bin/sh

[ -f "laze.yml" ] && {
    echo "laze.yml exists, exiting."
    exit 1
}

MODULE_NAME=${1:-$(basename $(pwd))}

USES="$(grep -ho '\bMODULE_\w*' *.c | sort -u | sed 's/^MODULE_//' | tr [:upper:] [:lower:])"
DEPENDS="$(cd ${RIOTBASE} ; USEMODULE=${MODULE_NAME} make -f dist/tools/laze/getdep.mk)"

arglist() {
    for i in $2; do
        echo $1=$i
    done
}

laze create --type=module --auto-sources --name=${MODULE_NAME} $(arglist --depends "${DEPENDS}") $(arglist --uses "${USES}")

[ -d "include" ] && {
    {
        echo "    env:"
        echo "      export:"
        echo "       includes:"
        echo "         - \${relpath}/include"
    } >> laze.yml
}
