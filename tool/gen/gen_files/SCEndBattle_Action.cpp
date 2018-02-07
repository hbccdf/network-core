#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCEndBattle_Action : public SCEndBattle_Msg
    {
        using this_t = SCEndBattle_Action;
        using base_t = SCEndBattle_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCEndBattle_Action);

    void SCEndBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
