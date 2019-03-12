#!python
import os

############################################################################################""
commonflags = ['-std=c++11']

debugcflags = commonflags + [] #['-W1', '-GX', '-EHsc', '-D_DEBUG', '/MDd']   #extra compile flags for debug
releasecflags = commonflags + [] #['-O2', '-EHsc', '-DNDEBUG', '/MD']         #extra compile flags for release

cpppaths = [os.path.join(os.getcwd(), 'src')]
libraries = ['cppunit']

buildroot = os.path.join(os.getcwd(), 'build')
############################################################################################""


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

env.Append( CPPPATH=cpppaths)

#make sure the sconscripts can get to the variables
Export('env', 'buildroot', 'mode', 'debugcflags', 'releasecflags', 'libraries')

#put all .sconsign files in one place
env.SConsignFile()

project = 'test'
SConscript('test/SConscript', exports=['project'])
