#pragma once
#include "Component.h"
#include "TextRenderer.h"

namespace bb
{
    class FpsCounter final : public jul::Component
    {
    public:
        FpsCounter(jul::GameObject* parent);

    private:
        void Awake() override;
        void Update() override;

        jul::TextRenderer* m_TextRenderer{};
    };

}  // namespace bb
