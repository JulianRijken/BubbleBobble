#pragma once

#include "GameObject.h"

namespace jul
{
    class Component;
}

namespace bb
{
    class IDamageable
    {
    public:
        virtual ~IDamageable() = default;

        IDamageable(IDamageable&&) = delete;
        IDamageable(const IDamageable&) = delete;
        IDamageable& operator=(IDamageable&&) = delete;
        IDamageable& operator=(const IDamageable&) = delete;

        virtual void OnDamage(jul::Component* instigator) = 0;

    protected:
        IDamageable() = default;
    };
}  // namespace bb
