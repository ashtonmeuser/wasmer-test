import os
from utils import download_wasmer, WASMER_VER_DEFAULT

# Initial options inheriting from CLI args
opts = Variables([], ARGUMENTS)

# Define options
opts.Add(
    EnumVariable("wasm_runtime", "Wasm runtime used", "wasmer", ["wasmer", "wasmtime"])
)
opts.Add(BoolVariable("download_runtime", "(Re)download runtime library", "no"))
opts.Add("runtime_version", "Runtime library version", None)

# SConstruct environment
env = DefaultEnvironment(options=opts)
env["platform"] = {"win32": "windows", "darwin": "macos", "posix": "linux"}[
    env["PLATFORM"]
]

print("Platform:", env["platform"])

# Download runtime if required
download_wasmer(
    env, env["download_runtime"], env.get("runtime_version", WASMER_VER_DEFAULT)
)

# Check platform specifics
if env["platform"] == "windows":
    env.Append(LIBS=["ole32.lib", "runtimeobject.lib"])
    env["LIBRUNTIMESUFFIX"] = ".lib"
    env.Append(CCFLAGS=["-MD"])  # Dynamic CRT used by Wasmer >= v3.2.0
    # if "/MT" in env["CCFLAGS"]: env["CCFLAGS"].remove("/MT")  # Silence MT/MD override warning
    # Force Windows SDK library suffix (see https://github.com/godotengine/godot/issues/23687)
    # env.Append(LINKFLAGS=["bcrypt.lib", "userenv.lib", "ws2_32.lib", "advapi32.lib", "ntdll.lib"])

    # env.Append(CPPDEFINES=['WIN32', '_WIN32', '_WINDOWS', '_CRT_SECURE_NO_WARNINGS'])
    env.Append(CCFLAGS=["-W3", "-GR"])
    env.Append(CXXFLAGS=["/std:c++latest"])
    env.Append(LIBS=["bcrypt", "userenv", "ws2_32", "advapi32.lib", "ntdll.lib"])

# Defines for GDExtension specific API
env.Append(CPPDEFINES=["GDEXTENSION", "LIBWASM_STATIC"])

# Explicit static libraries
runtime_lib = env.File(
    "wasmer/lib/{prefix}{runtime}{suffix}".format(
        prefix=env["LIBPREFIX"],
        runtime="wasmer",
        suffix=env.get("LIBRUNTIMESUFFIX", env["LIBSUFFIX"]),
    )
)

# CPP includes and libraries
env.Append(CPPPATH=[".", "wasmer/include"])
env.Append(LIBS=[runtime_lib])

sources = Glob("*.cpp")

env.SharedLibrary(target="build/wasmer-test", source=sources)
