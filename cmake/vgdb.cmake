set(HOST_LOCAL 1)

set(VGDB_LINUX 0)
set(VGDB_FILE_MASK "${VGDB_FILE_MASK};*.inc;*.h.in;*.xml;*.tcc")
set(VGDB_SELECT_SOURCE_TARGET "network")
set(VGDB_SELECT_DIR "bin64;cmake;example;network;test;third_party;*")

if(HOST_LOCAL)
    set(VGDB_REMOTE_DIR "/code/server")
    set(VGDB_HOST_NAME "local")
else()
    set(VGDB_REMOTE_DIR "/data/code")
    set(VGDB_HOST_NAME "code")
endif()