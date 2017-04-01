
if(NOT THRIFT_DIR)
    if(NOT "$ENV{THRIFT_HOME}" STREQUAL "")
        find_package(Thrift)
    endif()

    if(THRIFT_FOUND)
        if("${THRIFT_LIBRARY_DIR}" STREQUAL "")
            set(THRIFT_LIBRARY_DIR $ENV{THRIFT_LIBRARY_DIR})
        endif()
    
        message("Thrift include path: ${THRIFT_INCLUDE_DIR}")
        message("Thrift library path: ${THRIFT_LIBRARY_DIR}")
        message("Thrift Version: ${THRIFT_VERSION_STRING}")
        
        set(THRIFT_DIR ${THRIFT_INCLUDE_DIR})
        set(THRIFT_LIB_DIR ${THRIFT_LIBRARY_DIR})
    else()
        get_filename_component(THIS_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)
        set(THRIFT_DIR ${THIS_ROOT}/../third_party/thrift-0.10.0)
        set(THRIFT_LIB_DIR ${THIS_ROOT}/../libs64/thrift-0.10.0/thrift-0.10.0/x64)
    endif()
    
    include_directories(${THRIFT_DIR})
    link_directories(${THRIFT_LIB_DIR})
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LIBPATH:\"${THRIFT_LIB_DIR}/Release\"")
endif()