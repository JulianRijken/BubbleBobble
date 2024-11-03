#include "LevelState.h"

#include <CharacterInput.h>
#include <TweenEngine.h>

#include "Maita.h"
#include "MathExtensions.h"
#include "Prefabs.h"
#include "ZenChan.h"

using jul::EaseFunction;
using jul::GameObject;
using jul::TweenEngine;

bb::LevelState::LevelState(GameObject* parentPtr, const std::vector<EnemySpawn>& enemySpawns) :
    Component(parentPtr, "LevelState")
{
    // VS mode :)
    if(Game::GetInstance().GetActiveGameMode() == GameMode::VS)
    {
        if(jul::math::RandomValue() > 0.5f)
        {
            auto* zenChan = prefabs::SpawnZenChan({ 0, Game::GRID_SIZE_Y * 0.5, 0 });
            zenChan->GetGameObject()->AddComponent<CharacterInput>(false, 1);
            m_Enemies.insert(zenChan);
        }
        else
        {
            auto* maita = prefabs::SpawnMaita({ 0, Game::GRID_SIZE_Y * 0.5, 0 });
            maita->GetGameObject()->AddComponent<CharacterInput>(false, 1);
            m_Enemies.insert(maita);
        }
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
            else if(enemySpawn.type == EnemyType::Maita)
                enemy = prefabs::SpawnMaitaWithBehaviour(fromLocation);


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

    for(auto&& enemy : m_Enemies)
        enemy->GetOnDestroyedEvent().AddListener(this, &LevelState::OnEnemyDestroyed);
}

void bb::LevelState::OnLevelWin()
{
    TweenEngine::Start({ .delay = DELAY_BEFORE_LOADING_NEXT_LEVEL,
                         .duration = 0,
                         .invokeWhenDestroyed = false,
                         .onEnd = []() { Game::GetInstance().TryTransitionNextLevel(); } },
                       GetGameObject());
}

void bb::LevelState::OnEnemyDestroyed(Object* object)
{
    auto* enemy = dynamic_cast<Enemy*>(object);
    assert(enemy != nullptr);

    m_Enemies.erase(enemy);

    fmt::println("Enemys Left: {}", m_Enemies.size());
    if(m_Enemies.empty())
        OnLevelWin();
}
