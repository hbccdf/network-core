#pragma once
#include <cstdint>
#include "../meta/meta.hpp"

namespace cytx {
    // 4 byte
    struct full_procotol
    {
        uint16_t id;
        uint8_t server_type;
        bool need_reply : 1;
        bool is_reply : 1;
        uint8_t game_type : 6;
    };

    union common_protocol
    {
        uint32_t id;
        full_procotol full_proto;
    };

    inline uint32_t to_protocol(full_procotol proto)
    {
        common_protocol cp;
        cp.full_proto = proto;
        return cp.id;
    }

    inline full_procotol to_protocol(uint32_t id)
    {
        common_protocol cp;
        cp.id = id;
        return cp.full_proto;
    }

    inline uint32_t reply_protocol(uint32_t proto)
    {
        full_procotol full = to_protocol(proto);
        full.is_reply = true;
        full.need_reply = false;
        return to_protocol(full);
    }

    template<typename enum_t>
    inline auto reply_protocol(enum_t proto)->std::enable_if_t<std::is_enum<enum_t>::value, enum_t>
    {
        return (enum_t)reply_protocol((uint32_t)proto);
    }

    enum class result_code : uint16_t
    {
        ok = 0,
        not_reply = 99,
        fail = 100,
    };

    enum class error_code
    {
        ok,
        fail = (int)result_code::fail,
        unknown,
        timeout,
        cancel,
        connect_fail,
        badconnect,
        codec_fail,
        invalid_header,
        be_disconnected,
        repeat_connect,
        no_handler,
        remote_error,
        recv_error,
        send_error,
    };
    REG_ENUM(error_code, ok, unknown, fail, timeout, cancel, connect_fail, badconnect, codec_fail, invalid_header, be_disconnected,
        repeat_connect, no_handler, remote_error, recv_error, send_error);
}

