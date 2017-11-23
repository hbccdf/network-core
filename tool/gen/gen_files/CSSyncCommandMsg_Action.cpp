#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSSyncCommandMsg_Action : public CSSyncCommandMsg_Msg
        using this_t = CSSyncCommandMsg_Action;
        using base_t = CSSyncCommandMsg_Msg;
        CSSyncCommandMsg_Action(const this_t& rhs)
            : base_t(rhs)
        {}
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>(*this);
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSSyncCommandMsg_Action);

    void CSSyncCommandMsg_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {

    }
}
