#include <network/rpc/thrift_client.hpp>
#include "RpcHelloService.h"

using cytx::log_level_t;

int main(int argc, char* argv[])
{
    cytx::log::init_log(log_level_t::debug, "net");
    cytx::log::init_log(log_level_t::debug, "conn");

    auto transport_ptr = boost::make_shared<cytx::thrift::thrift_client>("127.0.0.1", 6327);

    RpcHelloServiceClient client(transport_ptr->get_multi_proto("test"));

    int result = client.show("test");

    std::cout << result << std::endl;

    return 0;
}