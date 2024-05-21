#pragma once

#include <AutoMove.h>
#include <Component.h>

#include <vector>


namespace bb
{

    using namespace jul;

    class MainMenu : public Component
    {
    public:
        MainMenu(GameObject* parentPtr, Transform* logoTransformPtr, GameObject* intoTextPtr);

    private:
        void OnLogoLand();

        Transform* m_LogoTransformPtr;
        GameObject* m_IntoTextPtr;
        std::vector<AutoMove*> m_Bubbles;
    };
}  // namespace bb
