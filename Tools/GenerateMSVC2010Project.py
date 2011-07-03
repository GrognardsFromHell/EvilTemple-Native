
from subprocess import call
from os import chdir

chdir('../')
call(["qmake", "-spec", "win32-msvc2010", "-recursive", "-tp", "vc"])
