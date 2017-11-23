#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCRequestRoomList_Action : public SCRequestRoomList_Msg
        using this_t = SCRequestRoomList_Action;
        using base_t = SCRequestRoomList_Msg;
        SCRequestRoomList_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCRequestRoomList_Action);

    void SCRequestRoomList_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
