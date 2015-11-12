dprint("default", "Building RIOT.")

include("riot-rules.py")

if os.path.relpath(_start_cwd).startswith(("tests/", "examples/")):
    # if started from within an applications directory, only consider that application
    APPLICATIONS = [os.path.relpath(_start_cwd)]
else:
    # otherwise, use environment or default
    APPLICATIONS = envlist("APPLICATIONS") or [ "examples/hello-world" ]

BOARDS = envlist("BOARDS") or envlist("BOARD") or [ "native" ]

if APPLICATIONS == [ "all" ]:
    app_build_files = glob.glob("tests/*/build.py") + glob.glob("examples/*/build.py")
    APPLICATIONS = [ dirname(x) for x in app_build_files ]

if BOARDS == [ "all" ]:
    board_build_files = glob.glob("boards/*/build.py")
    tmp = [ basename(dirname(x)) for x in board_build_files ]
    BOARDS = list(filter(lambda x: not x.endswith("_common"), tmp))

# set defaults
default.CFLAGS += "-Wall -Werror"
default.CFLAGS += "-std=gnu99 -Wall -Wstrict-prototypes -Werror=implicit-function-declaration"
default.CFLAGS += "-ffunction-sections -fdata-sections -fno-builtin -fshort-enums"
default.CFLAGS += "-ggdb -g3"
default.CFLAGS += "-Os"

default.defines += [ "RIOT_VERSION=\"test\"", "RIOT_FILE_RELATIVE=__FILE__" ]
default.includes += "core/include"
default.includes += "drivers/include"

default.defines += "RIOT_CHACHA_PRNG_DEFAULT=0x01234567"

default.OPENOCD = "openocd"
default.CCACHE_BASEDIR = _basedir
global_export("CCACHE_BASEDIR")

# we use gcc for assembly files
builders['.S'] = CompileC

for app in APPLICATIONS:
    for board in BOARDS:
        dprint("default", "Setting up application %s for board %s..." % (app, board))

        # setup build context for (app, board) pair
        BuildContext.init("%s:%s" % (app, board), bindir=os.path.join(app, "bin", board))

        # initialize module context
        Module.init_context()

        # include target board
        subinclude(os.path.join("boards", board))

        # setup default define and includes
        ctx.defines += "RIOT_MCU=\"%s\"" % ctx.CPU
        ctx.includes += "boards/%s/include" % ctx.BOARD

        # include application
        subinclude(app)

        # include other stuff
        subinclude("core")
        subinclude("drivers")
        subinclude("sys")

        # disable assertions if DEVELHELP is unset
        if not "DEVELHELP" in str(ctx.defines).split(" "):
            ctx.defines += "NDEBUG"

        # set current board/app ready for building
        BuildContext.finalize()

dprint("default", "Done parsing buildfiles.")
