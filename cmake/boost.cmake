
if(NOT BOOST_DIR)
    if(NOT "$ENV{BOOST_ROOT}" STREQUAL "")
        find_package(Boost 1.61.0)
    endif()

    if(Boost_FOUND)
        if("${Boost_LIBRARY_DIRS}" STREQUAL "")
            set(Boost_LIBRARY_DIRS $ENV{BOOST_LIBRARY_DIR})
        endif()
    
        message("Boost include path: ${Boost_INCLUDE_DIRS}")
        message("Boost library path: ${Boost_LIBRARY_DIRS}")
        message("Boost Version: ${Boost_VERSION}")
        
        set(BOOST_DIR ${Boost_INCLUDE_DIRS})
        set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
    else()
        get_filename_component(THIS_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)
        set(BOOST_DIR ${THIS_ROOT}/../third_party/boost_1_61_0)
        set(BOOST_LIB_DIR ${THIS_ROOT}/../libs64/boost_1_61_0/windows/x64)
    endif()
    
    include_directories(${BOOST_DIR})
    link_directories(${BOOST_LIB_DIR})
    
endif()