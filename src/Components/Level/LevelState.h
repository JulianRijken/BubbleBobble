#ifndef LEVELSTATE_H
#define LEVELSTATE_H
#include <Component.h>

#include <unordered_set>

#include "Enemy.h"

namespace bb
{
    class LevelState final : public jul::Component, public jul::IEventListener
    {
    public:
        inline static constexpr double SPAWN_DURATION{ 2.0 };
        inline static constexpr double DELAY_BEFORE_LOADING_NEXT_LEVEL{ 7.0 };

        struct EnemySpawn
        {
            EnemyType type = EnemyType::ZenChan;
            double delay = 0;
            glm::vec3 location = {};
        };

        LevelState(jul::GameObject* parentPtr, const std::vector<EnemySpawn>& enemySpawns);

    private:
        void OnLevelWin();

        void OnEnemyDestroyed(Object* object);
        std::unordered_set<Enemy*> m_Enemies{};
    };

}  // namespace bb
#endif // LEVELSTATE_H