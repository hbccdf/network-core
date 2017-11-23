#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSExitRoom_Action : public CSExitRoom_Msg
        using this_t = CSExitRoom_Action;
        using base_t = CSExitRoom_Msg;
        CSExitRoom_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSExitRoom_Action);

    void CSExitRoom_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
