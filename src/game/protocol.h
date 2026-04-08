#pragma once

#include <vector>
#include <cstdint>
#include <cstring>

/*
    
Declareation:
======================================
Define protocol structures with this pragma
With: static constexpr uint8_t type = [YOUR_TYPE_ID]

#pragma pack(push, 1)
struct Position {
    static constexpr uint8_t type = 1;
    uint32_t object_id;
    float x, y;
};
#pragma pack(pop)

Usage:    
=======================================

// Create object
Position position {
    .object_id = 1337,
    .x = 12.34f,
    .y = 56.78f
};

// Encode on sender
auto encoded = Protocol::encode(position);

// Decode on receiver

switch (Protocol::type(encoded.data())) {
    case Position::type:
        Position position = Protocol::decode<Position>(encoded.data())
        break;
    default:
        break;
}

*/

/**
 * @brief Utility class for encoding and decoding fixed-size binary protocols.
 *
 * Protocol format:
 * - [1 byte]   Type (defined as static `type` inside the struct)
 * - [2 bytes]  Payload size (little-endian)
 * - [N bytes]  Raw payload (struct data)
 */
class Protocol {
public:
    /**
     * @brief Encodes a struct into a binary protocol with type and size headers.
     *
     * @tparam T The struct type. Must contain a static constexpr uint8_t `type` field.
     * @param data The struct instance to encode.
     * @return std::vector<uint8_t> The binary protocol as a byte array.
     */
    template<typename T>
    static std::vector<uint8_t> encode(const T& data) {
        constexpr uint16_t size = sizeof(T);
        std::vector<uint8_t> buffer(1 + 2 + size); // type + size + payload

        buffer[0] = T::type;
        buffer[1] = size & 0xFF;
        buffer[2] = (size >> 8) & 0xFF;
        std::memcpy(buffer.data() + 3, &data, size);

        return buffer;
    }

    /**
     * @brief Extracts the type field from a raw protocol buffer.
     *
     * @param buffer Pointer to the raw data buffer.
     * @return uint8_t The type value (or 0 if the buffer is too small).
     */
    static uint8_t type(const uint8_t* buffer) {
        return buffer[0];
    }

    /**
     * @brief Decodes a binary protocol payload into a struct of type T.
     *
     * @tparam T The struct type to decode into.
     * @param buffer Pointer to the raw protocol buffer.
     * @return T The decoded struct (default-constructed if the buffer is invalid).
     */
    template<typename T>
    static T decode(const uint8_t* buffer) {
        T obj{};
        std::memcpy(&obj, buffer + 3, sizeof(T));
        return obj;
    }

    /**
     * @brief Extracts the payload size from the protocol header.
     *
     * @param buffer Pointer to the raw protocol buffer.
     * @return uint16_t Payload size in bytes.
     */
    static uint16_t size(const uint8_t* buffer) {
        return static_cast<uint16_t>(buffer[1]) | (static_cast<uint16_t>(buffer[2]) << 8);
    }
};


