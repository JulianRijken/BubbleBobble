#pragma once

namespace jul
{
    class Component;
}

class IDamagable
{
public:
    virtual ~IDamagable() = default;

    IDamagable(IDamagable&&) = delete;
    IDamagable(const IDamagable&) = delete;
    IDamagable& operator=(IDamagable&&) = delete;
    IDamagable& operator=(const IDamagable&) = delete;

    virtual void OnDamage(jul::Component* instigator) = 0;

protected:
    IDamagable() = default;
};
