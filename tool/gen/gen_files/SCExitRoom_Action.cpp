#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCExitRoom_Action : public SCExitRoom_Msg
        using this_t = SCExitRoom_Action;
        using base_t = SCExitRoom_Msg;
        SCExitRoom_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCExitRoom_Action);

    void SCExitRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
