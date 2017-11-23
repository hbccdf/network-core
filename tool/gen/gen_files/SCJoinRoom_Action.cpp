#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCJoinRoom_Action : public SCJoinRoom_Msg
        using this_t = SCJoinRoom_Action;
        using base_t = SCJoinRoom_Msg;
        SCJoinRoom_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCJoinRoom_Action);

    void SCJoinRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
