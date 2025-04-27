#!/bin/bash

trap cleanup EXIT

TMP_DIR=$(mktemp -d build_out.XXXXXX)

cleanup() {
    rm -Rf -- "${TMP_DIR}"
}

#ALL_GROUPS="static-tests cortex_m4_2 cortex_m4_1 cortex_m0_2 cortex_m0_1 x86 cortex_m3_2 cortex_m3_1 avr8 msp430 arm7"
ALL_GROUPS="static-tests avr8" # cortex_m4_2 cortex_m4_1 cortex_m0_2 cortex_m0_1 x86 cortex_m3_2 cortex_m3_1 avr8 msp430 arm7"

BUILD_ERROR=0

build_group() {
    BUILD_GROUP=$1
    echo "Building group $BUILD_GROUP..."
    NPROC=32 BUILDTEST_MCU_GROUP=$BUILD_GROUP RIOT_VERSION_OVERRIDE=buildtest \
        BUILDTEST_NO_REBASE=1 \
        ./dist/tools/travis-scripts/build_and_test.sh 2>&1 > ${TMP_DIR}/${BUILD_GROUP}

    RES=$?

    if [ $RES -eq 0 ]; then
        rm ${TMP_DIR}/${BUILD_GROUP}
    else
        BUILD_ERROR=1
    fi

    echo "Build group $BUILD_GROUP done."
    return $RES
}

BUILD_GROUPS="${1-$ALL_GROUPS}"

for group in $BUILD_GROUPS; do
    build_group $group &
done

wait

for output in $(ls ${TMP_DIR}); do
    echo BUILD ERRORS of group $(basename $output):
    cat ${TMP_DIR}/$output
done

test $BUILD_ERROR -eq 0
