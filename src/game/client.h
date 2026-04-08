#pragma once

#include "game/protocol.h"

#include <libwebsockets.h>

#include <string>
#include <functional>
#include <vector>
#include <deque>

class Client {
public:
    // using OnMessageCallback = std::function<void(const std::string&)>;
    // using OnMessageCallback = std::function<void(const std::vector<uint8_t>&)>;

    Client();
    ~Client();

    bool connect(const std::string& uri);   // wss://host[:port]/path
    void poll();                            // Call this every frame
    // void send(const std::vector<uint8_t>& message);  // Queue message for sending
    // void setOnMessage(OnMessageCallback cb);

    bool isConnected();
    void close();
    void enableAutoReconnect(bool enable);
    void wait();
    void flush();
    // void setMaxQueueSize(size_t maxSize);

    template<typename T>
    void setHandler(std::function<void(const T&)> handler) {
        handlers[T::type] = [handler](const std::vector<uint8_t>& buffer) {
            T decoded = Protocol::decode<T>(buffer.data());
            handler(decoded);
        };
    }

    template<typename T>
    void send(const T& message) {
        outgoing.push_back(Protocol::encode(message));
    }
        
    
private:
    static int callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);
    std::vector<uint8_t> buildChunk(const std::vector<uint8_t>& data);

    lws_context* context = nullptr;
    lws* wsi = nullptr;
    // std::vector<std::string> outgoing;
    // std::vector<std::vector<uint8_t>> outgoing;
    std::deque<std::vector<uint8_t>> outgoing;

    // OnMessageCallback onMessage;
    bool connected = false;
    bool autoReconnect = true;
    // size_t maxQueueSize = 100;
    std::string reconnectUri;
    std::vector<uint8_t> groupedBuffer;
    static constexpr uint8_t GROUP_TYPE = 0;        
        
    static constexpr int MAX_PAYLOAD = 4096;

    void dispatchMessage(const std::vector<uint8_t>& data);

    std::unordered_map<uint8_t, std::function<void(const std::vector<uint8_t>&)>> handlers;

};


