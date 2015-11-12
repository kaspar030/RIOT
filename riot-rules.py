dprint("default", "including RIOT pyjam rules...")

def envlist(name, splitter=" "):
    res = os.environ.get(name)
    if res:
        return res.split(splitter)

class Board(Module):
    def __init__(s, **kwargs):
        super().__init__(**kwargs)

        board = os.path.basename(s.targets[0])

        ctx.defines += "RIOT_BOARD=\"%s\"" % board

        cpu = kwargs.pop('cpu', board)
        cpu_model = kwargs.pop('cpu_model', cpu)

        ctx.BOARD = board
        ctx.CPU = cpu
        ctx.CPU_MODEL = cpu_model

        ctx.defines += "RIOT_BOARD=\"%s\"" % board
        ctx.defines += "MCU_" + cpu.upper()

        subinclude("../../cpu/%s" % (kwargs.get("cpudir", cpu)))
        s.needs(cpu)

class Application(Module):
    def __init__(s, name=None, **kwargs):
        if not name:
            name = os.path.basename(_relpath)

        super().__init__(name, sources=None, **kwargs)

        s.needs([ctx.BOARD, "core"])

        # Application() objects trigger inclusion of modules through
        # this function
        s.collect_modules()

        # set up linking of the .elf
        s.elfname = s.name + ".elf"
        LinkModule(s.elfname, s.name)
        ModuleList("info-modules", s)

        # dependencies
        all_target_name = relpath("all")
        VirtualTarget(all_target_name)
        depends(locate_bin(all_target_name)[0], s.elfname)
        depends(all_target_name, locate_bin(all_target_name)[0])
        depends("all", all_target_name)

        # flashing
        if ctx._flasher:
            ctx._flasher(s.elfname)
            VirtualTarget(relpath("flash"))
            depends(locate_bin("flash"), "all")
            depends(relpath("flash"), locate_bin("flash"))

    def require_feature(s, _set):
        _set = set(listify(_set))
        _available = ctx._features or set()
        if _set <= _available:
            pass
        else:
            print("Warning:", s.name, "has missing features:", sorted(_available - _set))
        return s

def _Module_auto_init(s):
    dprint("debug", "AUTO_INIT", s.name)
    if not ctx._auto_init:
        ctx._auto_init = []
    ctx._auto_init.append(s)
    return s

def _Module_collect_auto_init(s):
    for module in ctx._auto_init:
        if module.used:
            dprint("debug", "AUTO_INIT collected", s.name, module.name)
            s.uses(module.name, False)
        else:
            dprint("debug", "AUTO_INIT not used ", s.name, module.name)

def _Module_uses_auto_init(s):
    ctx._hooks.append((1, _Module_collect_auto_init, (s, )))
    return s

Module.auto_init=_Module_auto_init
Module.uses_auto_init=_Module_uses_auto_init

class OpenocdFlash(Tool):
    name="FLASH"
    actions = ("${OPENOCD} -f \"${OPENOCD_CONFIG}\""
              " ${OPENOCD_EXTRA_INIT}"
              " -c 'tcl_port 0'"
              " -c 'telnet_port 0'"
              " -c 'gdb_port 0'"
              " -c 'init'"
              " -c 'targets'"
              " -c 'reset halt'"
              " ${OPENOCD_PRE_FLASH_CMDS}"
              " -c 'flash write_image erase \"%sources\"'"
              " -c 'reset halt'"
              " ${OPENOCD_PRE_VERIFY_CMDS}"
              " -c 'verify_image \"%sources\"'"
              " -c 'reset run'"
              " -c 'shutdown'")

    def __init__(s, elf_file, **kwargs):
        super().__init__(locate_bin("flash"), elf_file)
        t = Target.get(s.targets[0])
        t.rebuild = True
        t.bound = True

        t.context._export([
            "OPENOCD_CONFIG",
            "OPENOCD",
            "OPENOCD_EXTRA_INIT",
            "OPENOCD_PRE_FLASH_CMDS",
            "OPENOCD_PRE_VERIFY_CMDS",
            ])

