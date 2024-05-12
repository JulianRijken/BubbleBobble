#pragma once

class IDamagable
{
public:
    virtual ~IDamagable() = default;

    IDamagable(IDamagable&&) = delete;
    IDamagable(const IDamagable&) = delete;
    IDamagable& operator=(IDamagable&&) = delete;
    IDamagable& operator=(const IDamagable&) = delete;

    virtual void OnDamage() = 0;

protected:
    IDamagable() = default;
};
