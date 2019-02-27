#!python
import os

debugcflags = [] #['-W1', '-GX', '-EHsc', '-D_DEBUG', '/MDd']   #extra compile flags for debug
releasecflags = [] #['-O2', '-EHsc', '-DNDEBUG', '/MD']         #extra compile flags for release

libraries = ['cppunit']

buildroot = os.path.join(os.getcwd(), 'build')

#get the mode flag from the command line
#default to 'release' if the user didn't specify
mode = ARGUMENTS.get('mode', 'release')   #holds current mode

#check if the user has been naughty: only 'debug' or 'release' allowed
if not (mode in ['debug', 'release']):
   print "Error: expected 'debug' or 'release', found: " + mymode
   Exit(1)

#tell the user what we're doing
print ('**** Compiling in ' + mode + ' mode...')

env = Environment()

#make sure the sconscripts can get to the variables
Export('env', 'buildroot', 'mode', 'debugcflags', 'releasecflags', 'libraries')

#put all .sconsign files in one place
env.SConsignFile()

project = 'src'
SConscript('src/SConscript', exports=['project'])

project = 'test'
SConscript('test/SConscript', exports=['project'])
