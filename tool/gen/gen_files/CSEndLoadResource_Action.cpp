#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSEndLoadResource_Action : public CSEndLoadResource_Msg
    {
        using this_t = CSEndLoadResource_Action;
        using base_t = CSEndLoadResource_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSEndLoadResource_Action);

    void CSEndLoadResource_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
