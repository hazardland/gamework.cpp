#include "game/client.h"
#include <cstring>
#include <iostream>
#include <thread>


Client::Client() {
    lws_set_log_level(LLL_ERR, nullptr);  // Only show errors
    // lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO, NULL);

}

Client::~Client() {
    if (context) {
        lws_context_destroy(context);
    }
}

bool Client::connect(const std::string& uri) {

    std::string address = "localhost";
    int port = 443;
    std::string path = "/";
    bool ssl = false;


    std::string uri_ = uri;
    if (uri_.rfind("wss://", 0) == 0) {
        ssl = true;
        uri_ = uri_.substr(6);
    } else if (uri_.rfind("ws://", 0) == 0) {
        uri_ = uri_.substr(5);
    }

    // Extract path
    auto slashPos = uri_.find('/');
    if (slashPos != std::string::npos) {
        path = uri_.substr(slashPos);
        uri_ = uri_.substr(0, slashPos);
    }

    // Extract host and port
    auto colonPos = uri_.find(':');
    if (colonPos != std::string::npos) {
        address = uri_.substr(0, colonPos);
        std::string portStr = uri_.substr(colonPos + 1);
        try {
            port = std::stoi(portStr);
        } catch (...) {
            std::cout << "[Client] Invalid port: " << portStr << std::endl;
            return false;
        }
    } else {
        address = uri_;
        port = ssl ? 443 : 80;
    }

    printf("[Client] uri: %s --> ssl: %s, path: %s, address: %s, port: %d\n",
            uri.c_str(),
            ssl ? "true" : "false",
            path.c_str(),
            address.c_str(),
            port);


    lws_protocols protocols[] = {
        { "ws", Client::callback, 0, MAX_PAYLOAD },
        { nullptr, nullptr, 0, 0 }
    };


    lws_context_creation_info ctxInfo = {};
    ctxInfo.port = CONTEXT_PORT_NO_LISTEN;
    ctxInfo.protocols = protocols;
    ctxInfo.gid = -1;
    ctxInfo.uid = -1;
    ctxInfo.ka_time = 15;      // After 15s of inactivity, start keepalive
    ctxInfo.ka_interval = 5;   // Send pings every 5s
    ctxInfo.ka_probes = 3;     // Allow 3 failed pings before dropping

    context = lws_create_context(&ctxInfo);
    if (!context) {
        std::cout << "[Client] Failed to create lws context\n";
        return false;
    }

    lws_client_connect_info connectInfo = {};
    connectInfo.context = context;
    connectInfo.address = address.c_str();
    connectInfo.port = port;
    connectInfo.path = path.c_str();
    connectInfo.host = address.c_str();
    connectInfo.origin = "origin";
    connectInfo.ssl_connection = ssl ? LCCSCF_USE_SSL : 0;
    connectInfo.protocol = "ws";
    connectInfo.userdata = this;

    reconnectUri = uri;

    wsi = lws_client_connect_via_info(&connectInfo);
    if (!wsi) {
        std::cout << "[Client] Connection failed\n";
        return false;
    }

    wait();

    if (!connected) {
        std::cout << "[Client] Connection failed\n";
        return false;
    }

    return true;
}

void Client::poll() {
    if (context) {
        lws_cancel_service(context);
        lws_service(context, 0);         // non-blocking
    }
}

