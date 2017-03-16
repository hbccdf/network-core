file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" , "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
source_group("头文件" FILES ${HEADERS})

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group("源文件" FILES ${SRC_FILES})
