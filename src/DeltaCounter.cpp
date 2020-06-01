
#include "DeltaCounter.h"

void DeltaCounter::Start()
{
    using namespace std::chrono;

    _started = true;
    _prev = system_clock::now();
}

float DeltaCounter::Stop()
{
    using namespace std::chrono;

    if (!_started)
        return 0;

    _started = false;

    auto cur = system_clock::now();
    auto length = duration_cast<microseconds>(cur - _prev);

    return (float)length.count();
}
