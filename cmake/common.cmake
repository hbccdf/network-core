if(NOT IN_BATCH)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        message("finded ccahce")
        set(CMAKE_C_COMPILER "ccache")
        set(CMAKE_C_COMPILER_ARG1 "gcc")

        set(CMAKE_CXX_COMPILER "ccache")
        set(CMAKE_CXX_COMPILER_ARG1 "g++")
    endif(CCACHE_FOUND)
        
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    get_filename_component(THIS_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)
    set(ROOT ${THIS_ROOT}/..)
	set(GIT_ROOT ${ROOT})
    SET(CMAKE_MODULE_PATH ${ROOT}/cmake)
endif()

if(NOT INCLUDED_COMMON)
    message("include common")
    if(MSVC)
        set(LIB_OUTPUT_DIR ${ROOT}/libs64)
        set(BIN_OUTPUT_DIR ${ROOT}/bin64)
        #message(${BIN_OUTPUT_DIR})
    else()
        if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
            set(BUILD_CONFIG_NAME "Debug")
        else()
            set(BUILD_CONFIG_NAME "Release")
        endif()
            
        set(LIB_OUTPUT_DIR ${ROOT}/libs64/${BUILD_CONFIG_NAME})
        set(BIN_OUTPUT_DIR ${ROOT}/bin64/${BUILD_CONFIG_NAME})
    endif()
    
    if(NOT ANDROID)
        set(EXECUTABLE_OUTPUT_PATH ${BIN_OUTPUT_DIR})
        set(LIBRARY_OUTPUT_PATH ${LIB_OUTPUT_DIR})
    endif()

    set(COMMON_LIBS )
    set(INCLUDE_DIRS 
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR})
		
	set(GIT_BRANCH "unknown")
    set(GIT_COMMIT_HASH "unknown")
    set(GIT_COMMIT_COUNT ${SUBVERSION})
    set(GIT_MODIFY "")
	
	set(STR_BUILD_TIME "2018-01-18 18:18")
    set(STR_VERSION "1.0.${SUBVERSION}")
	
	if(GAME_VERSION)
        if(EXISTS "${GIT_ROOT}/.git")
            execute_process(
                COMMAND git rev-parse --abbrev-ref HEAD
                WORKING_DIRECTORY ${GIT_ROOT}
                OUTPUT_VARIABLE GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )

            execute_process(
                COMMAND git log -1 --format=%h
                WORKING_DIRECTORY ${GIT_ROOT}
                OUTPUT_VARIABLE GIT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            execute_process(
                COMMAND git rev-list --all --count
                WORKING_DIRECTORY ${GIT_ROOT}
                OUTPUT_VARIABLE GIT_COMMIT_COUNT
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            execute_process(
                COMMAND git status
                WORKING_DIRECTORY ${GIT_ROOT}
                OUTPUT_VARIABLE GIT_STATUS
            )
            
            #message("${GIT_STATUS}")
            
            string(FIND ${GIT_STATUS} "modified:" GIT_MODIFY_STATUS)
            if(GIT_MODIFY_STATUS)
                set(GIT_MODIFY "M")
            else()
                set(GIT_MODIFY "")
            endif()
            
            set(SUBVERSION ${GIT_COMMIT_COUNT})
        endif()
        
        string(TIMESTAMP STR_BUILD_TIME "%Y-%m-%d %H:%M")
        set(STR_VERSION "1.0.${SUBVERSION}")
        
    endif()

    include(boost)
    if(MSVC)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x601 -D_WINSOCK_DEPRECATED_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DFMT_HEADER_ONLY -DNOMINMAX)
     
            
        include(thrift)
        set(THRIFT_LIB_NAME libthrift)
        
        # generator .brc file, to analize code graph
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FR")
        
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IMPLIB:")
        
        #/Wall /WX
        
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  /ZI /Gm")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /Gy /Oi /GL")
        
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IMPLIB:")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /LTCG:incremental")
    else()
        add_definitions(-DGNU_SOURCE -DLINUX -DFMT_HEADER_ONLY -DNOMINMAX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fpermissive -g")
        list(APPEND COMMON_LIBS
            pthread
            boost_thread
            boost_log
            boost_filesystem
            boost_system
            boost_serialization
            boost_program_options
            boost_date_time)
            
        set(THRIFT_LIB_NAME thrift)
        include_directories(${ROOT}/thrift_party/thrift-0.10.0_linux)
    endif()


    include(func)
    set(COMMON_DIR ${ROOT}/Common)
    set(PROTOCOL_DIR ${ROOT}/protocol)

    include_directories(
        ${INCLUDE_DIRS}
        ${BOOST_DIR}
        ${ROOT}
        ${ROOT}/third_party
        ${ROOT}/third_party/fmt
        ${ROOT}/third_party/rapidjson/include
        ${ROOT}/third_party/spdlog/include
        ${ROOT}/third_party/dir_monitor/src
        ${ROOT}/third_party/Boost.Application/include
        )

    link_directories(
        ${LIB_OUTPUT_DIR}
        )
endif()