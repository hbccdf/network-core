#include <network/rpc/thrift_server.hpp>
#include "RpcHelloService.h"

using cytx::log_level_t;

class RpcHelloServiceHandler : public RpcHelloServiceIf
{
public:
    RpcHelloServiceHandler()
    {}

    int32_t show(const std::string& name)
    {
        std::cout << "show " << name << std::endl;
        return 9;
    }
};

int main(int argc, char **argv) {
    boost::shared_ptr<RpcHelloServiceHandler> handler(new RpcHelloServiceHandler());
    boost::shared_ptr<apache::thrift::TProcessor> processor(new RpcHelloServiceProcessor(handler));
    boost::shared_ptr<apache::thrift::TMultiplexedProcessor> p(new apache::thrift::TMultiplexedProcessor());
    p->registerProcessor("test", processor);

    cytx::log::init_log(log_level_t::debug, "net");
    cytx::log::init_log(log_level_t::debug, "conn");
    cytx::log::init_log(log_level_t::debug, "thrift");

    cytx::thrift::thrift_server server(p, 6327);
    server.start();

    return 0;
}