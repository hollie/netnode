#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-v3.mk)" "nbproject/Makefile-local-v3.mk"
include nbproject/Makefile-local-v3.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=v3
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=lib
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=lib
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/cosm_socket.o ${OBJECTDIR}/clock.o ${OBJECTDIR}/eeprom.o ${OBJECTDIR}/oo.o ${OBJECTDIR}/shtxx.o
POSSIBLE_DEPFILES=${OBJECTDIR}/cosm_socket.o.d ${OBJECTDIR}/clock.o.d ${OBJECTDIR}/eeprom.o.d ${OBJECTDIR}/oo.o.d ${OBJECTDIR}/shtxx.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/cosm_socket.o ${OBJECTDIR}/clock.o ${OBJECTDIR}/eeprom.o ${OBJECTDIR}/oo.o ${OBJECTDIR}/shtxx.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-v3.mk dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F45K80
MP_PROCESSOR_OPTION_LD=18f45k80
MP_LINKER_DEBUG_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/cosm_socket.o: cosm_socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cosm_socket.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/cosm_socket.o   cosm_socket.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cosm_socket.o 
	@${FIXDEPS} "${OBJECTDIR}/cosm_socket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/clock.o: clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/clock.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/clock.o   clock.c 
	@${DEP_GEN} -d ${OBJECTDIR}/clock.o 
	@${FIXDEPS} "${OBJECTDIR}/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/eeprom.o: eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/eeprom.o   eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/oo.o: oo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/oo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/oo.o   oo.c 
	@${DEP_GEN} -d ${OBJECTDIR}/oo.o 
	@${FIXDEPS} "${OBJECTDIR}/oo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/shtxx.o: shtxx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/shtxx.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/shtxx.o   shtxx.c 
	@${DEP_GEN} -d ${OBJECTDIR}/shtxx.o 
	@${FIXDEPS} "${OBJECTDIR}/shtxx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/cosm_socket.o: cosm_socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/cosm_socket.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/cosm_socket.o   cosm_socket.c 
	@${DEP_GEN} -d ${OBJECTDIR}/cosm_socket.o 
	@${FIXDEPS} "${OBJECTDIR}/cosm_socket.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/clock.o: clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/clock.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/clock.o   clock.c 
	@${DEP_GEN} -d ${OBJECTDIR}/clock.o 
	@${FIXDEPS} "${OBJECTDIR}/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/eeprom.o: eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/eeprom.o   eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/oo.o: oo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/oo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/oo.o   oo.c 
	@${DEP_GEN} -d ${OBJECTDIR}/oo.o 
	@${FIXDEPS} "${OBJECTDIR}/oo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/shtxx.o: shtxx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/shtxx.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)  --extended -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/shtxx.o   shtxx.c 
	@${DEP_GEN} -d ${OBJECTDIR}/shtxx.o 
	@${FIXDEPS} "${OBJECTDIR}/shtxx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) -c dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  
else
dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) -c dist/${CND_CONF}/${IMAGE_TYPE}/likalibs.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/v3
	${RM} -r dist/v3

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
