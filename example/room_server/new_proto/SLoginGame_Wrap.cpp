#include "proto_wraps.hpp"

namespace CytxGame
{
    REGISTER_PROTOCOL(SLoginGame_Wrap);

    void SLoginGame_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr)
    {
        int32_t user_id = msgp->header().user_id;
        LOG_DEBUG("user {} login", user_id);

        SCServerInfo_Wrap data;
        data.scServerInfo_.currentServerTime = cytx::date_time::now().total_milliseconds();

    }
}