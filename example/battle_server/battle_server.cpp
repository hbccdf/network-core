#include <network/gameserver/game_server.hpp>
#include "proto/message_types.h"

namespace CytxGame
{
    using namespace cytx;
    using namespace cytx::gameserver;

    using game_server_t = cytx::gameserver::game_server;
    class room_server_t : public game_server_t
    {
        using base_t = game_server_t;
    public:
        room_server_t()
            : game_server_t(server_unique_id::room_server)
        {}
    protected:
        void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            base_t::on_receive(conn_ptr, msgp);
        }
    };
}






int main(int argc, char* argv[])
{
    CytxGame::room_server_t server;
    server.init();
    server.start();
}