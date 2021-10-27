# how to use

    # sudo ip tuntap add dev tap0 mode tap user ${USER}
    # sudo ip a a 192.168.111.1/24 dev tap0
    # sudo ip link set up dev tap0;

    # bin/native/nanonet.elf tap0

    # (in another shell)
    # ping 192.168.111.2

Or try "coap://192.168.111.2:5683/riot/board" with your coap browser of choice.
