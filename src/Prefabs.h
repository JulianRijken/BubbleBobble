#pragma once

#include <Scene.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace bb
{

    class Player;

    // More like a factory :)
    namespace prefabs
    {
        using namespace jul;

        void SpawnBubble(const glm::vec3& spawnPosition, float firePower, int direction);
        void SpawnBubble(const glm::vec3& spawnPosition, float firePower, glm::vec2 direction);
        void SpawnZenChan(const glm::vec3& spawnPosition);
        Player* SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation = {});
        void SpawnPlayerHUD(Scene& scene, int playerIndex);
        void SpawnMainCamera(Scene& scene);
    }  // namespace prefabs
}  // namespace bb
