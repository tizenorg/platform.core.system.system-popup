
#################### Low-battery ###############################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(lowbatt-popup C)

SET(SRCS ${CMAKE_SOURCE_DIR}/lowbatt-popup/src/lowbatt.c)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lowbatt-popup)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED appcore-efl elementary devman devman_haptic mm-sound notification sysman)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag} -g")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g ")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

ADD_CUSTOM_TARGET(lowbatt.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/../images
		${CMAKE_SOURCE_DIR}/edcs/lowbatt.edc ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lowbatt.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/edcs/lowbatt.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} lowbatt.edj)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /opt/apps/org.tizen.lowbat-syspopup/bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lowbatt.edj DESTINATION /opt/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt)

# install desktop file & icon
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/org.tizen.lowbat-syspopup.desktop.in ${CMAKE_SOURCE_DIR}/data/org.tizen.lowbat-syspopup.desktop)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.lowbat-syspopup.desktop DESTINATION /opt/share/applications/)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.lowbat-syspopup.png DESTINATION /opt/apps/org.tizen.lowbat-syspopup/res/icon)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/sound/09_Low_Battery.wav DESTINATION /opt/apps/org.tizen.lowbat-syspopup/res/keysound)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/batt_full_icon.png DESTINATION /opt/apps/org.tizen.lowbat-syspopup/res/icons/)

# install ini file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.ini DESTINATION /opt/share/process-info)

# i18n
ADD_SUBDIRECTORY(po_lowbatt)

###############################################################


