Module("saul", "saul_str.c").needs(["phydat"]).uses("auto_init_saul")
Module("saul_gpio", "gpio_saul.c").needs(["saul"]).uses("auto_init_gpio").auto_init()
PseudoModule("saul_default").needs("saul")
