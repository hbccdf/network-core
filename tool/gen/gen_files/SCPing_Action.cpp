#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCPing_Action : public SCPing_Msg
    {
        using this_t = SCPing_Action;
        using base_t = SCPing_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCPing_Action);

    void SCPing_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
