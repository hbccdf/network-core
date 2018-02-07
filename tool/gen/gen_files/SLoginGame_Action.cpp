#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SLoginGame_Action : public SLoginGame_Msg
    {
        using this_t = SLoginGame_Action;
        using base_t = SLoginGame_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SLoginGame_Action);

    void SLoginGame_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
