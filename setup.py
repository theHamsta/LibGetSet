#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
    Setup file for epipolar_consistency.

    This file was generated with PyScaffold 2.5.7.post0.dev6+ngcef9d7f, a tool that easily
    puts up a scaffold for your new Python project. Learn more under:
    http://pyscaffold.readthedocs.org/
"""

import sys
import platform
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
# from setuptools.command.install import _install

import os
import subprocess
from distutils.version import LooseVersion


# class my_build_ext(build_ext):

#     # extra compile args
#     copt = {'msvc': ['/EHsc'],
#             'unix': ['-ggdb'],
#             'mingw32': [],
#             }
#     # extra link args
#     lopt = {
#         'msvc': [],
#         'unix': [],
#         'mingw32': [],
#     }

#     def initialize_options(self, *args):
#         """omit -Wstrict-prototypes from CFLAGS since its only valid for C code."""
#         import distutils.sysconfig
#         cfg_vars = distutils.sysconfig.get_config_vars()

#         build_ext.initialize_options(self)

#     def _set_cflags(self):
#         # set compiler flags
#         c = self.compiler.compiler_type
#         if c in self.copt:
#             for e in self.extensions:
#                 e.extra_compile_args = self.copt[c]
#         if c in self.lopt:
#             for e in self.extensions:
#                 e.extra_link_args = self.lopt[c]

#     def build_extensions(self):
#         self._set_cflags()
#         print("Building")

#         # has to be last call
#         build_ext.build_extensions(self)

# shamelessly copied from https://github.com/pybind/cmake_example/blob/master/setup.py
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j8']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
        


# class install(_install):


#     def run(self):
#         if sys.platform.startswith('linux'):
#             previous_working_dir = os.getcwd()
#             try:
#                 os.mkdir('build_dir')
#             except Exception:
#                 pass
#             os.chdir('build_dir')
#             self.build_dir = os.getcwd()
#             subprocess.call(['cmake', '..',
#                              '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_SHARED_LIBS=OFF'])
#             subprocess.call(['make', '-j8'])
#             os.chdir(previous_working_dir)

#         else:
#             RuntimeError('Currently only Linux build supported')

#         _install.run(self)
#         self.execute(self._post_install, (self.install_lib,))


def setup_package():
    needs_sphinx = {'build_sphinx', 'upload_docs'}.intersection(sys.argv)
    sphinx = ['sphinx'] if needs_sphinx else []
    setup(setup_requires=['six', 'pyscaffold>=2.5a0,<2.6a0'] + sphinx,
          packages=['epipolar_consistency'],
        package_data={'epipolar_consistency': ['build_dir/*' ]},
        #   cmdclass={ 'install': install},
         ext_modules=[CMakeExtension('_epipolar_consistency')],
          cmdclass={
        'build_ext': CMakeBuild},
          use_pyscaffold=False
        )


if __name__ == "__main__":
    setup_package()
