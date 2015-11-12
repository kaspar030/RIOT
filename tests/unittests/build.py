x = Application()\
        .needs("embunit")\
        .add_includes("tests/unittests/common")

#BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos msb-430 msb-430h pca10000 \
#                          pca10005 spark-core stm32f0discovery \
#                          telosb wsn430-v1_3b wsn430-v1_4 z1 nucleo-f334 \
#                          yunjia-nrf51822 samr21-xpro arduino-mega2560 \
#                          airfy-beacon nrf51dongle nrf6310 weio

unittests = glob.glob("tests-*")
unittests.remove("tests-relic")

unittest_depends = {
        "tests-base64" : "base64",
        "tests-bitfield" : "bitfield",
        "tests-bloom" : [ "bloom", "hashes" ],
        "tests-cbor" : "cbor",
        "tests-checksum" : "checksum",
        "tests-color" : "color",
        "tests-crypto" : [ "crypto", "cipher_modes"],
        "tests-ecc" : "hamming256",
        "tests-fib" : "fib",
        "tests-fib_sr" : "fib",
        "tests-fmt" : "fmt",
        "tests-gnrc_ipv6_hdr" : "gnrc_ipv6_hdr",
        "tests-hash_string" : "hash_string",
        "tests-hsahes" : "hashes",
        "tests-inet_csum" : "inet_csum",
        "tests-ipv4_addr" : "ipv4_addr",
        "tests-ipv6_addr" : "ipv6_addr",
        "tests-ipv6_hdr" : "ipv6_hdr",
        "tests-ipv6_nc" : ["gnrc_ipv6_nc", "gnrc_ipv6_netif"],
        "tests-ipv6_netif" : ["ipv6_addr", "gnrc_ipv6_netif", "gnrc_netif", "gnrc_ndp_node" ],
        "tests-netif" : "gnrc_netif",
        "tests-netopt" : "netopt",
        "tests-netreg" : "gnrc_netreg",
        "tests-pktbuf" : "gnrc_pktbuf_static",
#        "tests-pktqueue" : "pktqueue",
        "tests-seq" : "seq",
        "tests-sixlowpan_ctx" : "gnrc_sixlowpan_ctx",
        "tests-timex" : "timex",
        "tests-ubjson" : [ "ubjson", "pipe" ],
        }

unittest_defines = {
        "tests-crypto" : "CRYPTO_THREEDES",
        "tests-fib" : [
            "FIB_DEVEL_HELPER",
            "UNIVERSAL_ADDRESS_SIZE=16",
            "UNIVERSAL_ADDRESS_MAX_ENTRIES=40"],
        "tests-fib_sr" : [
            "FIB_DEVEL_HELPER",
            "UNIVERSAL_ADDRESS_SIZE=16",
            "UNIVERSAL_ADDRESS_MAX_ENTRIES=40"],
        "tests-ipv6_netif" : "GNRC_NETIF_NUMOF=3",
        "tests-netif" : "GNRC_NETIF_NUMOF=3",
        }

if len(unittests) == 0:
    x.context.defines += "NO_TEST_SUITES"
    dprint("default", "There was no test suite specified!")

else:
    unittest_names = []
    for unittest in unittests:
        # if the test subdirectory has its own buildfile, include it
        if os.path.isfile(os.path.join(unittest, "build.py")):
            subinclude(unittest)

        # otherwise, create default test module
        else:
            ModuleDir(unittest)\
                    .uses("unittests")\
                    .needs(unittest_depends.get(unittest))\
                    .add_includes("tests/unittests/%s" % unittest)

            extra_defines = unittest_defines.get(unittest)
            if extra_defines:
                ctx.defines += extra_defines

        # make unittest application depend on the test module
        x.needs(unittest)

        # cut off "tests-"
        unittest_names.append(unittest[6:])

# some modules use this to ifdef unittest-only code, so define it globally
ctx.defines += "TEST_SUITES='%s'" % ",".join(unittest_names)

# needed for tests-cbor
ctx.libs += "m"

x.context.defines += "RIOT_FILE_RELATIVE=__FILE__"
