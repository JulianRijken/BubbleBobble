#pragma once
#include <Transform.h>

namespace bb
{
    class IBubbleable
    {
    public:
        virtual ~IBubbleable() = default;

        IBubbleable(IBubbleable&&) = delete;
        IBubbleable(const IBubbleable&) = delete;
        IBubbleable& operator=(IBubbleable&&) = delete;
        IBubbleable& operator=(const IBubbleable&) = delete;

        virtual void OnCapture() {}

        virtual void OnRelease() {}

        virtual jul::Transform* GetCaptureTransform() = 0;

    protected:
        IBubbleable() = default;
    };
}  // namespace bb
