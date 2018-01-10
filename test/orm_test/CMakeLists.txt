include(../../cmake/common.cmake)
include_directories(${ROOT}
    ${ROOT}/third_party/googletest-release-1.8.0/googlemock/include
	${ROOT}/third_party/googletest-release-1.8.0/googletest/include
    ${ROOT}/third_party/CppFreeMock)

add_executable(orm_test main.cpp 
    expr_test.cpp)
set_target_properties(orm_test PROPERTIES FOLDER "test")
target_link_libraries(orm_test ${COMMON_LIBS} gmock)