#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSPlayerInput_Action : public CSPlayerInput_Msg
    {
        using this_t = CSPlayerInput_Action;
        using base_t = CSPlayerInput_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSPlayerInput_Action);

    void CSPlayerInput_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
