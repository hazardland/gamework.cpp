#include <thread>

#include <game/client.h>
#include <game/protocol.h>
#include <iostream>
#include <chrono>

// Message struct definition
#pragma pack(push, 1)
struct Position {
    static constexpr uint8_t type = 1;
    uint32_t client_id;
    uint32_t message_id;
};
#pragma pack(pop)

std::string getColor(uint32_t client_id) {
    switch (client_id % 6) {
        case 1: return "\033[34m"; // Blue
        case 2: return "\033[33m"; // Yellow
        case 3: return "\033[31m"; // Red
        case 4: return "\033[32m"; // Green
        case 5: return "\033[35m"; // Magenta
        case 6: return "\033[36m"; // Cyan
        default: return "\033[0m"; // Default
    }
}

const std::string COLOR_RESET = "\033[0m";

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: ./client <client_id>\n";
        return 1;
    }

    uint32_t client_id = std::stoi(argv[1]);

    Client client;

    client.setHandler<Position>([](const Position& pos) {
        std::string color = getColor(pos.client_id);
        if (pos.message_id % 1000 == 0) {
            std::cout << color
                      << "cl_id: " << pos.client_id
                      << ", msg_id: " << pos.message_id
                      << COLOR_RESET << "\n";
        }
    });
    
    client.enableAutoReconnect(true);

    if (!client.connect("ws://localhost:9000")) {
        return 1;
    }

    const uint32_t messageCount = 10000000;
    const uint32_t batchSize = 100;
    uint32_t sent = 0;

    client.wait();
    auto start = std::chrono::steady_clock::now();

    while (sent < messageCount) {

        for (int i = 0; i < 10; ++i) {
            client.poll();
        }    
       
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
       
        for (int i = 0; i < 1000; ++i) {
            client.send(Position{ .client_id = client_id, .message_id = ++sent });
        }

        client.flush();
    }
    

    auto end = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Sent " << sent << " messages in " << elapsedMs << " ms\n";
    std::cout << "Average: " << (elapsedMs / static_cast<float>(messageCount)) << " ms/message\n";

    return 0;
}

// g++ client.cpp src/game/client.cpp -I./src -lwebsockets -o client