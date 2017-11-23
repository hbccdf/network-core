#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCDisConnect_Action : public SCDisConnect_Msg
        using this_t = SCDisConnect_Action;
        using base_t = SCDisConnect_Msg;
        SCDisConnect_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCDisConnect_Action);

    void SCDisConnect_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
