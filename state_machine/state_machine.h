#ifndef FB_FSM_TEMPLATE_H
#define FB_FSM_TEMPLATE_H

#include "fb_gptool.h"
#include "dbms_log.h"

namespace DBDist {

/*
* - goal: 
*   for the fsm programming, it is absolutely generalizaiton, no matter the
*   function fsm or the class fsm. so the fsm programming is more simple.
*
* - priciple:
*   on the embedded software, there are two logic pattern, the based on
*   logic and the based on the data. for the logic , the flow is usually
*   serial based on the time. for the data, the flow is usually parallel
*   based on the data. theoretically, all the serial flow can be
*   implemented by the fsm, and the advantage of the fsm is absolutely
*   verificated and concise. this is explained on the leslie lamport
*   "computation and state machines".
*/
template <typename Next, typename Trans>
class EventDispatcher {
public:
    static uint32_t Dispatch(typename Trans::DerivedFsm &fsm, 
                        uint32_t state, 
                        const typename Trans::DerivedEvent &event) 
    {
        if (state == Trans::m_current) {
            (void)Trans::Execute(fsm, event);
            return Trans::m_next;
        } else {
            return Next::Dispatch(fsm, state, event);
        }
    }

    using Result = EventDispatcher;

private:
    EventDispatcher(const EventDispatcher &) = delete;
    EventDispatcher &operator=(const EventDispatcher &) = delete;
};

class DefaultDispatcher {
public:
    template <typename Fsm, typename Event>
    static uint32_t Dispatch(Fsm &fsm, uint32_t state, const Event &event)
    {
        return fsm.UndefinedEvent(state, event);
    }

private:
    DefaultDispatcher(const DefaultDispatcher &) = delete;
    DefaultDispatcher &operator=(const DefaultDispatcher &) = delete;
};

template <typename Event, typename Trans>
struct EventMatcher {
    using Result = typename IsEqual<Event, typename Trans::DerivedEvent>::Result;
};

template <typename Table, typename Event>
struct DispatcherGenerator {
public:
    template <typename Trans> using TransitionMatcher = typename EventMatcher<Event, Trans>::Result;
    using MatchedTransitions = typename Filter<Table, TransitionMatcher>::Result;

    using Result = typename Accumulate<MatchedTransitions, DefaultDispatcher, EventDispatcher>::Result;
};

// the fsm interface
template <typename Derived>
class StateMachine {
public:
    template <typename Event> uint32_t ProcessEvent(const Event &event);
    template <typename Event> uint32_t UndefinedEvent(uint32_t state, const Event &event);

protected:
    StateMachine() : m_currentState(Derived::M_INITIAL_STATE) {}    
    virtual ~StateMachine() {}

    template <uint32_t currentState, typename Event, uint32_t nextState, uint32_t (Derived::*action)(const Event &)>
    struct Transition {
        using DerivedFsm = Derived;
        using DerivedEvent = Event;

        enum {
            m_current = currentState,
            m_next = nextState
        };

        static uint32_t Execute(Derived &fsm, const Event &event)
        {
            return (fsm.*action)(event);
        }
    };

protected:
    uint32_t m_currentState;

private:
    StateMachine(const StateMachine &) = delete;
    StateMachine &operator=(const StateMachine &) = delete;
};

template <typename Derived>
template <typename Event>
uint32_t StateMachine<Derived>::ProcessEvent(const Event &event)
{
    using Dispatcher = typename DispatcherGenerator<typename Derived::TransitionTable, Event>::Result;
    this->m_currentState = Dispatcher::Dispatch(*static_cast<Derived *>(this), this->m_currentState, event);
    return this->m_currentState; 
}

template <typename Derived>
template <typename Event>
uint32_t StateMachine<Derived>::UndefinedEvent(uint32_t state, const Event &event)
{
    CDbmsLog::DbmsCycleLog("err:no transit on state(%u) event(%s)", state, typeid(event).name());
    return m_currentState;
}

// using : only including the three steps:
// - step 1 : define the fsm event by the struct.
// - step 2 : define the fsm-state include the init state by enum FSMStates.
// - step 3 : define the fsm table by the FSMTable.
#define FSMTable(...) typename DBDist::ListType < __VA_ARGS__ > ::Result

}
#endif