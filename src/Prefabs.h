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

        void SpawnCaptureBubble(const glm::vec3& spawnPosition, glm::vec2 velocity);
        void SpawnZenChan(const glm::vec3& spawnPosition);
        Player* SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation = {});
        void SpawnPlayerHUD(Scene& scene, int playerIndex);
        void SpawnMainCamera(Scene& scene);
    }  // namespace prefabs
}  // namespace bb
