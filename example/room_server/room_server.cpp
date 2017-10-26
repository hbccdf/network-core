#include <network/gameserver/game_server.hpp>

//namespace CytxGame
//{
//    using namespace cytx;
//    using namespace cytx::gameserver;
//    class room_server
//    {
//    public:
//        public init()
//    private:
//        game_server server_;
//    };
//}


using namespace cytx;
using namespace cytx::gameserver;

int main(int argc, char* argv[])
{
    game_server server(server_unique_id::room_server);
    server.init();
    server.start();
}