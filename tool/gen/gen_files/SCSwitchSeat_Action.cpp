#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCSwitchSeat_Action : public SCSwitchSeat_Msg
    {
        using this_t = SCSwitchSeat_Action;
        using base_t = SCSwitchSeat_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCSwitchSeat_Action);

    void SCSwitchSeat_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
