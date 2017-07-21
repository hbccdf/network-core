function(set_target_workdir)
    #message("${ARGC}")
    set(WORK_IDR "$(OutDir)")
    if("${ARGC}" STREQUAL "2")
        #message("two arg")
        set(WORK_IDR ${ARGV1})
        #message("${ARGV0}")
        #message("${ARGV1}")
    endif()
    
    file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.vcxproj.user" 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?> 
<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
    <PropertyGroup Condition=\"'$(Platform)'=='x64'\">
        <LocalDebuggerWorkingDirectory>${WORK_IDR}</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
</Project>")
endfunction()


function(target_use_pch)
    target_sources(${ARGV0} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp)
    target_compile_options(${ARGV0} PUBLIC "/Yu" "/FI stdafx.h")
    set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Yc")
    #set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LIBPATH:\"${THRIFT_LIB_DIR}/Release\"")
    
    #file(READ ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.vcxproj PROJ_STR)
    #string(FIND PROJ_STR "<PrecompiledHeader>NotUsing</PrecompiledHeader>" HAS_NOT_USEING)
    #if(HAS_NOT_USEING)
    #    string(REPLACE "<PrecompiledHeader>NotUsing</PrecompiledHeader>" "<PrecompiledHeader>Use</PrecompiledHeader>" WRITE_PROJ_STR PROJ_STR)
    #    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.vcxproj" ${WRITE_PROJ_STR})
    #endif()
endfunction()

function(optimize_target)
    # generator .brc file, to analize code graph
    #target_compile_options(${ARGV0} PUBLIC "/FR")
endfunction()

macro(add_files)
    file(GLOB NEW_FILES "${ARGV1}/*")
    #message("${NEW_FILES}")
    list(APPEND ${ARGV0} ${NEW_FILES})
    #message("${${ARGV1}}")
    source_group("${ARGV1}" FILES ${NEW_FILES})
endmacro()

macro(add_more_files arg name)
    #message("${name}")
    file(GLOB NEW_FILES ${ARGN})
    list(APPEND ${arg} ${NEW_FILES})
    source_group("${name}" FILES ${NEW_FILES})
endmacro()