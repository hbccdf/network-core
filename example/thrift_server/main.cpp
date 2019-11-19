#include "RpcHelloService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class RpcHelloServiceHandler : virtual public RpcHelloServiceIf {
public:
    RpcHelloServiceHandler() {
        // Your initialization goes here
    }

    int32_t show(const std::string& name) {
        // Your implementation goes here
        printf("show\n");
        return 0;
    }

};

int main(int argc, char **argv) {
    int port = 9090;
    shared_ptr<RpcHelloServiceHandler> handler(new RpcHelloServiceHandler());
    shared_ptr<TProcessor> processor(new RpcHelloServiceProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
}