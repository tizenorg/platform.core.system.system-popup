CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(system-cooperator C)

IF(SYSTEM_COOPERATOR STREQUAL off)
	RETURN()
ENDIF(SYSTEM_COOPERATOR STREQUAL off)

SET(SRCS ${CMAKE_SOURCE_DIR}/system-cooperator/system-cooperator.c)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(PKG_MODULES
		appcore-efl
		glib-2.0
		vconf
		dlog
		feedback
		capi-media-sound-manager
		capi-media-wav-player
)
INCLUDE(FindPkgConfig)
pkg_check_modules(system-cooperator_pkgs REQUIRED ${PKG_MODULES})

FOREACH(flag ${system-cooperator_pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag} -g")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIE")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g ")
SET(CMAKE_C_FLAGS_RELEASE "-O2")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

ADD_DEFINITIONS("-DAPPNAME=\"${PROJECT}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${system-cooperator_pkgs_LDFLAGS})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
