#ifndef HEALTH_H
#define HEALTH_H

#include <Component.h>

namespace bb
{
    class Health final : public jul::Component
    {
    public:
        Health(jul::GameObject* parentPtr, int startingLives, int playerIndex);

        [[nodiscard]] jul::Event<>& GetOnFinalDeath() { return m_OnDeath; }

        int GetLivesLeft() const { return m_LivesLeft; }

        bool IsDead() const { return m_Dead; }

        void Damage();

    private:
        jul::Event<> m_OnDeath{};

        int m_LivesLeft{};
        int m_PlayerIndex{};
        bool m_Dead{ false };
    };
}  // namespace bb
#endif  // HEALTH_H
