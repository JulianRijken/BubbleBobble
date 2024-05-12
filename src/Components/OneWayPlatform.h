#pragma once
#include <Component.h>
#include <ICollisionListener.h>


using namespace jul;

namespace bb
{
    class OneWayPlatform : public jul::Component, public ICollisionListener
    {
    public:
        OneWayPlatform(GameObject* parentPtr);

        static constexpr float DELTA_EPSILON = -0.05f;

    private:
        void OnCollisionPreSolve(Collision collision, const b2Manifold* oldManifold) override;
    };

}  // namespace bb
