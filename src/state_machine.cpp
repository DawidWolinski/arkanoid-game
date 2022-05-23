#include "state_machine.h"

StateMachine::StateMachine()
:is_adding(false), is_removing(false), is_replacing(false)
{}

void StateMachine::addState(StateRef new_state, bool is_replacing)
{
	is_adding = true;
	this->is_replacing = is_replacing;

	this->new_state = std::move(new_state);
}

void StateMachine::removeState()
{
	is_removing = true;
}

void StateMachine::handleStateChange()
{
	if (is_removing && !states.empty())
	{
		states.pop();

		if (!states.empty())
		{
			states.top()->resume();
		}
	}

	is_removing = false;

	if (is_adding)
	{
		if (!states.empty())
		{
			if (is_replacing)
			{
				states.pop();
			}
			else
			{
				states.top()->pause();
			}
		}

		states.push(std::move(new_state));
		states.top()->initialise();
		is_adding = false;
	}
}

StateRef& StateMachine::getCurrentState()
{
	return states.top();
}

void StateMachine::removeAllStates()
{
	while (!states.empty())
		states.pop();
}