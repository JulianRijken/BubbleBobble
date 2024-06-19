#pragma once
#include <Component.h>
#include <ICollisionListener.h>

namespace bb
{
    class OneWayPlatform final : public Component, public ICollisionListener
    {
    public:
        OneWayPlatform(GameObject* parentPtr);

        static constexpr float DELTA_EPSILON = -0.05f;

    private:
        void OnCollisionPreSolve(const Collision& collision, const b2Manifold* oldManifold) override;
    };

}  // namespace bb
