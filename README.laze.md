# RIOT ninja/laze build system howto

## Introduction

This branch contains an experimental, ninja-based build system for RIOT.

## Status

This is experimental and incomplete.

Only a handful of boards are supported (at least native, nrf52dk, arduino-mega2560)
and the resulting builds have not been tested much.

## How to try

### Install laze and prerequisites
1. install ninja using the package manager of your choice
2. get a rust toolchain either through package manager or https://rustup.rs/
3. install laze using cargo: `cargo install laze`

### Usage

The command `laze build` will build all possible configurations in a specific
application folder.

E.g.,

    $ cd examples/hello-world
    $ laze build

You can specify a single board using `-b <board-name>`:

    $ laze build -b samr21-xpro

Most boards have a "flash" task which invokes a default flasher:

    $ laze task -b samr21-xpro flash

Tasks build their dependencies, so this also works:

    $ laze task -b native run
