CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(SRCS src/usb-syspopup.c)

SET(VENDOR "tizen")
SET(PACKAGE "usb-syspopup")
SET(PKGNAME "org.${VENDOR}.${PACKAGE}")
SET(PREFIX "${TZ_SYS_RO_APP}/${PKGNAME}")
SET(BINDIR "${PREFIX}/bin")
SET(RESDIR "${PREFIX}/res")
SET(LOCALEDIR "${RESDIR}/locale")
SET(MANIFESTDIR "${TZ_SYS_RO_PACKAGES}")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
				appcore-efl
				bundle
				dlog
				ecore
				elementary
				evas
				syspopup
				devman
				appsvc
				pmapi
				vconf
				libtzplatform-config)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -O2 -g -Wall")

ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${PACKAGE} ${SRCS})
TARGET_LINK_LIBRARIES(${PACKAGE} syspopup  ${pkgs_LDFLAGS})

INSTALL(TARGETS ${PACKAGE} DESTINATION ${BINDIR})

# install manifest file
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/org.tizen.usb-syspopup.xml.in ${CMAKE_SOURCE_DIR}/data/org.tizen.usb-syspopup.xml)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/${PKGNAME}.xml DESTINATION ${MANIFESTDIR})

# instatll rule file
INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.usb-syspopup.rule DESTINATION ${TZ_SYS_SMACK}/accesses.d)
