#pragma once

#include <memory>
#include <stack>
#include "game_states/state.h"

typedef std::unique_ptr<State> StateRef;

class StateMachine
{
public:
	StateMachine();

	void addState(StateRef new_state, bool is_replacing = true);
	void removeState();
	void handleStateChange();
	void removeAllStates();

	StateRef& getCurrentState();

private:
	std::stack<StateRef> states;
	StateRef new_state;

	bool is_removing;
	bool is_adding;
	bool is_replacing;

};