#pragma once

#include <Component.h>
#include <ICollisionListener.h>

namespace bb
{
    class Boulder final : public jul::Component, public jul::ICollisionListener
    {
    public:
        Boulder(jul::GameObject* parentPtr);

    private:
        void OnCollisionBegin(const jul::Collision& /*unused*/) override;
    };
}  // namespace bb
