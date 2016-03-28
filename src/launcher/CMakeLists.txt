CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(LAUNCHER_DIR "${CMAKE_SOURCE_DIR}/src/launcher")
SET(APPNAME "sysapp-launcher")

SET(SRCS ${LAUNCHER_DIR}/launcher.c
		 ${LAUNCHER_DIR}/popup.c
)

INCLUDE_DIRECTORIES(${COMMON_DIR})

SET(PKG_MODULES
		edbus
		dlog
		ecore
		elementary
		syspopup-caller
		appcore-common
)

INCLUDE(FindPkgConfig)
pkg_check_modules(launcher_pkgs REQUIRED ${PKG_MODULES})

FOREACH(flag ${launcher_pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -g -fno-omit-frame-pointer -finstrument-functions")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -g -Wall")

ADD_DEFINITIONS("-DAPPNAME=\"${APPNAME}\"")

ADD_EXECUTABLE(${APPNAME} ${SRCS})
TARGET_LINK_LIBRARIES(${APPNAME} ${launcher_pkgs_LDFLAGS} "-ldl")

INSTALL(TARGETS ${APPNAME} DESTINATION bin)
INSTALL(FILES ${LAUNCHER_DIR}/org.tizen.system.popup.service DESTINATION ${DBUS_SERVICE_DIR})
INSTALL(FILES ${LAUNCHER_DIR}/launcher.conf DESTINATION /etc/dbus-1/system.d)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${LICENSE_DIR} RENAME ${APPNAME})
