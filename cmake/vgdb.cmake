set(HOST_LOCAL 0)

set(VGDB_LINUX 1)
set(VGDB_FILE_MASK "${VGDB_FILE_MASK};*.inc;*.h.in;*.xml")
set(VGDB_SELECT_SOURCE_TARGET "network")
set(VGDB_SELECT_DIR "bin64;cmake;example;network;test;third_party;*")

if(HOST_LOCAL)
    set(VGDB_REMOTE_DIR "/code/server")
    set(VGDB_HOST_NAME "code")
else()
    set(VGDB_REMOTE_DIR "/data/code")
    set(VGDB_HOST_NAME "code")
endif()