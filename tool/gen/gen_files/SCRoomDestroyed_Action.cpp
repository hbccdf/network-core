#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCRoomDestroyed_Action : public SCRoomDestroyed_Msg
    {
        using this_t = SCRoomDestroyed_Action;
        using base_t = SCRoomDestroyed_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCRoomDestroyed_Action);

    void SCRoomDestroyed_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
