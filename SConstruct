import os
import sys
from distutils.dir_util import copy_tree, remove_tree

env = DefaultEnvironment()

wasmer_path = 'wasmer/'
platform = env['PLATFORM']

print('Platform:', platform)

if platform == 'posix':
  wasmer_platform_source = 'wasmer-linux-amd64'
elif platform == 'darwin':
  wasmer_platform_source = 'wasmer-darwin-amd64'
elif platform == 'win32':
  wasmer_platform_source = 'wasmer-windows-amd64'
else:
  sys.exit(1)

if os.path.exists(wasmer_path): remove_tree(wasmer_path)
copy_tree(wasmer_platform_source, wasmer_path)

env.Append(CPPPATH=['.', wasmer_path + 'include/'])
env.Append(LIBPATH=[wasmer_path + 'lib/'])
env.Append(LIBS=['wasmer'])

sources = Glob('*.c')

env.SharedLibrary(target='build/wasmer-test', source=sources)
