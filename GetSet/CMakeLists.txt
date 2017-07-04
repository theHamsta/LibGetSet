cmake_minimum_required(VERSION 2.8)

set(GETSET_HEADERS "BaseTypes.hxx;TypeString.hxx;StringUtil.hxx;GetSet.hxx;GetSetSection.hxx;GetSet_impl.hxx;GetSetCmdLine.hxx;GetSetScripting.h;GetSetDictionary.h;GetSetInternal.h;GetSetIO.h")

add_library(GetSet
	${GETSET_HEADERS}
	GetSetDictionary.cpp
	GetSetInternal.cpp
	GetSetIO.cpp
	GetSetScripting.cpp
)
set_target_properties(GetSet PROPERTIES VERSION ${GETSET_VERSION})
install(FILES ${GETSET_HEADERS} DESTINATION include/GetSet)
install(TARGETS GetSet EXPORT GetSetExport DESTINATION lib)
