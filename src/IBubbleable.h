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

        virtual void SpawnDeadVersion() = 0;

        [[nodiscard]] virtual std::string GetSpriteName() = 0;

        [[nodiscard]] virtual std::string GetSpriteAnimationName() = 0;

        [[nodiscard]] virtual jul::Transform* GetCaptureTransform() = 0;

        // (Is Captured) :P
        bool m_IsBubbleabled{ false };  // NOLINT - C.131: Avoid trivial getters and setters

    protected:
        IBubbleable() = default;
    };

}  // namespace bb
