# Choose public headers
set(GETSETGUI_HEADERS "Configurator.hxx;Initializer.hxx;ObjectFactory.h")
# Build library
add_library(GetSetObjects
	${GETSETGUI_HEADERS}
	ObjectFactory.cpp
)
target_link_libraries(GetSetObjects GetSet)
set_target_properties(GetSetObjects PROPERTIES VERSION ${GETSET_VERSION})
# Install library
install(FILES ${GETSETGUI_HEADERS} DESTINATION include/GetSetObjects)
install(TARGETS GetSetObjects EXPORT GetSetExport DESTINATION lib)
