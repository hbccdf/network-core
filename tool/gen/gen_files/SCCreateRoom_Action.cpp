#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCCreateRoom_Action : public SCCreateRoom_Msg
    {
        using this_t = SCCreateRoom_Action;
        using base_t = SCCreateRoom_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCCreateRoom_Action);

    void SCCreateRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
