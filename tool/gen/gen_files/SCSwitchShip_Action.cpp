#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCSwitchShip_Action : public SCSwitchShip_Msg
        using this_t = SCSwitchShip_Action;
        using base_t = SCSwitchShip_Msg;
        SCSwitchShip_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCSwitchShip_Action);

    void SCSwitchShip_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
