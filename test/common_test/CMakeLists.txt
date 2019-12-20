include(../../cmake/common.cmake)
include_directories(
    ${ROOT}/third_party/googletest-release-1.8.0/googlemock/include
	${ROOT}/third_party/googletest-release-1.8.0/googletest/include
    ${ROOT}/third_party/CppFreeMock
	${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "serialize")
add_files(SRC false "util")

add_executable(common_test ${SRC})
set_target_properties(common_test PROPERTIES FOLDER "test")
target_link_libraries(common_test ${COMMON_LIBS} gmock)