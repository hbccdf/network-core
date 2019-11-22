#include "thrift_common.hpp"

namespace cytx
{
    class thrift_server : public net::irouter<connection_t>
    {
    public:
        thrift_server(processer_ptr processer)
            : processer_(processer)
        {}

        void init()
        {
            net::server_options options;
            options.batch_send_msg = true;
            options.disconnect_interval = 0;
            options.thread_mode = net::server_thread_mode::no_io_thread;
            server_ = std::make_unique<server_t>("127.0.0.1", 6327, options, this);
        }

        void start()
        {
            server_->start();
        }

        void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            if (err)
                return;

            auto transport = boost::make_shared<thrift_transport>(conn_ptr);
            protocol_t* proto_ptr = new protocol_t(transport);
            conn_ptr->world()["proto"] = proto_ptr;
            conn_ptr->world()["trans"] = transport.get();
        }

        void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            //log error
            protocol_t* proto_ptr = conn_ptr->world()["proto"];
            if (proto_ptr != nullptr)
            {
                delete proto_ptr;
                proto_ptr = nullptr;
                conn_ptr->world()["proto"] = proto_ptr;
            }
        }

        void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            protocol_t* proto_ptr = conn_ptr->world()["proto"];
            thrift_transport* trans = conn_ptr->world()["trans"];
            if (!proto_ptr || !trans)
            {
                //log
                return;
            }

            trans->set_current_msg(msgp);

            protocol_ptr proto(proto_ptr, [](auto ptr) {});

            processer_->process(proto, proto, nullptr);
        }

    private:
        server_ptr server_;
        processer_ptr processer_;
    };
}


class RpcHelloServiceHandler : virtual public RpcHelloServiceIf {
public:
    RpcHelloServiceHandler() {
        // Your initialization goes here
    }

    int32_t show(const std::string& name) {
        // Your implementation goes here
        std::cout << "show " << name << std::endl;
        return 9;
    }

};

int main(int argc, char **argv) {
    shared_ptr<RpcHelloServiceHandler> handler(new RpcHelloServiceHandler());
    shared_ptr<TProcessor> processor(new RpcHelloServiceProcessor(handler));

    cytx::log::init_log(cytx::log_level_t::debug, "net");
    cytx::log::init_log(cytx::log_level_t::debug, "conn");

    cytx::thrift_server server(processor);
    server.init();
    server.start();

    return 0;
}