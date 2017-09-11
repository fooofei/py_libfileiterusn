#coding=utf-8

'''
 run python setup.py build to generate .pyd
'''

import os
# output debug messages
#os.environ.update({'DISTUTILS_DEBUG':"true"})

import shutil
import sys
from distutils.core import setup
from distutils.core import Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

curpath = os.path.dirname(os.path.realpath(__file__))

MODULE_NAME='libfileiterusn'
VERSION='1.0'






def entry():
    # not work
    #os.environ.update({'VS90COMNTOOLS':'C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build'})
    # my computer have Visual Studio 2012, 2017
    os.environ.update({'VS90COMNTOOLS':os.environ.get('VS110COMNTOOLS')})

    # not add .h files
    sources=[
        'main.cpp',
        '../cpp_pieces/file_iterator_usn.cpp',
    ]


    setup(
        name=MODULE_NAME,
        version=VERSION,
        description="libfileiterusn fast iter drive files use NTFS usn",
        ext_modules=cythonize(Extension(MODULE_NAME,
            language='c++',
            sources=sources,
            define_macros=[('WIN32',None)],
            include_dirs=['../cpp_common','../cpp_pieces','../py_string_address/python_dev/include'],
            #extra_compile_args=['/Zi'],
            #extra_link_args=['/DEBUG:FULL']
        ))
    )


if __name__ == '__main__':
    entry()