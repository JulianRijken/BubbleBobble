#pragma once

#include <Component.h>

#include <glm/vec3.hpp>


class AutoRotateAround final : public jul::Component
{
public:
    AutoRotateAround(jul::GameObject* parent, float distanceFromTarget, float speed = 1.0f);

private:
    void LateUpdate() override;

    float m_Speed{};
	float m_DistanceFromTarget{};
};
