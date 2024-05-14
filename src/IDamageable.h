#pragma once

namespace jul
{
    class Component;
}

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
