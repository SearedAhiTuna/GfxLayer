
#include "FrameCounter.h"

#include <thread>

FrameCounter::FrameCounter(const float& fps, const size_t& avgDepth) :
    _frameLength((int)(1000000.f / fps)),
    _started(false),
    _fps(avgDepth)
{

}

void FrameCounter::wait()
{
    using namespace std::chrono;

    if (_started)
    {
        auto cur = system_clock::now();
        auto length = duration_cast<microseconds>(cur - _prev);
        if (length > _frameLength)
            _fps.enter(1000000.f / length.count());
        else
            _fps.enter(1000000.f / _frameLength.count());

        auto target = _prev + _frameLength;
        std::this_thread::sleep_until(target);
    }
    else
    {
        _started = true;
    }

    _prev = system_clock::now();
}

float FrameCounter::fps()
{
    return _fps.get();
}