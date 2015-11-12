Board(cpu="cpu_native", cpudir="native").needs(["native_base", "config"])

ctx._features = { "periph_test" }

subinclude("drivers")
