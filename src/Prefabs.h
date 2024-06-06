#pragma once

#include <Scene.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Game.h"

namespace bb
{

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
        Player* SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation = {});
        void SpawnPlayerHUD(Scene& scene, int playerIndex);
        void SpawnMainCamera(Scene& scene);
        void SpawnPickup(PickupType fruitType, const glm::vec3& spawnPosition);
        void SpawnSideWalls(Scene& scene);
    }  // namespace prefabs
}  // namespace bb
