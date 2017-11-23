#include "proto/all_actions.hpp"

namespace CytxGame
{
    class SCSyncCommandMsg_Action : public SCSyncCommandMsg_Msg
        using this_t = SCSyncCommandMsg_Action;
        using base_t = SCSyncCommandMsg_Msg;
        SCSyncCommandMsg_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(SCSyncCommandMsg_Action);

    void SCSyncCommandMsg_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
