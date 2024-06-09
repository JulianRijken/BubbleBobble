#pragma once

#include <Scene.h>
#include <SDL_pixels.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Game.h"

namespace bb
{

    class Maita;
    class Player;
    class ZenChan;

    // More like a factory :)
    namespace prefabs
    {
        using namespace jul;

        void SpawnCaptureBubble(const glm::vec3& spawnPosition, glm::vec2 velocity);

        ZenChan* SpawnZenChanWithBehaviour(const glm::vec3& spawnPosition);
        ZenChan* SpawnZenChan(const glm::vec3& spawnPosition);
        void SpawnZenChanDead(const glm::vec3& spawnPosition);

        Maita* SpawnMaitaWithBehaviour(const glm::vec3& spawnPosition);
        Maita* SpawnMaita(const glm::vec3& spawnPosition);
        void SpawnMaitaDead(const glm::vec3& spawnPosition);

        void SpawnPickup(PickupType fruitType, const glm::vec3& spawnPosition);

        Player* SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation = {});

        void SpawnLevelHUD(Scene& scene);
        void SpawnPlayerHUD(Scene& scene, int playerIndex);
        void SpawnMainCamera(Scene& scene);
        void SpawnSideWalls(Scene& scene);
        void SpawnScoreText(const glm::vec3& spawnPosition, int score, const SDL_Color& color = { 255, 255, 255, 255 });
    }  // namespace prefabs
}  // namespace bb
