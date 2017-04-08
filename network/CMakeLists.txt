
file(GLOB BASE_FILES "base/*.h" "base/*.hpp")
file(GLOB CLIENT_FILES "client/*.h" "client/*.hpp")
file(GLOB SERVER_FILES "server/*.h" "server/*.hpp")
file(GLOB SERIALIZE_FILES "serialize/*.h" "serialize/*.hpp")
file(GLOB NET_FILES "net/*.h" "net/*.hpp")
file(GLOB TRAIT_FILES "traits/*.h" "traits/*.hpp")
file(GLOB CODEC_FILES "codec/*.h" "codec/*.hpp")
file(GLOB ORM_FILES "orm/*.h" "orm/*.hpp")

file(GLOB HEADERS "*.h" "*.hpp")

set(ALL_FILES ${BASE_FILES} ${CLIENT_FILES} ${SERVER_FILES} ${SERIALIZE_FILES} ${NET_FILES} ${TRAIT_FILES} ${CODEC_FILES} ${ORM_FILES} ${HEADERS})

add_library(network STATIC ${ALL_FILES})
set_target_properties(network PROPERTIES LINKER_LANGUAGE CXX)

source_group("files\\base" FILES ${BASE_FILES})
source_group("files\\client" FILES ${CLIENT_FILES})
source_group("files\\server" FILES ${SERVER_FILES})
source_group("files\\serialize" FILES ${SERIALIZE_FILES})
source_group("files\\net" FILES ${NET_FILES})
source_group("files\\traits" FILES ${TRAIT_FILES})
source_group("files\\codec" FILES ${CODEC_FILES})
source_group("files\\orm" FILES ${ORM_FILES})
source_group("files" FILES ${HEADERS})