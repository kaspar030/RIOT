cbor = Module("cbor")
if ctx.BOARD != "native":
    cbor.context.defines += [ "CBOR_NO_FLOAT", "CBOR_NO_PRINT", "CBOR_NO_SEMANTIC_TAGGING" ]
