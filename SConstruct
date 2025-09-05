import os
from utils import download_wasmer, download_wasmtime, WASMER_VER_DEFAULT, WASMTIME_VER_DEFAULT

# Initial options inheriting from CLI args
opts = Variables([], ARGUMENTS)

# Define options
opts.Add(EnumVariable("wasm_runtime", "Wasm runtime used", "wasmer", ["wasmer", "wasmtime"]))
opts.Add(BoolVariable("download_runtime", "(Re)download runtime library", "no"))
opts.Add("runtime_version", "Runtime library version", None)

# SConstruct environment
env = DefaultEnvironment(options=opts)
env["platform"] = {"win32": "windows", "darwin": "macos", "posix": "linux"}[env["PLATFORM"]]
if env.get("runtime_version") == "":
    del env["runtime_version"]

# Download runtime if required
if env["wasm_runtime"] == "wasmer":
    download_wasmer(env, env["download_runtime"], env.get("runtime_version", WASMER_VER_DEFAULT))
elif env["wasm_runtime"] == "wasmtime":
    download_wasmtime(env, env["download_runtime"], env.get("runtime_version", WASMTIME_VER_DEFAULT))

# Check platform specifics
if env["platform"] == "windows":
    env.Append(LIBS=["ole32.lib", "runtimeobject.lib", "oleaut32.lib"])
    env["LIBRUNTIMESUFFIX"] = ".lib"
    env.Append(CCFLAGS=["-MD"])  # Dynamic CRT used by Wasmer >= v3.2.0
    if "/MT" in env["CCFLAGS"]:
        env["CCFLAGS"].remove("/MT")  # Silence MT/MD override warning
    # Force Windows SDK library suffix (see https://github.com/godotengine/godot/issues/23687)
    env.Append(LINKFLAGS=["bcrypt.lib", "userenv.lib", "ws2_32.lib", "advapi32.lib", "ntdll.lib"])
    env.Append(CXXFLAGS=["/std:c++latest"])
    # Additional libraries to build wasmtime for Windows
    if env["wasm_runtime"] == "wasmtime":
        env.Append(LINKFLAGS=["shell32.lib", "ole32.lib", "wasmtime\\lib\\wasmtime.dll.lib"])
        env.Append(LINKFLAGS=["/WX:NO"])  # Temporarily disable warnings as errors to fix LIBCMT conflict warning

# Defines for GDExtension specific API
env.Append(CPPDEFINES=["GDEXTENSION", "LIBWASM_STATIC"])

# Explicit static libraries
runtime_lib = env.File(
    "{runtime}/lib/{prefix}{runtime}{suffix}".format(
        runtime=env["wasm_runtime"],
        prefix=env["LIBPREFIX"],
        suffix=env.get("LIBRUNTIMESUFFIX", env["LIBSUFFIX"]),
    )
)

# CPP includes and libraries
env.Append(CPPPATH=[".", "{}/include".format(env["wasm_runtime"])])
env.Append(LIBS=[runtime_lib])

sources = Glob("*.cpp")

env.SharedLibrary(target="build/wasmer-test", source=sources)
