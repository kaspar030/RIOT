#!/bin/bash

set -e

gen_manifest() {
   local out="$1"
   shift
   local seqnr="$1"
   shift


   "${RIOTBASE}/dist/tools/suit_v3/gen_manifest.py" \
     --urlroot "test://test" \
     --seqnr "$seqnr" \
     --uuid-vendor "riot-os.org" \
     --uuid-class "${BOARD}" \
     -o "$out.tmp" \
     "${1}:$((0x1000))" "${2}:$((0x2000))"

    ${SUIT_TOOL} create -f suit -i "$out.tmp" -o "$out"

    rm -f "$out.tmp"
}

sign_manifest() {
  local in="$1"
  local out="$2"

  ${SUIT_TOOL} sign -k "${SUIT_SEC}" -m "$in" -o "$out"
}

# random invalid data files
echo foo > manifests/file1.bin
echo bar > manifests/file2.bin

# random valid cbor (manifest but not signed, missing cose auth)
gen_manifest manifests/manifest0.bin 1 manifests/file1.bin manifests/file2.bin

# manifest with invalid seqnr
sign_manifest manifests/manifest0.bin manifests/manifest1.bin

(BOARD=invalid gen_manifest manifests/manifest2.bin.unsigned 2 manifests/file1.bin manifests/file2.bin)
sign_manifest manifests/manifest2.bin.unsigned manifests/manifest2.bin

# valid manifest, valid seqnr, signed
gen_manifest manifests/manifest3.bin.unsigned 2 manifests/file1.bin manifests/file2.bin
sign_manifest manifests/manifest3.bin.unsigned manifests/manifest3.bin
