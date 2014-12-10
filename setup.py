from distutils.core import setup
import py2exe

PY2EXE_OPTIONS = dict()

setup(console=['automator.py'], options = { 'py2exe' : OPTIONS },)