void Client::wait() {
    int retries = 0;
    while (!connected && retries++ < 500) {
        poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


// Inside Client class
void Client::dispatchMessage(const std::vector<uint8_t>& data) {
    if (data.size() < 3) return;

    uint8_t type = Protocol::type(data.data());
    auto it = handlers.find(type);
    if (it != handlers.end()) {
        it->second(data);
    }
}


void Client::flush() {
    if (!wsi || !connected || outgoing.empty())
        return;

    std::vector<std::vector<uint8_t>> groupedChunks;
    std::vector<uint8_t> currentChunk;

    for (const auto& msg : outgoing) {
        size_t totalSizeIfAdded = currentChunk.size() + msg.size();
        if (totalSizeIfAdded > MAX_PAYLOAD - LWS_PRE) {  // 3 = group header
            // finalize current chunk
            if (!currentChunk.empty()) {
                groupedChunks.push_back(buildChunk(currentChunk));
                currentChunk.clear();
            }
        }
        currentChunk.insert(currentChunk.end(), msg.begin(), msg.end());
    }

    // Don't forget the last chunk
    if (!currentChunk.empty()) {
        groupedChunks.push_back(buildChunk(currentChunk));
    }

    // Replace outgoing with grouped chunks
    outgoing.clear();
    for (auto& chunk : groupedChunks) {
        outgoing.push_back(std::move(chunk));
    }

    // Now start actual sending
    lws_callback_on_writable(wsi);
    while (!outgoing.empty()) {
        poll();
    }
}

std::vector<uint8_t> Client::buildChunk(const std::vector<uint8_t>& data) {
    uint16_t size = static_cast<uint16_t>(data.size());
    std::vector<uint8_t> chunk(3 + data.size());

    chunk[0] = GROUP_TYPE;
    chunk[1] = size & 0xFF;
    chunk[2] = (size >> 8) & 0xFF;

    std::memcpy(chunk.data() + 3, data.data(), data.size());

    return chunk;
}


bool Client::isConnected()
{
    return connected;
}

int Client::callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {

    Client* self = reinterpret_cast<Client*>(lws_wsi_user(wsi));

    switch (reason) {

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            std::cout << "[Client] Connected\n";
            self->connected = true;

            if (!self->outgoing.empty()) {
                lws_callback_on_writable(wsi);
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
        if (self) {
            const uint8_t* buffer = static_cast<const uint8_t*>(in);
            size_t totalLen = len;

            if (Protocol::type(buffer) == GROUP_TYPE) {
                size_t offset = 3;
                while (offset + 3 <= totalLen) {
                    const uint8_t* msgPtr = buffer + offset;
                    uint16_t msgSize = Protocol::size(msgPtr);

                    if (offset + 3 + msgSize > totalLen)
                        break;

                    std::vector<uint8_t> msg(msgPtr, msgPtr + 3 + msgSize);
                    self->dispatchMessage(msg);

                    offset += 3 + msgSize;
                }
            } else {
                std::vector<uint8_t> msg(buffer, buffer + totalLen);
                self->dispatchMessage(msg);
            }
        }
        break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if (self && !self->outgoing.empty()) {
                const auto& msg = self->outgoing.front();

                unsigned char buf[LWS_PRE + MAX_PAYLOAD];

                if (msg.size() > MAX_PAYLOAD + LWS_PRE) {
                    std::cout << "[Client] Binary payload too large!\n";
                    return -1;
                }

                std::memcpy(&buf[LWS_PRE], msg.data(), msg.size());


                int flags = LWS_WRITE_BINARY;// | LWS_WRITE_NO_FIN;
                int bytes_written = lws_write(wsi, &buf[LWS_PRE], msg.size(), (lws_write_protocol)flags);

                if (bytes_written < 0 || bytes_written < msg.size()) {
                    std::cout << "[Client] lws_write failed!\n";
                    return -1;
                }

                self->outgoing.pop_front();


                if (!self->outgoing.empty()) {
                    lws_callback_on_writable(wsi);
                }
            }
            break;


        case LWS_CALLBACK_CLIENT_CLOSED:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            std::cout << "[Client] Connection error\n";
            if (in && len > 0)
                std::cout << "Error: " << std::string((char*)in, len) << "\n";

            self->connected = false;

            // if (self->context) {
            //     lws_context_destroy(self->context);
            //     self->context = nullptr;
            // }
            self->wsi = nullptr;

            if (self->autoReconnect && !self->reconnectUri.empty()) {
                std::cout << "[Client] Attempting reconnect...\n";
                self->connect(self->reconnectUri);
            }
            break;


    default:
        break;
    }

    return 0;
}

void Client::close() {
    if (wsi) {
        lws_set_timeout(wsi, PENDING_TIMEOUT_CLOSE_SEND, LWS_TO_KILL_ASYNC);
        wsi = nullptr;
    }

    if (context) {
        lws_context_destroy(context);
        context = nullptr;
    }

    connected = false;
    std::cout << "[Client] Connection closed\n";
}

void Client::enableAutoReconnect(bool enable) {
    autoReconnect = enable;
}

// void Client::setMaxQueueSize(size_t maxSize) {
//     maxQueueSize = maxSize;
// }
