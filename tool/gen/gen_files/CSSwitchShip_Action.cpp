#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSSwitchShip_Action : public CSSwitchShip_Msg
        using this_t = CSSwitchShip_Action;
        using base_t = CSSwitchShip_Msg;
        CSSwitchShip_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSSwitchShip_Action);

    void CSSwitchShip_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
