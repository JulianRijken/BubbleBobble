#include "LevelHUD.h"

#include <fmt/format.h>
#include <ResourceManager.h>
#include <SpriteRenderer.h>

#include "Game.h"
#include "GameObject.h"

bb::LevelHUD::LevelHUD(GameObject* parentPt) :
    Component(parentPt, "LevelHUD")

{
    auto& scene = GetGameObject()->GetScene();

    double spawnHeight = Game::GRID_SIZE_Y / 2 - 2;

    auto* levelTextGoPtr = scene.AddGameObject("LevelText", { 0, spawnHeight, 0 }, GetGameObject());
    m_LevelNumberText =
        levelTextGoPtr->AddComponent<TextRenderer>("error", ResourceManager::GetFont("NES"), -5, glm::vec2{ 0.5, 0 });

    auto* blackBox1 = scene.AddGameObject("LevelText", { 0, spawnHeight, 0 }, GetGameObject());
    blackBox1->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -10, glm::ivec2{ 4, 20 });
    auto* blackBox2 = scene.AddGameObject("LevelText", { -1, spawnHeight, 0 }, GetGameObject());
    blackBox2->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -10, glm::ivec2{ 4, 20 });


    UpdateText(1);
    Game::GetInstance().GetLevelTransitionChangeEvent().AddListener(this, &LevelHUD::OnLevelTransitionChange);
}

void bb::LevelHUD::OnLevelTransitionChange(bool /*unused*/, int levelIndex)
{
    GetGameObject()->SetActive(levelIndex > 0);
    UpdateText(levelIndex);
}

void bb::LevelHUD::UpdateText(int levelIndex) { m_LevelNumberText->SetText(fmt::format("{:02}", levelIndex)); }
