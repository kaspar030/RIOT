- download of sources with ninja
- features in boards
- module conflicts
- module provides, defaults when multiple options


# Patterns:

## submodules

    module:
        - name: core
        - sources:
            [...]
          uses:
              - core_thread_flags

        - name: core_thread_flags
          sources: thread_flags.c
          depends: core

