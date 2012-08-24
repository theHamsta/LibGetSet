cmake_minimum_required(VERSION 2.8)

set(GETSET_HEADERS "StringUtil.hxx;GetSet.hxx;GetSetDictionary.h;GetSetInternal.h;GetSetFile.h;GetSetXML.h;GetSetCmdLine.h;ObjectFactory.h")

add_library(GetSet
	${GETSET_HEADERS}
	GetSetDictionary.cpp
	GetSetInternal.cpp
	GetSetFile.cpp
	GetSetXML.cpp
	GetSetCmdLine.cpp
	ObjectFactory.cpp
)
set_target_properties(GetSet PROPERTIES VERSION ${GETSET_VERSION})

install(FILES ${GETSET_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/GetSet )

install(TARGETS GetSet EXPORT GetSetExport ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
