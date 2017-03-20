if(NOT IN_BATCH)
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    get_filename_component(THIS_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)
    set(ROOT ${THIS_ROOT}/..)
    SET(CMAKE_MODULE_PATH ${ROOT}/cmake)
endif()
if(NOT INCLUDED_COMMON)
    message("include common")
    set(LIB_OUTPUT_DIR ${ROOT}/libs64)
    set(BIN_OUTPUT_DIR ${ROOT}/bin64)

    set(EXECUTABLE_OUTPUT_PATH ${BIN_OUTPUT_DIR})
    set(LIBRARY_OUTPUT_PATH ${LIB_OUTPUT_DIR})

    set(COMMON_LIBS Common)
    set(INCLUDE_DIRS 
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR})

    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x601 -D_WINSOCK_DEPRECATED_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DFMT_HEADER_ONLY)
    
    include(boost)
    include(thrift)
    set(THRIFT_LIB_NAME libthrift)

    set(COMMON_DIR ${ROOT}/Common)
    set(EXTS "*.cpp")
    set(PROTOCOL_DIR ${ROOT}/protocol)

    include_directories(
        ${INCLUDE_DIRS}
        ${BOOST_DIR}
        ${ROOT}/third_party
        ${ROOT}/third_party/fmt
        ${ROOT}/third_party/rapidjson/include
        ${ROOT}/third_party/spdlog/include
        )

    link_directories(
        ${LIB_OUTPUT_DIR}
        )
endif()