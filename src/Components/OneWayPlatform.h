#pragma once
#include <Component.h>
#include <ICollisionListener.h>

namespace bb
{
    class OneWayPlatform final : public jul::Component, public jul::ICollisionListener
    {
    public:
        OneWayPlatform(jul::GameObject* parentPtr);

        static constexpr float DELTA_EPSILON = -0.05f;

    private:
        void OnCollisionPreSolve(const jul::Collision& collision, const b2Manifold* oldManifold) override;
    };

}  // namespace bb
