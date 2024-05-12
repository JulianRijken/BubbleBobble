#include "FpsCounter.h"

#include <fmt/core.h>

#include "GameObject.h"
#include "TextRenderer.h"
#include "GameTime.h"

bb::FpsCounter::FpsCounter(jul::GameObject* parent) :
    Component(parent, "FpsCounter"),
    m_TextRenderer(parent->GetComponent<jul::TextRenderer>())
{
}

void bb::FpsCounter::Update()
{
    if (m_TextRenderer == nullptr)
        return;

    m_TextRenderer->SetText(fmt::format("{:.1f}", jul::GameTime::GetSmoothFps()));
}
