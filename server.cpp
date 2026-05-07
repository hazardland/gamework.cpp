// File: beast_server.cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <thread>

using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

std::set<std::shared_ptr<websocket::stream<tcp::socket>>> clients;
std::mutex clients_mutex;

class session : public std::enable_shared_from_this<session> {
public:
    explicit session(tcp::socket socket)
        : ws_(std::move(socket)) {}

    void start() {
        ws_.set_option(websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
        ws_.accept([self = shared_from_this()](boost::beast::error_code ec) {
            if (!ec) self->on_accept();
        });
    }

private:
    websocket::stream<tcp::socket> ws_;
    boost::beast::flat_buffer buffer_;

    void on_accept() {
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.insert(ws_.shared_from_this());
        }
        do_read();
    }

    void do_read() {
        ws_.async_read(buffer_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            if (ec == websocket::error::closed) {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(self->ws_.shared_from_this());
                return;
            }

            if (!ec && self->ws_.got_binary()) {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (const auto& client : clients) {
                    if (client != self->ws_.shared_from_this()) {
                        boost::beast::error_code ignored;
                        client->write(boost::asio::buffer(self->buffer_.data()), ignored);
                    }
                }
            }

            self->buffer_.consume(self->buffer_.size());
            self->do_read();
        });
    }
};

void do_accept(tcp::acceptor& acceptor, boost::asio::io_context& ioc) {
    acceptor.async_accept([&acceptor, &ioc](boost::beast::error_code ec, tcp::socket socket) {
        if (!ec) std::make_shared<session>(std::move(socket))->start();
        do_accept(acceptor, ioc);
    });
}

int main() {
    try {
        boost::asio::io_context ioc{1};
        tcp::acceptor acceptor{ioc, tcp::endpoint(tcp::v4(), 9000)};

        std::cout << "✅ Boost.Beast WebSocket server running on ws://localhost:9000\n";
        do_accept(acceptor, ioc);
        ioc.run();
    } catch (std::exception& e) {
        std::cerr << "[!] Error: " << e.what() << "\n";
    }

    return 0;
}
