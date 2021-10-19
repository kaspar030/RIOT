MODULE = libc_string

ifeq (1, $(LTO))
  # with LTO enabled, ld discards the symbols in an early step, but then later
  # inserts implicit calls for some code, leading to "undefined reference"
  # errors.
  # So, when building with LTO enabled, don't LTO memset() and memcpy().
  SRC_NOLTO += memset.c memcpy.c
endif

include $(RIOTBASE)/Makefile.base
