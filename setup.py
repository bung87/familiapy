from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import glob
# from setuptools.dist import Distribution

# cpp_args = ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7']
# extra_objects = glob.glob('Familia/third_party/lib/*.a')

ext_modules = [
    Extension(
    'familiapy.familia',
    ['Familia/src/inference_engine.cpp', 
    'Familia/src/sampler.cpp',
    'Familia/src/config.cpp',
    'Familia/src/document.cpp',
    'Familia/src/model.cpp',
    'Familia/src/semantic_matching.cpp',
    'Familia/src/tokenizer.cpp',
    'Familia/src/util.cpp',
    'Familia/src/vocab.cpp',
    'Familia/src/vose_alias.cpp',
    'familiapy/familia.cpp'],
    library_dirs=['Familia/third_party/lib'],
    # libraries=['google'],
    libraries=['gflags', 'glog','protobuf'],
    # extra_objects=extra_objects,
    include_dirs=['pybind11/include','Familia/include','Familia/third_party/include'],
    language='c++',
    # extra_compile_args = cpp_args,
    ),
    
    Extension(
    'familiapy.inference_engine',
    ['Familia/src/inference_engine.cpp', 
    'Familia/src/sampler.cpp',
    'Familia/src/config.cpp',
    'Familia/src/document.cpp',
    'Familia/src/model.cpp',
    'Familia/src/semantic_matching.cpp',
    'Familia/src/tokenizer.cpp',
    'Familia/src/util.cpp',
    'Familia/src/vocab.cpp',
    'Familia/src/vose_alias.cpp',
    'familiapy/inference_engine.cpp'],
    library_dirs=['Familia/third_party/lib'],
    # libraries=['google'],
    libraries=['gflags', 'glog','protobuf'],
    # extra_objects=extra_objects,
    include_dirs=['pybind11/include','Familia/include','Familia/third_party/include'],
    language='c++',
    # extra_compile_args = cpp_args,
    ),
]




# class get_pybind_include(object):
#     """Helper class to determine the pybind11 include path
#     The purpose of this class is to postpone importing pybind11
#     until it is actually installed, so that the ``get_include()``
#     method can be invoked. """

#     def __init__(self, user=False):
#         self.user = user

#     def __str__(self):
#         import pybind11
#         return pybind11.get_include(self.user)


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    """
    if has_flag(compiler, '-std=c++14'):
        return '-std=c++14'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    else:
        raise RuntimeError('Unsupported compiler -- at least C++11 support '
                           'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libstdc++','-flat_namespace', '-std=c++11',"-lstdc++",'-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        opts.append("-fPIC")
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

# class MyDist(Distribution):
#      def has_ext_modules(self):
#          return True

setup(
    name='familiapy',
    version='0.0.1',
    author='bung',
    description='Familia python binding using pybind11',
    license = "MIT",
    url = "https://github.com/bung87/familiapy", 
    ext_modules=ext_modules,
)