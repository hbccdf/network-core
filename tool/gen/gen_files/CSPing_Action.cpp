#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSPing_Action : public CSPing_Msg
    {
        using this_t = CSPing_Action;
        using base_t = CSPing_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSPing_Action);

    void CSPing_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
