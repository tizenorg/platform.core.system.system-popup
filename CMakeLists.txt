####### System-popup #############
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(system-popup C)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(SYSTEM_RESDIR "${PREFIX}/share/system-apps/res")
SET(SYSTEM_ICONDIR "${SYSTEM_RESDIR}/icons")

# SUbmodules
ADD_SUBDIRECTORY(po)
ADD_SUBDIRECTORY(poweroff-popup)
ADD_SUBDIRECTORY(lowbatt-popup)
ADD_SUBDIRECTORY(lowmem-popup)
ADD_SUBDIRECTORY(mmc-popup)
ADD_SUBDIRECTORY(usb-popup)
ADD_SUBDIRECTORY(usbotg-popup)

############## END ##############
