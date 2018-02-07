#include <network/gameserver/server_manager.hpp>

namespace CytxGame
{
    using namespace cytx;
    using namespace cytx::gameserver;

    using game_server_t = cytx::gameserver::server_manager;
    class center_server_t : public game_server_t
    {
        using base_t = game_server_t;
    public:
        center_server_t()
            : game_server_t(server_unique_id::center_server)
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
    CytxGame::center_server_t server;
    server.init();
    server.start();
}