#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSSwitchSeat_Action : public CSSwitchSeat_Msg
        using this_t = CSSwitchSeat_Action;
        using base_t = CSSwitchSeat_Msg;
        CSSwitchSeat_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSSwitchSeat_Action);

    void CSSwitchSeat_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
