#pragma once
#include "../engine.h"
#include "../headers.h"
#include "paddle.h"
#include "../boss_animation_states/boss_animation.h"
#include "../boss_animation_states/boss_idle.h"
#include "../boss_animation_states/boss_death.h"
#include "../boss_animation_states/boss_shooting.h"
#include "../boss_animation_states/boss_opening.h"
#include "../boss_animation_states/boss_closing.h"

class Boss : public sf::Sprite
{
public:

	Boss(GameDataRef data, sf::Vector2f frame_position);
	Direction whichSide(const sf::Sprite& obiekt, const sf::Vector2f& velocity);
	
	void update(const float& dt, std::unique_ptr<Paddle>& player);
	void handleHit();
	void draw();

	int hp;
	bool is_drawing;
	bool is_outro;
	sf::RectangleShape hitbox;
	sf::Sound death_sound;


	BossAnimation* state;

	BossOpening opening;
	BossShooting shooting;
	BossClosing closing;
	BossIdle idle;
	BossDeath death;

	sf::Sprite projectile;
	std::vector<std::tuple<std::unique_ptr<sf::Sprite>, std::unique_ptr<sf::Vector2f>, unsigned int>> projectiles{};
	std::vector<sf::IntRect> projectile_frames;
	

private:
	void animation(const float& dt, std::unique_ptr<Paddle>& player);
	void hitAnimation(const float& dt);
	void deathAnimation(const float& dt);
	void projectileAnimation(const float& dt);

	GameDataRef data;
	Direction Right, Left, Down, Up, NoDirection;

	float hit_time;
	float death_time;
	float projectile_time;
	int top;
	int multiplier;
	

	sf::RectangleShape background;
};

