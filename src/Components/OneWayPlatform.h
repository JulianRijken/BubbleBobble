#pragma once
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Component.h>
#include <ICollisionListener.h>
#include <Rigidbody.h>


using namespace jul;

namespace bb
{
    class OneWayPlatform : public jul::Component, public ICollisionListener
    {
    public:
        OneWayPlatform(GameObject* parentPtr);

        static constexpr float DOT_EPSILON = 0.05f;
        static constexpr float DELTA_EPSILON = -0.5f;

    private:
        void OnCollisionPreSolve(b2Contact* contact, b2Fixture* otherFixture) override;
    };

}  // namespace bb