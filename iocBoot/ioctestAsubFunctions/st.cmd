#!../../bin/windows-x64-debug/testAsubFunctions

## You may have to change testAsubFunctions to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/testAsubFunctions.dbd"
testAsubFunctions_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/testCharToStringWaveform.db","P=$(MYPVPREFIX)")
dbLoadRecords("db/testSplitCharWaveform.db","P=$(MYPVPREFIX)")
dbLoadRecords("db/testParseJSON.db","P=$(MYPVPREFIX)")
dbLoadRecords("db/testGenData.db","P=$(MYPVPREFIX)")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=faa59Host"
