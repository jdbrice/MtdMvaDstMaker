#!python
import subprocess
import os
import platform

SConscript('color_SConscript')
Import( 'env' )

# env = Environment()

SConscript( "modules/FemtoDstFormat/SConstruct" )
SConscript( "modules/MvaDstFormat/SConstruct" )
# SConscript( "modules/PicoDstSL16cMtd/SConstruct" )
# SConscript( "modules/ProductionUtils/SConstruct")

env.Append(CPPPATH    = [ "modules/" ])
env.Append(CXXFLAGS   = ['-std=c++11'])
JDB_LIB = os.environ.get("JDB_LIB", "" )
env.Append(LIBPATH    = [ JDB_LIB + "/lib/" ] )
env.Append(LIBS       = [ "libRooBarbCore.a", "libRooBarbConfig.a", "libRooBarbTasks.a", "libRooBarbRootAna.a", "libRooBarbUnitTest.a", "libRooBarbExtra.a" ] )

# # ROOT
ROOTCFLAGS      = subprocess.check_output( ['root-config',  '--cflags'] ).rstrip().split( " " )
ROOTLIBS        = subprocess.check_output( ["root-config",  "--libs"] )
ROOT_SYS        = os.environ.get( "ROOTSYS", "" )

env.Append(CPPPATH   = [ ROOT_SYS + "/include/", JDB_LIB + "/include" ])
env.Append(CXXFLAGS  = ROOTCFLAGS )
env.Append(LINKFLAGS = ROOTCFLAGS )

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    env.Append(CXXFLAGS = '-g')

env[ "_LIBFLAGS" ] = env[ "_LIBFLAGS" ] + " " + ROOTLIBS + " "
# REMOVE "-pthread" from clang link options in OS X
# assuming darwin=clang but must be a better way...
if "Darwin" in platform.platform() :
	env[ "LINKFLAGS" ].remove( "-pthread" )

env.Program( target="bin/ana.app", source=[ "modules/MvaDstFormat/DictionaryTrackHeap.cpp", "modules/FemtoDstFormat/DictionaryFemtoDst.cpp", "Engine.cpp"] )
