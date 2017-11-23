#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCFactionMsg_Action : public SCFactionMsg_Msg
        using this_t = SCFactionMsg_Action;
        using base_t = SCFactionMsg_Msg;
        SCFactionMsg_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCFactionMsg_Action);

    void SCFactionMsg_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
