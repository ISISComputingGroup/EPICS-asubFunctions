TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += asubFunctions
USR_INCLUDES += -I"$(TOP)/../../../libraries/master/jsonformoderncpp" -I"$(TOP)/../../../libraries/master/boost/include"

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install asubFunctions.dbd into <top>/dbd
DBD += asubFunctions.dbd

# specify all source files to be compiled and added to the library
asubFunctions_SRCS += charToStringWaveform.c stringConcatWithCtrlB.c setFunctions.c setFunctionsImpl.cpp 
asubFunctions_SRCS += splitCharWaveform.c displayMotorMSTA.c splitToStringWaveform.c
asubFunctions_SRCS += genData.c incrementor.c copyArgA.c iocExit.c checkMotorError.c
asubFunctions_SRCS += copyArgs.c parseJSON.cpp

asubFunctions_LIBS += $(EPICS_BASE_IOC_LIBS)

# googleTest Runner
ifeq ($(findstring 10.0,$(VCVERSION)),)
SRC_DIRS += $(TOP)/asubFunctionsApp/tests

GTESTPROD_HOST += runner
runner_SRCS += splitCharWaveform_tests.cc parseJSON_tests.cpp
runner_LIBS += asubFunctions
runner_LIBS += $(EPICS_BASE_IOC_LIBS)
GTESTS += runner 
endif

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
-include $(GTEST)/cfg/compat.RULES_BUILD
