#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	dir = true;

	idleR.totalFrames = 0;
	idleL.totalFrames = 0;
	left.totalFrames = 0;
	right.totalFrames = 0;
	jumpR.totalFrames = 0;
	jumpL.totalFrames = 0;
	jumpR2.totalFrames = 0;
	jumpL2.totalFrames = 0;
	death.totalFrames = 0;
	win.totalFrames = 0;

	//initilize textures
	texture = app->tex->Load(texturePath);
	//idleR Anim
	for (int i = 0; i < 3; i++) {
		idleR.PushBack({ 0 + (i * 48), 336, 48, 48 });
	}
	idleR.loop = true;
	idleR.speed = 0.1f;

	//idleL Anim
	for (int i = 3; i >= 0; i--) {
		idleL.PushBack({ 576 + (i * 48), 336, 48, 48 });
	}
	idleL.loop = true;
	idleL.speed = 0.1f;

	//right Anim
	for (int i = 0; i < 5; i++) {
		right.PushBack({ 0 + (i * 48), 480, 48, 48 });
	}
	right.loop = true;
	right.speed = 0.2f;

	//left Anim
	for (int i = 5; i >= 0; i--) {
		left.PushBack({ 480 + (i * 48), 480, 48, 48 });
	}
	left.loop = true;
	left.speed = 0.2f;

	//jumpR Anim
	for (int i = 0; i < 5; i++) {
		jumpR.PushBack({ 0 + (i * 48), 240, 48, 48 });
	}
	jumpR.loop = false;
	jumpR.speed = 0.15f;

	//jumpL Anim
	for (int i = 5; i >= 0; i--) {
		jumpL.PushBack({ 480 + (i * 48), 240, 48, 48 });
	}
	jumpL.loop = false;
	jumpL.speed = 0.15f;

	//jumpR2 Anim
	for (int i = 0; i < 3; i++) {
		jumpR2.PushBack({ 0 + (i * 48), 384, 48, 48 });
	}
	jumpR2.loop = false;
	jumpR2.speed = 0.15f;
	
	//jumpL2 Anim
	for (int i = 3; i >= 0; i--) {
		jumpL2.PushBack({ 576 + (i * 48), 384, 48, 48 });
	}
	jumpL2.loop = false;
	jumpL2.speed = 0.15f; 
	
	//death Anim
	for (int i = 0; i < 5; i++) {
		death.PushBack({ 0 + (i * 48), 192, 48, 48 });
	}
	death.loop = false;
	death.speed = 0.2f;

	//win Anim
	for (int i = 0; i < 5; i++) {
		win.PushBack({ 0 + (i * 48), 576, 48, 48 });
	}
	win.loop = true;
	win.speed = 0.15f;

	currentAnimation = &idleR;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y+16, 16, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.wav");
	jumpFx = app->audio->LoadFx("Assets/Audio/Fx/jump.wav");

	return true;
}

bool Player::Update()
{
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	currentAnimation->Update();

	app->render->DrawTexture(texture, position.x, position.y - 16, &rect);

	// L07 DONE 5: Add physics to the player - updated player position using physics

	int speed = 10; 
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y); 

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		//
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}
		
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !dead && !winner) {
		vel = b2Vec2(-speed, -GRAVITY_Y);
		currentAnimation = &left;
		dir = false;
	}
	else {
		left.Reset();
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !dead && !winner) {
		vel = b2Vec2(speed, -GRAVITY_Y);
		currentAnimation = &right;
		dir = true;
	}
	else {
		right.Reset();
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !dead && !winner) {
		app->audio->PlayFx(jumpFx);
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && !dead && !winner) {
		if (jumpTimer > 0)
		{
			if (dir) {
				currentAnimation = &jumpR;
			}
			else {
				currentAnimation = &jumpL;
			}
			onair = true;
			vel = b2Vec2(0, GRAVITY_Y);
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !dead && !winner) {
				vel = b2Vec2(speed*0.9, GRAVITY_Y);
				dir = true;
				currentAnimation = &jumpR;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !dead && !winner) {
				vel = b2Vec2(-speed*0.9, GRAVITY_Y);
				dir = false;
				currentAnimation = &jumpL;
			}
			jumpTimer--;
		}
	}
	else {
		jumpL.Reset();
		jumpR.Reset();
	}

	if (dead) {
		currentAnimation = &death;
		dir = true;
	}
	else {
		death.Reset();
	}

	if (winner) {
		currentAnimation = &win;
		dir = true;
	}
	else {
		win.Reset();
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_IDLE
		&& app->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_IDLE
		&& app->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_IDLE && dir && !dead && !winner)
		currentAnimation = &idleR;

	if (app->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_IDLE
		&& app->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_IDLE
		&& app->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_IDLE && !dir && !dead && !winner)
		currentAnimation = &idleL;

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	SDL_Rect textSection = { 0,0,48,48 };

	//app->render->DrawTexture(texture, position.x , position.y, &textSection);
	//app->render->DrawTexture(texture, position.x , position.y);

	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision
	switch (physB->ctype)
	{
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			app->audio->PlayFx(pickCoinFxId);
			break;
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			jumpTimer = 30;
			break;
		case ColliderType::DEATH:
			LOG("Collision DEATH");
			dead = true;
			break;
		case ColliderType::WIN:
			LOG("Collision WIN");
			winner = true;
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
	}
}
