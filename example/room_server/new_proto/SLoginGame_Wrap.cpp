#include "proto/message_wraps.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(SLoginGame_Wrap);

    void SLoginGame_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        int32_t user_id = msgp->header().user_id;
        LOG_DEBUG("user {} login", user_id);

        SCServerInfo_Wrap data;
        data.scServerInfo.currentServerTime = cytx::date_time::now().total_milliseconds();
        server.send_client_msg(user_id, data);
    }
}
