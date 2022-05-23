#pragma once

enum class Direction { Right, Left, Down, Up, NoDirection };

#define WIDTH 224.f
#define HEIGHT 256.f

#define FPS_LIMIT 120

//Sciezki

//Pliki
#define DATA_FILE "data/scoreboard_data.json"

//Tekstury
#define BACKGROUND_TEXTURE_FILE "assets/textures/Backgrounds.png"
#define BRICK_TEXTURE_FILE "assets/textures/Bricks.png"
#define PADDLE_TEXTURE_FILE "assets/textures/Palettes.png"
#define BONUS_TEXTURE_FILE "assets/textures/Powerups.png"
#define BOSS_TEXTURE_FILE "assets/textures/Boss.png"
#define PROJECTILE_TEXTURE_FILE "assets/textures/Projectiles.png"
#define COPYRIGHT_TEXTURE_FILE "assets/textures/Copyright.png"
#define SKY_TEXTURE_FILE "assets/textures/Sky.png"
#define ARKANOID_SHIP_TEXTURE_FILE "assets/textures/Arkanoid_Ship.png"
#define SHIP_TEXTURE_FILE "assets/textures/Ship.png"

//Czcionki
#define FONT_FILE "assets/fonts/Arkanoid_Font.ttf"
#define TAITO_FONT_FILE "assets/fonts/Taito_Font.ttf"

//Dzwieki
#define ENLARGE_SOUND_FILE "assets/sounds/Enlarge.wav"
#define BOSS_BOUNCE_SOUND_FILE "assets/sounds/Boss_Bounce.wav"
#define PADDLE_BOUNCE_SOUND_FILE "assets/sounds/Paddle_Bounce.wav"
#define BRICK_BOUNCE_SOUND_FILE "assets/sounds/Brick_Bounce.wav"
#define GOLD_BRICK_BOUND_SOUND_FILE "assets/sounds/Gold_Brick_Bounce.wav"
#define PADDLE_DEATH_SOUND_FILE "assets/sounds/Paddle_Death.wav"
#define BOSS_DEATH_SOUND_FILE "assets/sounds/Boss_Death.wav"

#define INTRO_SOUND_FILE "assets/sounds/Intro.ogg"
#define START_SOUND_FILE "assets/sounds/Game_Start.ogg"
#define HP_SOUND_FILE "assets/sounds/HP.ogg"
#define BOSS_START_SOUND_FILE "assets/sounds/Boss_Start.ogg"
#define GAME_OVER_SOUND_FILE "assets/sounds/Game_Over.ogg"
#define OUTRO_SOUND_FILE "assets/sounds/Outro.ogg"
#define LASER_SOUND_FILE "assets/sounds/Laser.ogg"