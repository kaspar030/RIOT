ctx.includes += relpath("include")
#ctx.includes += relpath("libc/include")

ModuleDir("base64")
ModuleDir("bitfield")
ModuleDir("bloom")
ModuleDir("cipher_modes", "crypto/modes")
ModuleDir("color")
ModuleDir("config").auto_init()

# (missing pyjam cpp support)
#ModuleDir("cpp11-compat", "cpp11-compat").uses(["vtimer", "timex"])

ModuleDir("checksum")
ModuleDir("crypto")
ModuleDir("embunit").add_includes("sys/include/embUnit")
ModuleDir("hamming256", "ecc/hamming256")
ModuleDir("fmt")
ModuleDir("hashes")
ModuleDir("od")

ModuleDir("oneway_malloc", "oneway-malloc")\
        .context.includes += "oneway-malloc/include"

ModuleDir("pipe")
ModuleDir("posix").needs(["timex", "vtimer"])
ModuleDir("posix_semaphore", "posix/semaphore").needs("sema")

ModuleDir("posix_sockets", "posix/sockets")\
        .needs(["posix", "random"])\
        .uses(["conn_ip", "conn_tcp", "conn_udp"])

ModuleDir("ps").uses("schedstatistics")
ModuleDir("phydat").needs(["fmt"])

ModuleDir("pthread", "posix/pthread")\
        .needs(["xtimer", "vtimer", "timex"])\
        .context.includes += "posix/pthread/include"

ModuleDir("quad_math")
ModuleDir("saul_reg").needs("saul")
ModuleDir("sema").needs(["xtimer"])
ModuleDir("seq")
ModuleDir("timex")
ModuleDir("trickle")
ModuleDir("tsrb")
ModuleDir("uart_stdio").needs(["tsrb", "periph"])
ModuleDir("ubjson")
ModuleDir("universal_address").uses(["fib", "gnrc_ipv6"])
ModuleDir("vtimer", "compat/vtimer").needs(["timex", "xtimer"])
ModuleDir("xtimer").needs([ctx.BOARD, "periph"]).auto_init()

subinclude("auto_init")
subinclude("cbor")
subinclude("net")
subinclude("newlib")
subinclude("random")
subinclude("shell")
#subinclude("arduino") # (missing pyjam cpp support)
