#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCBeginLoadResource_Action : public SCBeginLoadResource_Msg
        using this_t = SCBeginLoadResource_Action;
        using base_t = SCBeginLoadResource_Msg;
        SCBeginLoadResource_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCBeginLoadResource_Action);

    void SCBeginLoadResource_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
