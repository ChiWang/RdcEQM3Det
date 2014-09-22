'''
 *  $Id: SConstruct, 2014-09-22 16:38:14 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 23/09/2013
 *----------------------------------------------------------
 *  1. create a shared library
 *  2. install it into $DMPSWWORK
 *  then, in your JobOpt* file could use your Alg.
 *----------------------------------------------------------
'''

import os

target='DmpHex2Root'

print "---> compiling: %s\n" % target
#--------------------------------------------------------------------
# environment
env = Environment(ENV = os.environ)
env.ParseConfig("dmpsw-config --include --libs")
env.ParseConfig("python-config --include --libs")  # 2 lines for binding
env.ParseConfig("root-config --cflags --libs")
#env.ParseConfig("clhep-config --include --libs")
#env.PrependENVPath('PATH', os.environ['G4INSTALL']+'/../../../bin')    # 2 lines for G4
#env.ParseConfig("geant4-config --cflags --libs")
#env.MergeFlags('-fopenmp')

#-------------------------------------------------------------------
# include dir. and external libs
env.Prepend(CPPPATH=['./include'])
env.Append(LIBS=['DmpEventRaw'])

#-------------------------------------------------------------------
# main build
goal=env.SharedLibrary(target,Glob('./src/*.cc'))
Default(env.InstallAs(os.environ['DMPSWWORK']+'/lib/lib'+target+".so",goal))

#-------------------------------------------------------------------
# install connector
def ProcessThisDir(fromPath,toPath):
    for tmp in os.listdir(fromPath):
        if "." in tmp[0] or "scons" in tmp or "test" in tmp:   continue
        if os.path.isdir(fromPath+tmp):  ProcessThisDir(fromPath+tmp,toPath+tmp)
        else: Default(Command(toPath+'/'+tmp,fromPath+'/'+tmp,Copy("$TARGET","$SOURCE")))

ProcessThisDir("./share/Connector",os.environ['DMPSWWORK']+'/share/Connector')


