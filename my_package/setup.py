from setuptools import setup, Extension
import os

setup(
    name="peb.lib",
    version="1",
    ext_modules=[Extension('_peb', [os.path.join('src', 'my_package', 'extension.c')])]
)