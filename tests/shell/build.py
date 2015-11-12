Application("test_shell").needs(["shell", "shell_commands", "ps"])

## chronos is missing a getchar implementation
##BOARD_BLACKLIST += chronos
#
#include $(RIOTBASE)/Makefile.include
