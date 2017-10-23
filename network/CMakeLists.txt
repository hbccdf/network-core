

file(GLOB SRC "*.h" "*.hpp")

add_files(SRC false "base")
add_files(SRC false "client")
add_files(SRC false "codec")
add_files(SRC false "gameserver")
add_files(SRC false "meta")
add_files(SRC false "net")
add_files(SRC false "orm")
add_files(SRC false "serialize")
add_files(SRC false "server")
add_files(SRC false "timer")
add_files(SRC false "traits")

add_library(network STATIC ${SRC})
set_target_properties(network PROPERTIES LINKER_LANGUAGE CXX)