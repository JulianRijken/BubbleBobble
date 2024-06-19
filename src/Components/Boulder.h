#pragma once

#include <Component.h>
#include <ICollisionListener.h>

namespace bb
{
    class Boulder final : public Component, public ICollisionListener
    {
    public:
        Boulder(GameObject* parentPtr);

    private:
        void OnCollisionBegin(const Collision&) override;
    };
}  // namespace bb
