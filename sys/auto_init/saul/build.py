PseudoModule("auto_init_saul").auto_init()\
        .uses("auto_init_gpio")

Module("auto_init_gpio", "auto_init_gpio.c").auto_init()\
        .needs("saul_gpio")\
        .use_if("auto_init_saul and saul_gpio")
