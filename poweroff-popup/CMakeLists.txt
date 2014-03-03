
#################### Poweroff #########################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(poweroff-popup C)

SET(PACKAGE ${PROJECT_NAME})

SET(SRCS ${CMAKE_SOURCE_DIR}/poweroff-popup/src/poweroff.c
		 ${CMAKE_SOURCE_DIR}/share/src/common.c)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/poweroff-popup/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/share/src)
INCLUDE_DIRECTORIES(/usr/include/svi)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED appcore-efl elementary syspopup syspopup-caller dlog devman sysman pmapi
deviced utilX bundle evas ecore-evas notification vconf ecore-input appsvc)

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
ADD_DEFINITIONS("-DPACKAGE=\"${PACAKGE}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS} "-lsvi")

ADD_CUSTOM_TARGET(poweroff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/edcs/icons
		${CMAKE_SOURCE_DIR}/edcs/poweroff.edc ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/poweroff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/edcs/poweroff.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} poweroff.edj)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/apps/org.tizen.poweroff-syspopup/bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/poweroff.edj DESTINATION /usr/apps/org.tizen.poweroff-syspopup/res/edje/poweroff)

# install desktop file & icon
SET(PREFIX ${CMAKE_INSTALL_PREFIX})

# install manifest xml file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.xml DESTINATION /usr/share/packages/)

# instatll rule file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.poweroff-syspopup.rule DESTINATION /opt/etc/smack/accesses.d)

# i18n
ADD_SUBDIRECTORY(po_poweroff)

