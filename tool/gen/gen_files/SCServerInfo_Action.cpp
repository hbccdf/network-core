#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCServerInfo_Action : public SCServerInfo_Msg
        using this_t = SCServerInfo_Action;
        using base_t = SCServerInfo_Msg;
        SCServerInfo_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCServerInfo_Action);

    void SCServerInfo_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
