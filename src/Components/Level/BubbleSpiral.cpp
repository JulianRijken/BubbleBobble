#include "BubbleSpiral.h"

#include <Animator.h>
#include <AutoMove.h>
#include <GameObject.h>
#include <GameTime.h>
#include <MathExtensions.h>
#include <Resources.h>
#include <Scene.h>
#include <SpriteRenderer.h>

#include <numbers>

using jul::Animator;
using jul::GameObject;
using jul::GameTime;
using jul::Resources;
using jul::Scene;
using jul::SpriteRenderer;

bb::BubbleSpiral::BubbleSpiral(GameObject* parentPtr) :
    Component(parentPtr, "BubbleSpiral")
{
}

void bb::BubbleSpiral::Update()
{
    m_SpawnIntervalTimer += GameTime::GetDeltaTime();
    if(m_SpawnIntervalTimer >= SPAWN_INTERVAL)
    {
        m_SpawnIntervalTimer -= SPAWN_INTERVAL;
        SpawnBubble();
    }
}

void bb::BubbleSpiral::SpawnBubble()
{
    for(int i{}; i < SIDES; i++)
    {
        const double offset = (std::numbers::pi * 2) / SIDES * i;

        const double time = GameTime::GetElapsedTime() * ROTATE_SPEED;
        glm::vec2 velocity{ std::cos(time + offset), std::sin(time + offset) };
        velocity *= BUBBLE_SPEED;

        Scene& thisScene = GetGameObject()->GetScene();

        auto* bubble = thisScene.AddGameObject("Bubble");
        auto* sprite = bubble->AddComponent<SpriteRenderer>(Resources::GetSprite("AttackBubble"), 0);
        bubble->AddComponent<Animator>(sprite, "Spawn");
        auto* autoMove = bubble->AddComponent<AutoMove>(glm::vec3(velocity.x, velocity.y, 0));
        autoMove->SetUseBoundingBox(false);
        bubble->DestroyDelayed(1.3);
    }
}
