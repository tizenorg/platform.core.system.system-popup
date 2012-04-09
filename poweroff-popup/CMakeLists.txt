
#################### Poweroff #########################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(poweroff-popup C)

SET(SRCS ${CMAKE_SOURCE_DIR}/poweroff-popup/src/poweroff.c)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/poweroff-popup)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED appcore-efl elementary devman devman_haptic mm-sound syspopup 
mm-keysound sysman utilX bundle pmapi evas ecore-evas notification vconf)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag} -g")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2 ")

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

ADD_CUSTOM_TARGET(poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/images
		${CMAKE_SOURCE_DIR}/edcs/poweroff.edc ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/edcs/poweroff.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} poweroff.edj)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /opt/apps/org.tizen.poweroff-syspopup/bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/poweroff.edj DESTINATION /opt/apps/org.tizen.poweroff-syspopup/res/edje/poweroff)

# install desktop file & icon
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.desktop.in ${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.desktop)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.desktop DESTINATION /opt/share/applications/)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.png DESTINATION /opt/apps/org.tizen.poweroff-syspopup/res/icon)

# install ini file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.ini DESTINATION /opt/share/process-info)

# i18n
ADD_SUBDIRECTORY(po_poweroff)

