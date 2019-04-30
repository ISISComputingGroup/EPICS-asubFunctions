TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = testAsubFunctions
# testAsubFunctions.dbd will be created and installed
DBD += testAsubFunctions.dbd

# testAsubFunctions.dbd will be made up from these files:
testAsubFunctions_DBD += base.dbd

# Include dbd files from all support applications:
testAsubFunctions_DBD += asubFunctions.dbd

# Add all the support libraries needed by this IOC
testAsubFunctions_LIBS += asubFunctions

# testAsubFunctions_registerRecordDeviceDriver.cpp derives from testAsubFunctions.dbd
testAsubFunctions_SRCS += testAsubFunctions_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
testAsubFunctions_SRCS_DEFAULT += testAsubFunctionsMain.cpp
testAsubFunctions_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#testAsubFunctions_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
testAsubFunctions_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

