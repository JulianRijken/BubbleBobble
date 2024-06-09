#include "LevelState.h"

#include <CharacterInput.h>
#include <TweenEngine.h>

#include "prefabs.h"
#include "ZenChan.h"

bb::LevelState::LevelState(jul::GameObject* parentPtr, const std::vector<EnemySpawn>& enemySpawns) :
    Component(parentPtr, "LevelState")
{
    // VS mode :)
    if(Game::GetInstance().GetActiveGameMode() == GameMode::VS)
    {
        auto* zenChan = prefabs::SpawnZenChan({ 0, Game::GRID_SIZE_Y * 0.5, 0 });
        zenChan->GetGameObject()->AddComponent<CharacterInput>(false, 1);
        m_Enemies.insert(zenChan);
    }
    else
    {
        for(auto&& enemySpawn : enemySpawns)
        {
            glm::vec3 fromLocation = enemySpawn.location;
            fromLocation.y = static_cast<float>(Game::GRID_SIZE_Y) * 0.5f;

            Enemy* enemy = nullptr;
            if(enemySpawn.type == EnemyType::ZenChan)
                enemy = prefabs::SpawnZenChanWithBehaviour(fromLocation);
            assert(enemy != nullptr && "Enemy Not Implemented");

            enemy->GetGameObject()->SetActive(false);

            m_Enemies.insert(enemy);

            enemy->OnLevelSpawn();
            TweenEngine::Start({ .delay = enemySpawn.delay,
                                 .from = fromLocation.y,
                                 .to = enemySpawn.location.y,
                                 .duration = SPAWN_DURATION,
                                 .invokeWhenDestroyed = false,
                                 .easeFunction = EaseFunction::Linear,
                                 .onStart = [enemy]() { enemy->GetGameObject()->SetActive(true); },
                                 .onUpdate =
                                     [enemy, fromLocation](double value)
                                 {
                                     // Move enemy down
                                     enemy->GetTransform().SetWorldPosition(fromLocation.x, value, fromLocation.z);
                                 },
                                 .onEnd = [enemy]() { enemy->OnLevelPlaced(); } },
                               enemy);
        }
    }


    ///
    ///         // enemy->GetOnDestroyedEvent().AddListener(this,[this](){
    //     m_Enemies.erase()


    // });
}
