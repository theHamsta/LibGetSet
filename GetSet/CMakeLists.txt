cmake_minimum_required(VERSION 2.8)

set(GETSET_HEADERS "BaseTypes.hxx;TypeString.hxx;StringUtil.hxx;GetSet.hxx;GetSet_impl.hxx;GetSetCmdLine.hxx;GetSetScripting.h;GetSetInternal.h;GetSetIO.h")

add_library(GetSet
	${GETSET_HEADERS}
	GetSetInternal.cpp
	GetSetIO.cpp
	GetSetScripting.cpp
)
set_target_properties(GetSet PROPERTIES VERSION ${GETSET_VERSION})

install(FILES ${GETSET_HEADERS} DESTINATION include/GetSet)
install(TARGETS GetSet EXPORT GetSetExport DESTINATION lib)

if (WIN32)
	install(FILES $<TARGET_FILE:GetSet> CONFIGURATIONS Debug   DESTINATION bin )
	install(FILES $<TARGET_FILE:GetSet> CONFIGURATIONS Release DESTINATION bin )
endif(WIN32)