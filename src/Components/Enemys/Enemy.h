#ifndef Enemy_H
#define Enemy_H

#include "Character.h"

namespace bb
{
    class Enemy : public Character
    {
    public:
        explicit Enemy(jul::GameObject* parentPtr = nullptr, const std::string& name = "Enemy");
        ~Enemy() override = default;

        Enemy(Enemy&&) = delete;
        Enemy(const Enemy&) = delete;
        Enemy& operator=(Enemy&&) = delete;
        Enemy& operator=(const Enemy&) = delete;

        virtual void OnLevelSpawn(){};
        virtual void OnLevelPlaced(){};
    };
}  // namespace bb
#endif  // Enemy_H
