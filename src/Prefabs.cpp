#include "Prefabs.h"

#include <Animator.h>
#include <AttackBubble.h>
#include <BoxCollider.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <ZenChan.h>


void bb::prefabs::Spawnbubble(const glm::vec3& spawnPosition, float firePower, int direction)
{
    auto* bubble = SceneManager::GetInstance().AddGameObject("AttackBubble", spawnPosition);
    bubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), 0);
    bubble->AddComponent<Animator>();
    bubble->AddComponent<Rigidbody>();
    bubble->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.5f,
        .restitution = 0.8f,
        .size = {1.5, 1.5f},
    });
    bubble->AddComponent<AttackBubble>(glm::vec3{ direction * firePower, 0, 0 });
}

void bb::prefabs::SpawnZenChan(const glm::vec3& spawnPosition)
{
    auto* zenchanGO = SceneManager::GetInstance().AddGameObject("ZenChan", spawnPosition);
    zenchanGO->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Enemys"), 0);
    zenchanGO->AddComponent<Animator>();
    zenchanGO->AddComponent<Rigidbody>();
    zenchanGO->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.90f, 1.90f},
    });
    zenchanGO->AddComponent<ZenChan>();
}
