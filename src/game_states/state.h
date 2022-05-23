#pragma once

class State
{
public:
	virtual ~State() {};

	virtual void initialise() = 0;

	virtual void handleEvents(const float& dt) = 0;
	virtual void update(const float& dt) = 0;
	virtual void draw(const float& dt) = 0;

	virtual void pause() {};
	virtual void resume() {}
};
