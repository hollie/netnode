#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-18F2580.mk)" "nbproject/Makefile-local-18F2580.mk"
include nbproject/Makefile-local-18F2580.mk
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=18F2580
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/oo.o ${OBJECTDIR}/_ext/1472/utility_monitor.o ${OBJECTDIR}/_ext/1472/xpl.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/eeprom.o.d ${OBJECTDIR}/_ext/1472/oo.o.d ${OBJECTDIR}/_ext/1472/utility_monitor.o.d ${OBJECTDIR}/_ext/1472/xpl.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/eeprom.o ${OBJECTDIR}/_ext/1472/oo.o ${OBJECTDIR}/_ext/1472/utility_monitor.o ${OBJECTDIR}/_ext/1472/xpl.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-18F2580.mk dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F2580
MP_PROCESSOR_OPTION_LD=18f2580
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x7dc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0x5f4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/eeprom.o   ../eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/eeprom.o 
	
${OBJECTDIR}/_ext/1472/oo.o: ../oo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/oo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/oo.o   ../oo.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/oo.o 
	
${OBJECTDIR}/_ext/1472/utility_monitor.o: ../utility_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/utility_monitor.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/utility_monitor.o   ../utility_monitor.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/utility_monitor.o 
	
${OBJECTDIR}/_ext/1472/xpl.o: ../xpl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/xpl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/xpl.o   ../xpl.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/xpl.o 
	
else
${OBJECTDIR}/_ext/1472/eeprom.o: ../eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/eeprom.o   ../eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/eeprom.o 
	
${OBJECTDIR}/_ext/1472/oo.o: ../oo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/oo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/oo.o   ../oo.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/oo.o 
	
${OBJECTDIR}/_ext/1472/utility_monitor.o: ../utility_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/utility_monitor.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/utility_monitor.o   ../utility_monitor.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/utility_monitor.o 
	
${OBJECTDIR}/_ext/1472/xpl.o: ../xpl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/xpl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/_ext/1472/xpl.o   ../xpl.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/xpl.o 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_ICD3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/utility_monitor.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/18F2580
	${RM} -r dist/18F2580

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
