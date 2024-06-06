#pragma once

namespace bb
{
    class ZenChan;

    class ZenChanState
    {
    public:
        virtual ~ZenChanState() = default;

        ZenChanState(ZenChanState&&) = delete;
        ZenChanState(const ZenChanState&) = delete;
        ZenChanState& operator=(ZenChanState&&) = delete;
        ZenChanState& operator=(const ZenChanState&) = delete;

        virtual void OnEnterState(ZenChan& /*unused*/){};

        virtual void Update(ZenChan& /*unused*/){};
        virtual void FixedUpdate(ZenChan& /*unused*/){};

        virtual void OnAttackInput(ZenChan& /*unused*/){};
        virtual void OnJumpInput(ZenChan& /*unused*/){};


        virtual void OnExitState(ZenChan& /*unused*/){};


    protected:
        ZenChanState() = default;
    };
}  // namespace bb
