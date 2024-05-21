#include "AutoRotateAround.h"

#include "GameTime.h"
#include "Transform.h"

AutoRotateAround::AutoRotateAround(jul::GameObject* parent, float distanceFromTarget, float speed) :
    Component(parent, "AutoRotateAround"),
    m_Speed(speed),
    m_DistanceFromTarget(distanceFromTarget)
{
}

void AutoRotateAround::LateUpdate()
{
    glm::vec3 targetPosition = glm::vec3{ std::cos(jul::GameTime::GetElapsedTime() * m_Speed) * m_DistanceFromTarget,
                                          std::sin(jul::GameTime::GetElapsedTime() * m_Speed) * m_DistanceFromTarget,
                                          0 };

    GetTransform().SetLocalPosition(targetPosition);
} 
