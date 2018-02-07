#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCPlayerInput_Action : public SCPlayerInput_Msg
    {
        using this_t = SCPlayerInput_Action;
        using base_t = SCPlayerInput_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCPlayerInput_Action);

    void SCPlayerInput_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
