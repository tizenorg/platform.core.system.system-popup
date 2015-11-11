# for i18n

INCLUDE(po_list)

SET(MSGFMT "/usr/bin/msgfmt")

FOREACH(pofile ${POFILES})
	SET(pofile ${CMAKE_CURRENT_SOURCE_DIR}/${pofile})
	MESSAGE("PO: ${pofile}")
	GET_FILENAME_COMPONENT(absPofile ${pofile} ABSOLUTE)
	GET_FILENAME_COMPONENT(lang ${absPofile} NAME_WE)
	SET(moFile ${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo)
	ADD_CUSTOM_COMMAND(
			OUTPUT ${moFile}
			COMMAND ${MSGFMT} -o ${moFile} ${absPofile}
			DEPENDS ${absPofile}
	)
	INSTALL(FILES ${moFile}
			DESTINATION ${LOCALE_DIR}/${lang}/LC_MESSAGES RENAME ${PKGNAME}.mo)
	SET(moFiles ${moFiles} ${moFile})
ENDFOREACH(pofile)

MESSAGE(".mo files: ${moFiles}")
ADD_CUSTOM_TARGET(po_${PKGNAME} ALL DEPENDS ${moFiles})
