#pragma once

#include <Component.h>

namespace bb
{
    using namespace jul;

    class Health final : public Component
    {
    public:
        Health(GameObject* parentPtr, int startingLives, int playerIndex);

        [[nodiscard]] Event<>& GetOnFinalDeath() { return m_OnDeath; }

        int GetLivesLeft() const { return m_LivesLeft; }

        bool IsDead() const { return m_Dead; }

        void Damage();

    private:
        Event<> m_OnDeath{};

        int m_LivesLeft{};
        int m_PlayerIndex{};
        bool m_Dead{ false };
    };
}  // namespace bb
