#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSJoinRoom_Action : public CSJoinRoom_Msg
        using this_t = CSJoinRoom_Action;
        using base_t = CSJoinRoom_Msg;
        CSJoinRoom_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSJoinRoom_Action);

    void CSJoinRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
