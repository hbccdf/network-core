function(set_target_workdir)
    if(MSVC)
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
    endif()
endfunction()


function(target_use_pch)
    if(MSVC)
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
    endif()
endfunction()

function(optimize_target)
    # generator .brc file, to analize code graph
    #target_compile_options(${ARGV0} PUBLIC "/FR")
endfunction()

function(setup_target target folder)
    if(${folder} STREQUAL "")
    else()
        set_target_properties(${target} PROPERTIES FOLDER ${folder})
    endif()
    
    target_use_pch(${target})
    set_target_workdir(${target})
    optimize_target(${target})
endfunction()

function(setup_target_folder target folder)
    if(${folder} STREQUAL "")
    else()
        set_target_properties(${target} PROPERTIES FOLDER ${folder})
    endif()
endfunction()

macro(add_files)
    file(GLOB NEW_FILES LIST_DIRECTORIES false "${ARGV2}/*")
    list(APPEND ${ARGV0} ${NEW_FILES})
    if(${ARGV1})
        string(FIND "${ARGV2}" "/" END_POS REVERSE)
        if("${END_POS}" STREQUAL "-1")
        else()
            string(SUBSTRING "${ARGV2}" 0 ${END_POS} INC_DIR)
            include_directories(${INC_DIR})
        endif()
    endif()
    string(REPLACE "/" "\\" NEW_STRING ${ARGV2})
    source_group("${NEW_STRING}" FILES ${NEW_FILES})
endmacro()

macro(add_more_files arg name)
    #message("${name}")
    file(GLOB NEW_FILES LIST_DIRECTORIES false ${ARGN})
    list(APPEND ${arg} ${NEW_FILES})
    source_group("${name}" FILES ${NEW_FILES})
endmacro()

macro(internal_append_files var includePath path begin_path group_path)
    # init variable
    set(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${begin_path}/${path}/*")
    set(NEW_FILES "")
    set(INC_DIR "")
    set(NEW_STRING "")
    set(GROUP_PATH "${group_path}")

    # root path
    if(${path} STREQUAL "*")
        set(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${begin_path}/*")
    # not root path
    else()
        string(REPLACE "/" "\\" NEW_STRING ${path})
        set(GROUP_PATH "${group_path}\\${NEW_STRING}")
    endif()

    file(GLOB NEW_FILES LIST_DIRECTORIES false "${FULL_PATH}")
    list(APPEND ${var} ${NEW_FILES})
    source_group("${GROUP_PATH}" FILES ${NEW_FILES})

    # need include current directory
    if(${includePath})
        string(FIND "${path}" "/" END_POS REVERSE)
        if("${END_POS}" STREQUAL "-1")
        else()
            string(SUBSTRING "${path}" 0 ${END_POS} INC_DIR)
            include_directories("${begin_path}/${INC_DIR}")
        endif()
    endif()
endmacro()

macro(append_files var includePath path)
    internal_append_files(${var} ${includePath} ${path} "include" "header")
    internal_append_files(${var} ${includePath} ${path} "src" "source")
endmacro()