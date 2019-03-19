# RIOT ninja/laze build system howto

## Introduction

This branch contains an experimental, ninja-based build system for RIOT.

## Status

This is experimental and incomplete.

Only a handful of boards are supported (at least native, samr21-xpro,
stm32f4discovery) and the resulting builds have not been tested much.

Only building is supported, no flashing. (Though the old Makefiles are still in
place).

## How to try

1. install ninja using the package manager of your choice
2. get laze: "pip3 install --user laze"
3. in examples/hello-world, run "LAZE=1 BOARD=samr21-xpro make all"
