#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCBattleResult_Action : public SCBattleResult_Msg
    {
        using this_t = SCBattleResult_Action;
        using base_t = SCBattleResult_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCBattleResult_Action);

    void SCBattleResult_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
