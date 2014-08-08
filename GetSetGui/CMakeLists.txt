# Qt requires moc-ing
set(GetSetGui_QOBJ_H GetSetWidget.h GetSetSettingsWindow.h )
#QT5_WRAP_CPP(GetSetGui_MOC ${GetSetGui_QOBJ_H})

set(GETSETGUI_HEADERS "GetSetGui.h;${GetSetGui_QOBJ_H}")

# build library
add_library(GetSetGui STATIC
	${GETSETGUI_HEADERS}
	GetSetWidget.cpp
	GetSetSettingsWindow.cpp
	GetSetGui.cpp
)
qt5_use_modules(GetSetGui Widgets)
set_target_properties(GetSetGui PROPERTIES VERSION ${GETSET_VERSION})

install(FILES ${GETSETGUI_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/GetSetGui )

install(TARGETS GetSetGui EXPORT GetSetExport ARCHIVE DESTINATION GetSet/${LIB_INSTALL_DIR})
