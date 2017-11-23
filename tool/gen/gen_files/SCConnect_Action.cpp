#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCConnect_Action : public SCConnect_Msg
        using this_t = SCConnect_Action;
        using base_t = SCConnect_Msg;
        SCConnect_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCConnect_Action);

    void SCConnect_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
