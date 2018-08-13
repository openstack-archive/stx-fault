
from distutils.core import setup, Extension

setup(name="fm_core", version="1.0",
      ext_modules=[Extension("fm_core", [
                             "fm_python_mod_main.cpp"],
                             libraries=[
                             'pq',
                             "fmcommon"],
                             library_dirs=['.']
                             )])
