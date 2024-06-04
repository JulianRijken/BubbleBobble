#pragma once

#include <glm/vec3.hpp>

namespace bb::prefabs
{
    void Spawnbubble(const glm::vec3& spawnPosition, float firePower, int direction);
    void SpawnZenChan(const glm::vec3& spawnPosition);
}  // namespace bb::prefabs
