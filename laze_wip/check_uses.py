#!/usr/bin/env python3

from typing import List, Set

import os
import sys
import yaml
from subprocess import run
from yaml import load, CBaseLoader


builder = "native"
app = "tests/pkg_fatfs_vfs"

data = yaml.load(open(sys.argv[1]), Loader=CBaseLoader)

native = data[app][builder]
modules = native["modules"]
relpath = "../.."


def grep_file_uses(filename: str):
    res = run("grep -E -o 'MODULE_\w+' %s | cut -c8- | tr [:upper:] [:lower:]" % filename,
              shell=True, check=True, capture_output=True, text=True)
    if not res.stdout:
        return set()
    else:
        return set(res.stdout.splitlines())


uses_map = {}
for k, v in modules.items():
    res = set(v.get("uses", []))
    res -= set("all")
    uses_map[k] = res


def get_transitive_uses(module, seen=None):
    if seen is None:
        seen = set()
    else:
        if module in seen:
            return set()
    seen.add(module)
    res = uses_map.get(module, set())
    for dep in list(res):
        res |= get_transitive_uses(dep, seen)
    return res


for k, v in modules.items():
    uses = get_transitive_uses(k)
    if "all" in uses:
        continue

    srcdir = v["vars"]["srcdir"]
    sources = v.get("sources", [])


    if type(sources) is not list:
        sources = [sources]

    sources[:] = [x for x in sources if type(x) == str]

    sources += v.get("optional sources used", [])
    if not sources:
    #    print("  (no sources)")
        continue

    #print("  sources:", sources)
    module_actually_used: Set[str] = set()
    for source in sources:
        filename = os.path.join(relpath, srcdir, source)
        module_actually_used |= grep_file_uses(filename)

    #print("  file:", module_actually_used)
    #print("  laze:", sorted(set(v.get("uses", []))))
    laze_missing = set(module_actually_used) - uses
    if laze_missing:
        print("module:", k)
        print("  diff:", sorted(laze_missing))

