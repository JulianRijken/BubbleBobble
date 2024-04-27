#include "Bounce.h"

#include "GameSettings.h"
#include "GameTime.h"
#include "Transform.h"

using namespace glm;

Bounce::Bounce(jul::GameObject* parent, float offset, float restitution) :
	Component(parent,"Bounce"),
	m_Offset(offset),
	m_Restitution(restitution)
{}


void Bounce::FixedUpdate()
{
	vec3 pos = GetTransform().WorldPosition();
	pos += m_Velocity * jul::GameTime::GetFixedDeltaTimeF();


    const float ground = jul::GameSettings::s_WindowHeight - m_Offset;


    if(pos.y > ground)
    {
		// Add overshot
		pos.y -= pos.y - ground;

		m_Velocity.y = -m_Velocity.y * m_Restitution;
	}

	m_Velocity.y += jul::GameTime::GetFixedDeltaTimeF() * GRAVITY;

	GetTransform().SetLocalPosition(pos);
}
