# Choose public headers
set(GETSETGUI_HEADERS "GetSetGui.h;GetSetWidget.h;GetSetSettingsWindow.h;GetSetProgressWindow.h")
# Build library
add_library(GetSetGui
	${GETSETGUI_HEADERS}
	GetSetWidget.cpp
	GetSetSettingsWindow.cpp
	GetSetProgressWindow.cpp
	GetSetGui.cpp
)
target_link_libraries(GetSetGui GetSet)
qt5_use_modules(GetSetGui Widgets)
set_target_properties(GetSetGui PROPERTIES VERSION ${GETSET_VERSION})
# Install library
install(FILES ${GETSETGUI_HEADERS} DESTINATION include/GetSetGui)
install(TARGETS GetSetGui EXPORT GetSetExport DESTINATION lib)
