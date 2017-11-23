#include "proto/all_actions.hpp"

namespace CytxGame
{
    class CSRequestRoomList_Action : public CSRequestRoomList_Msg
    {
        using this_t = CSRequestRoomList_Action;
        using base_t = CSRequestRoomList_Msg;
    public:
        proto_ptr_t clone() override
        {
            return std::make_shared<this_t>();
        }
        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;
    };

    REGISTER_PROTOCOL(CSRequestRoomList_Action);

    void CSRequestRoomList_Action::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
    {
        //auto generated
    }
}
