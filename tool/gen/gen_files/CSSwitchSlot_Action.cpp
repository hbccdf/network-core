#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSSwitchSlot_Action : public CSSwitchSlot_Msg
    {
        using this_t = CSSwitchSlot_Action;
        using base_t = CSSwitchSlot_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSSwitchSlot_Action);

    void CSSwitchSlot_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
