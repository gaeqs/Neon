//
// Created by gaeqs on 8/21/24.
//

#include "Coroutine.h"

namespace neon
{

    WaitForSeconds::WaitForSeconds(float seconds)
    {
        auto now = std::chrono::system_clock::now();

        _wakeUpTime = now + std::chrono::microseconds(static_cast<uint64_t>(seconds * 1000000.0f));
    }

    inline bool WaitForSeconds::isReady()
    {
        return std::chrono::system_clock::now() >= _wakeUpTime;
    }

    WaitForNextFrame::WaitForNextFrame() = default;

    bool WaitForNextFrame::isReady()
    {
        return true;
    }

    WaitUntil::WaitUntil(std::function<bool()> predicate) :
        _predicate(std::move(predicate))
    {
    }

    bool WaitUntil::isReady()
    {
        return _predicate();
    }

    WaitWhile::WaitWhile(std::function<bool()> predicate) :
        _predicate(std::move(predicate))
    {
    }

    bool WaitWhile::isReady()
    {
        return !_predicate();
    }
} // namespace neon
