#pragma once

namespace cytx
{
    class imodule
    {
    public:
        virtual ~imodule() {}
        virtual bool init() = 0;
        virtual bool start() = 0;
        virtual bool reload() = 0;
        virtual void stop() = 0;
    };
}