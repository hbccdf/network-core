#include "proto/all_actions.hpp"
#include "service/common_check.hpp"
#include "service/player_service.h"
#include "service/room_service.h"

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
        CHECK_PLAYER_SERVICE(player_svc);
        CHECK_ROOM_SERVICE(room_svc);

        auto& header = msgp->header();
        CSRequestRoomList& data = csRequestRoomList;

        SCRequestRoomList_Msg data_wrap;
        auto& ret = data_wrap.scRequestRoomList.result;
        auto player = player_svc->find_player(header.user_id);
        if (!check_player(ret, player) || !check_not_matched(ret, player) ||
            !check_search_room_mode(ret, data.pattern))
        {
            LOG_DEBUG("player {} request room list failed, {}, player:[{}]", header.user_id, error_code_str(ret), get_player_info(player, header.user_id));
            server.send_client_msg(header.user_id, data_wrap);
            return;
        }

        if (data.countPerPage <= 0 || data.pageNum <= 0)
        {
            server.send_client_msg(header.user_id, data_wrap);
            return;
        }

        std::vector<room_ptr_t> room_list = room_svc->get_room_list((room_mode)data.pattern, data.pattern == -1);
        data_wrap.scRequestRoomList.roomCount = (int)room_list.size();
        if (room_list.empty())
        {
            server.send_client_msg(header.user_id, data_wrap);
            return;
        }

        int total_page_num = ((int)room_list.size() / data.countPerPage) + ((int)room_list.size() % data.countPerPage > 0 ? 1 : 0);
        if (data.pageNum > total_page_num)
        {
            server.send_client_msg(header.user_id, data_wrap);
            return;
        }

        int begin_index = (data.pageNum - 1) * data.countPerPage;
        for (int i = 0; i < data.countPerPage; ++i)
        {
            int index = begin_index + i;
            if (index >= (int)room_list.size())
                break;

            room_ptr_t room = room_list[index];
            RoomData room_data;
            room_data.id = room->id();
            room_data.pattern = (int)room->mode_;
            room_data.playerCount = (int)room->players_.size();
            room_data.playerMaxCount = (int)room->capacity();
            data_wrap.scRequestRoomList.roomList.push_back(room_data);
        }

        server.send_client_msg(header.user_id, data_wrap);
    }
}
