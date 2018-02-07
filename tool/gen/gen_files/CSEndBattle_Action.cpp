#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSEndBattle_Action : public CSEndBattle_Msg
    {
        using this_t = CSEndBattle_Action;
        using base_t = CSEndBattle_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSEndBattle_Action);

    void CSEndBattle_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
