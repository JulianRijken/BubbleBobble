#pragma once
#include "Component.h"
#include <glm/vec3.hpp>

namespace bb
{
    using namespace jul;

    class AutoMove final : public jul::Component
    {
    public:
        AutoMove(jul::GameObject* parent, const glm::vec3& velocity = {}, const glm::vec3& boundingBox = {},
                 const glm::vec3& boundingBoxCenter = {});

        void SetVelocity(glm::vec3 velocity);

        inline glm::vec3 GetVelocity() const { return m_Velocity; }

    private:
        void Update() override;

        glm::vec3 m_Velocity{};
        glm::vec3 m_BoundingBox{};
        glm::vec3 m_BoundingBoxCenter{};
    };

}  // namespace bb
