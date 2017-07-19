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

# Basic Deployment. Note: for full support of Qt5, you should still use windeployqt.
if (WIN32)
	foreach(DLL Core Gui Widgets)
		get_target_property(Qt5_${DLL}Location      Qt5::${DLL} LOCATION)
		get_target_property(Qt5_${DLL}LocationDebug Qt5::${DLL} IMPORTED_LOCATION_DEBUG)
	endforeach(DLL)
	add_custom_command(TARGET GetSetGui POST_BUILD	
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt5_CoreLocationDebug}>    $<$<NOT:$<CONFIG:Debug>>:${Qt5_CoreLocation}>    $<TARGET_FILE_DIR:GetSet>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt5_GuiLocationDebug}>     $<$<NOT:$<CONFIG:Debug>>:${Qt5_GuiLocation}>     $<TARGET_FILE_DIR:GetSet>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt5_WidgetsLocationDebug}> $<$<NOT:$<CONFIG:Debug>>:${Qt5_WidgetsLocation}> $<TARGET_FILE_DIR:GetSet>
	)
	install(FILES ${Qt5_CoreLocationDebug} ${Qt5_GuiLocationDebug} ${Qt5_WidgetsLocationDebug}  CONFIGURATIONS Debug   DESTINATION bin )
	install(FILES ${Qt5_CoreLocation}      ${Qt5_GuiLocation}      ${Qt5_WidgetsLocation}       CONFIGURATIONS Release DESTINATION bin )
	install(FILES $<TARGET_FILE:GetSetGui> CONFIGURATIONS Debug   DESTINATION bin )
	install(FILES $<TARGET_FILE:GetSetGui> CONFIGURATIONS Release DESTINATION bin )
endif(WIN32)
