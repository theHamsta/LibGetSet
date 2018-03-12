#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
    Setup file for epipolar_consistency.

    This file was generated with PyScaffold 2.5.7.post0.dev6+ngcef9d7f, a tool that easily
    puts up a scaffold for your new Python project. Learn more under:
    http://pyscaffold.readthedocs.org/
"""

import sys
from setuptools import setup
from setuptools.command.build_ext import build_ext
from setuptools.command.install import _install

import os
import subprocess


class my_build_ext(build_ext):

    # extra compile args
    copt = {'msvc': ['/EHsc'],
            'unix': ['-ggdb'],
            'mingw32': [],
            }
    # extra link args
    lopt = {
        'msvc': [],
        'unix': [],
        'mingw32': [],
    }

    def initialize_options(self, *args):
        """omit -Wstrict-prototypes from CFLAGS since its only valid for C code."""
        import distutils.sysconfig
        cfg_vars = distutils.sysconfig.get_config_vars()

        build_ext.initialize_options(self)

    def _set_cflags(self):
        # set compiler flags
        c = self.compiler.compiler_type
        if c in self.copt:
            for e in self.extensions:
                e.extra_compile_args = self.copt[c]
        if c in self.lopt:
            for e in self.extensions:
                e.extra_link_args = self.lopt[c]

    def build_extensions(self):
        self._set_cflags()
        print("Building")

        # has to be last call
        build_ext.build_extensions(self)


class install(_install):
    def run(self):
        if sys.platform.startswith('linux'):
            previous_working_dir = os.getcwd()
            try:
                os.mkdir('build_dir')
            except Exception:
                pass
            os.chdir('build_dir')
            subprocess.call(['cmake', '../EpipolarConsietency',
                             '-DCMAKE_BUILD_TYPE=Release'])
            subprocess.call(['make', '-j8'])
            os.chdir(previous_working_dir)

        else:
            RuntimeError('Currently only Linux build supported')

        _install.run(self)


def setup_package():
    needs_sphinx = {'build_sphinx', 'upload_docs'}.intersection(sys.argv)
    sphinx = ['sphinx'] if needs_sphinx else []
    setup(setup_requires=['six', 'pyscaffold>=2.5a0,<2.6a0'] + sphinx,
          use_pyscaffold=True,
          cmdclass={'build_ext': my_build_ext,
                    'install': install})


if __name__ == "__main__":
    setup_package()
