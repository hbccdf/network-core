include(../../cmake/common.cmake)
include_directories(
    ${ROOT}/third_party/googletest-release-1.8.0/googlemock/include
	${ROOT}/third_party/googletest-release-1.8.0/googletest/include
    ${ROOT}/third_party/CppFreeMock)

add_executable(common_test main.cpp
    se_test.cpp
    xml_test.cpp
    enum_test.cpp
    info_test.cpp
    gos_test.cpp
    base_test.cpp
    program_options_test.cpp
    )
set_target_properties(common_test PROPERTIES FOLDER "test")
target_link_libraries(common_test ${COMMON_LIBS} gmock)