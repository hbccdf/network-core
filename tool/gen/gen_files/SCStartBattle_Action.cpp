#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCStartBattle_Action : public SCStartBattle_Msg
    {
        using this_t = SCStartBattle_Action;
        using base_t = SCStartBattle_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCStartBattle_Action);

    void SCStartBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
