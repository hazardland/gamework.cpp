#pragma once

#pragma pack(push, 1)
struct FootmanState {
    static constexpr uint8_t type = 1;
    uint32_t object_id;
    float x;
    float y;
    uint32_t action;
    uint32_t faceX;
    uint32_t faceY;
};
#pragma pack(pop)