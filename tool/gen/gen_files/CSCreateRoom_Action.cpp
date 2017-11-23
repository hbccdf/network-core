#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSCreateRoom_Action : public CSCreateRoom_Msg
        using this_t = CSCreateRoom_Action;
        using base_t = CSCreateRoom_Msg;
        CSCreateRoom_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSCreateRoom_Action);

    void CSCreateRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
