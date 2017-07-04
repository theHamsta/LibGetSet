# Choose public headers
set(GETSETGUI_HEADERS "GetSetGui.h;ProgressInterface.hxx;GetSetWidget.h;GetSetTabWidget.h;GetSetProgressWindow.h;GetSetScriptEdit.h")
# Build library
add_library(GetSetGui
	${GETSETGUI_HEADERS}
	GetSetGui.cpp
	GetSetWidget.cpp
	GetSetTabWidget.cpp
	GetSetProgressWindow.cpp
	GetSetScriptEdit.cpp
	GetSetModalDialog.cpp
)
target_link_libraries(GetSetGui GetSet)
qt5_use_modules(GetSetGui Widgets)
set_target_properties(GetSetGui PROPERTIES VERSION ${GETSET_VERSION})
# Install library
install(FILES ${GETSETGUI_HEADERS} DESTINATION include/GetSetGui)
install(TARGETS GetSetGui EXPORT GetSetExport DESTINATION lib)
