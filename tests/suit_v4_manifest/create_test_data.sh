#!/bin/bash

set -e

gen_manifest() {
  local out="$1"
  shift
  local seqnr="$1"
  shift

  ${RIOTBASE}/dist/tools/suit_v4/gen_manifest.py \
    --template ${RIOTBASE}/dist/tools/suit_v4/test-2img.json \
    --urlroot "test://test" \
    --seqnr $seqnr \
    --uuid-vendor "riot-os.org" \
    --uuid-class $BOARD \
    --offsets 0x1000,0x2000 \
    -o "$out" \
    $*
}

sign_manifest() {
  local in="$1"
  local out="$2"

  ${RIOTBASE}/dist/tools/suit_v4/sign-04.py keys/private_key keys/public_key "$in" "$out"
}

# random invalid manifest
echo foo > manifests/manifest0.bin

# random valid cbor
gen_manifest manifests/manifest1.bin 1 manifests/manifest0.bin manifests/manifest0.bin

# manifest with invalid seqnr
sign_manifest manifests/manifest1.bin manifests/manifest2.bin

# manifest with valid seqnr, invalid class id
( BOARD=invalid gen_manifest manifests/manifest3_unsigned.bin 2 manifests/manifest0.bin manifests/manifest0.bin)
sign_manifest manifests/manifest3_unsigned.bin manifests/manifest3.bin

# manifest with valid seqnr, valid class id
gen_manifest manifests/manifest4_unsigned.bin 2 manifests/manifest0.bin manifests/manifest0.bin
sign_manifest manifests/manifest4_unsigned.bin manifests/manifest4.bin
