CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(poweroff-popup C)

SET(PACKAGE ${PROJECT_NAME})

SET(SRCS ${CMAKE_SOURCE_DIR}/poweroff-popup/src/poweroff.c
		 ${CMAKE_SOURCE_DIR}/share/src/common.c)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/poweroff-popup/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/share/src)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
		appcore-efl
		elementary
		evas
		syspopup
		syspopup-caller
		dlog
		sysman
		deviced
		bundle
		vconf
		svi
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag} -g")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2 ")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DPACKAGE=\"${PACAKGE}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${PACKAGE} ${SRCS})
TARGET_LINK_LIBRARIES(${PACKAGE} ${pkgs_LDFLAGS})

INSTALL(TARGETS ${PACKAGE} DESTINATION /usr/apps/org.tizen.poweroff-syspopup/bin)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/org.tizen.poweroff-syspopup.xml DESTINATION /usr/share/packages/)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.poweroff-syspopup.rule DESTINATION /opt/etc/smack/accesses.d)
