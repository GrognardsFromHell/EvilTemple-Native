
from subprocess import call
from os import chdir, mkdir
from os.path import isdir
import shutil
import urllib2
import zipfile

# TODO: On Windows, we could download the swigwin file here and unzip it automatically
# http://www.hartte.de/swigwin-2.0.4.zip
if not isdir('swigwin-2.0.4'):
	print("Downloading SWIG 2.0.4 for Windows")
	response = urllib2.urlopen('http://www.hartte.de/tools/swigwin-2.0.4.zip')
	content = response.read()
	f = open('swigwin-2.0.4.zip', 'wb')
	f.write(content)
	f.close()
	
	print("Extracting archive.")
	zf = zipfile.ZipFile('swigwin-2.0.4.zip', 'r')
	zf.extractall()
	zf.close()	

# Create generated code directory (although it should already exist)
if not isdir('../../NativeEngine.Interop/Generated'):
	mkdir('../../NativeEngine.Interop/Generated')
	
# Remove previously generated code


swigPath = "swigwin-2.0.4/swig.exe"

print("Generating Wrapper Source Code")
call([swigPath, "-c++", "-namespace", "EvilTemple.NativeEngine.Interop", "-csharp", "-dllimport", "NativeEngine", "-outdir", "../NativeEngine.Interop/Generated", "../NativeEngine/wrapper/nativeengine.i"])
