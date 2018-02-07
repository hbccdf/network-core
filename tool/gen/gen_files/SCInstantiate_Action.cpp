#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCInstantiate_Action : public SCInstantiate_Msg
    {
        using this_t = SCInstantiate_Action;
        using base_t = SCInstantiate_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCInstantiate_Action);

    void SCInstantiate_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
