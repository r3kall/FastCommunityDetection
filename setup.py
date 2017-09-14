from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

import numpy as np

ext_modules=[Extension("fcd.algorithm.construction",
                       ["fcd/algorithm/construction.pyx"]),
             Extension("fcd.algorithm.update",
                       ["fcd/algorithm/update.pyx"],
                       extra_compile_args=["-ffast-math"])
             ]

setup(
    name='FastCommunityDetection',
    version='0.1',
    packages=['fcd', 'algorithm'],
    url='',
    license='',
    author='Lorenzo Rutigliano',
    author_email='lnz.rutigliano@gmail.com',
    description='',
    ext_modules=cythonize(ext_modules)
)
