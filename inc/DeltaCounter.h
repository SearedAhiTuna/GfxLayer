
#pragma once

#include <chrono>

class DeltaCounter
{
public:
    void Start();

    // Time in microseconds
    float Stop();

private:
    bool _started{};
    std::chrono::system_clock::time_point _prev;
};