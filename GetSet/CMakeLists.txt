cmake_minimum_required(VERSION 2.8)

set(GETSET_HEADERS "BaseTypes.hxx;TypeString.hxx;StringUtil.hxx;GetSet.hxx;GetSet_impl.hxx;GetSetDictionary.h;GetSetInternal.h;GetSetIO.h;GetSetXML.h")

add_library(GetSet
	${GETSET_HEADERS}
	GetSetDictionary.cpp
	GetSetInternal.cpp
	GetSetIO.cpp
	GetSetXML.cpp
)
set_target_properties(GetSet PROPERTIES VERSION ${GETSET_VERSION})

install(FILES ${GETSET_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/GetSet )

install(TARGETS GetSet EXPORT GetSetExport ARCHIVE DESTINATION GetSet/${LIB_INSTALL_DIR})

