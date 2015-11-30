TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += asubFunctions

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install asubFunctions.dbd into <top>/dbd
DBD += asubFunctions.dbd

# specify all source files to be compiled and added to the library
asubFunctions_SRCS += charToStringWaveform.c stringConcatWithCtrlB.c setFunctions.c setFunctionsImpl.cpp 
asubFunctions_SRCS += splitCharWaveform.c displayMotorMSTA.c splitToStringWaveform.c
asubFunctions_SRCS += genData.c incrementor.c copyArgA.c iocExit.c checkMotorError.c
asubFunctions_SRCS += queuedPVSet.c queuedPVSetImpl.cpp copyArgs.c

asubFunctions_LIBS += pv $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

