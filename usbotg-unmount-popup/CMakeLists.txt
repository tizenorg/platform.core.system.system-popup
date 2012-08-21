
########################### usbotg-unmount ###########################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(usbotg-unmount-popup C)

SET(SRCS ${CMAKE_SOURCE_DIR}/usbotg-unmount-popup/src/usbotg-unmount.c)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/usbotg-unmount-popup)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED appcore-efl elementary devman devman_haptic mm-sound sysman syspopup syspopup-caller ecore-evas appsvc)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag} -g -I/usr/include/elementary-0 ")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
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

ADD_CUSTOM_TARGET(usbotg-unmount.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/../images
		${CMAKE_SOURCE_DIR}/edcs/usbotg-unmount.edc ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/usbotg-unmount.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/edcs/usbotg-unmount.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} usbotg-unmount.edj)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /opt/apps/org.tizen.usbotg-unmount-popup/bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/usbotg-unmount.edj DESTINATION /opt/apps/org.tizen.usbotg-unmount-popup/res/edje/usbotg-unmount)

# install desktop file & icon
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.usbotg-unmount-popup.png DESTINATION /opt/apps/org.tizen.usbotg-unmount-popup/res/icon)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/sound/02_Warning.wav DESTINATION /opt/apps/org.tizen.usbotg-unmount-popup/res/keysound)

# install manifest xml file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.usbotg-unmount-popup.xml DESTINATION /opt/share/packages/)

# install ini file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.ini DESTINATION /opt/share/process-info)

# i18n
ADD_SUBDIRECTORY(po_usbotg_unmount)

################################# End ##############################################

