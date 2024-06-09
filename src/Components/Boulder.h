#pragma once

#include <Component.h>
#include <ICollisionListener.h>

namespace bb
{
    using namespace jul;

    class Boulder : public Component, public ICollisionListener
    {
    public:
        Boulder(GameObject* parentPtr);

    private:
        void OnCollisionBegin(const Collision&) override;
    };
}  // namespace bb
