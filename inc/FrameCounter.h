
#pragma once

#include "RunningAverage.h"

#include <chrono>

class FrameCounter
{
public:
    FrameCounter(const float& fps, const size_t& avgDepth = 1);

    void wait();

    float fps();

private:
    const std::chrono::microseconds _frameLength;

    bool _started;
    std::chrono::system_clock::time_point _prev;

    RunningAverage<float> _fps;
};