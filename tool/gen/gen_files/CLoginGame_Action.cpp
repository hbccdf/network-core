#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CLoginGame_Action : public CLoginGame_Msg
        using this_t = CLoginGame_Action;
        using base_t = CLoginGame_Msg;
        CLoginGame_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CLoginGame_Action);

    void CLoginGame_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
