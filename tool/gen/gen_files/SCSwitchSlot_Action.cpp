#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCSwitchSlot_Action : public SCSwitchSlot_Msg
        using this_t = SCSwitchSlot_Action;
        using base_t = SCSwitchSlot_Msg;
        SCSwitchSlot_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCSwitchSlot_Action);

    void SCSwitchSlot_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
