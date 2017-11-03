#pragma once
#include "../base/type_id.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            class game_server_base;
        }
    }

    using game_server_base_t = cytx::gameserver::detail::game_server_base;

    class iservice
    {
    public:
        virtual ~iservice() {}
        virtual void set_server(game_server_base_t*) = 0;
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void reset() = 0;
    public:
        virtual type_id_t get_type_id() const = 0;
    };
}