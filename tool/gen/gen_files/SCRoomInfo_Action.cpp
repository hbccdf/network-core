#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCRoomInfo_Action : public SCRoomInfo_Msg
    {
        using this_t = SCRoomInfo_Action;
        using base_t = SCRoomInfo_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCRoomInfo_Action);

    void SCRoomInfo_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
