#include "proto/all_wraps.hpp"

namespace CytxGame
{/*
    REGISTER_PROTOCOL(CSPing_Wrap);

    void CSPing_Wrap::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        SCPing_Wrap data_wrap;
        server.send_client_msg(msgp->header().user_id, data_wrap);
    }*/


    class CSPing_Process : public CSPing_Wrap
    {

    public:
        virtual proto_ptr_t clone() override
        {
            return std::make_shared<CSPing_Process>();
        }


        virtual void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;

    };


    REGISTER_PROTOCOL(CSPing_Process);

    void CSPing_Process::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        SCPing_Wrap data_wrap;
        server.send_client_msg(msgp->header().user_id, data_wrap);
    }
}
