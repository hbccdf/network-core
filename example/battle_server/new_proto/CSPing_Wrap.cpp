#include "proto/all_wraps.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(CSPing_Wrap);

    void CSPing_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        SCPing_Wrap data_wrap;
        server.send_client_msg(msgp->header().user_id, data_wrap);
    }
}
