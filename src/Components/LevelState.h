#pragma once
#include <Component.h>

#include <unordered_set>

#include "Enemy.h"

namespace bb
{
    using namespace jul;

    class LevelState final : public Component
    {
    public:
        inline static constexpr double SPAWN_DURATION{ 2.0 };

        struct EnemySpawn
        {
            EnemyType type = EnemyType::ZenChan;
            double delay = 0;
            glm::vec3 location = {};
        };

        LevelState(GameObject* parentPtr, const std::vector<EnemySpawn>& enemySpawns);

    private:
        std::unordered_set<Enemy*> m_Enemies{};
    };

}  // namespace bb
