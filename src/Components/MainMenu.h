#pragma once

#include <Component.h>

namespace bb
{
    using namespace jul;

    class MainMenu : public Component
    {
    public:
        MainMenu(GameObject* parentPtr, Transform* logoTransform);

    private:
        void OnLogoLand();
        Transform* m_LogoTransform;
    };
}  // namespace bb
