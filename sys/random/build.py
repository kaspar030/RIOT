prngs = [ "mersenne", "minstd", "musl_lcg" ]
prng_default = "mersenne"

for prng in prngs:
    module_name = "prng_" + prng
    x = Module(module_name, prng + ".c")
    if prng == prng_default:
        others = prngs.copy()
        others.remove(prng)
        x.use_if("not (" + " or ".join(["prng_" + x for x in others]) + ")")

PseudoModule("random").uses(["prng_" + x for x in prngs])
