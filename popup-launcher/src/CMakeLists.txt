CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(SRCS ${CMAKE_SOURCE_DIR}/popup-launcher/src/launcher.c
		 ${CMAKE_SOURCE_DIR}/popup-launcher/src/popup.c
		 ${CMAKE_SOURCE_DIR}/popup-launcher/src/noti.c
		 ${CMAKE_SOURCE_DIR}/popup-launcher/src/app.c
		 ${CMAKE_SOURCE_DIR}/share/src/common.c
)

SET(APPNAME "popup-launcher")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/share/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/popup-launcher/src)

SET(PKG_MODULES ${PKG_MODULES}
		edbus
		dlog
		ecore
		syspopup-caller
		syspopup
		appsvc
		notification
		appcore-efl
		elementary
		devman
		sysman
		pmapi
		deviced
		utilX
		bundle
		evas
		vconf
		svi
		libtzplatform-config
)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED ${PKG_MODULES})

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -g -fno-omit-frame-pointer -finstrument-functions")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -g -Wall")

ADD_DEFINITIONS("-DAPPNAME=\"${APPNAME}\"")

ADD_EXECUTABLE(${APPNAME} ${SRCS})
TARGET_LINK_LIBRARIES(${APPNAME} ${pkgs_LDFLAGS} "-ldl")

INSTALL(TARGETS ${APPNAME} DESTINATION bin)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/${APPNAME}/src/org.tizen.system.popup.service DESTINATION /usr/share/dbus-1/services)


INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/datausage_warning.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/led_torch.png          DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sdcard_encryption.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sdcard_decryption.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sdcard_encryption_error.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/sdcard_decryption_error.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/usb.png                DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/battery_full_indi.png  DESTINATION ${SYSTEM_ICONDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/battery_full_noti.png  DESTINATION ${SYSTEM_ICONDIR})
