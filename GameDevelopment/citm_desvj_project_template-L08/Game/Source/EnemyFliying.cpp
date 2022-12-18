#include "EnemyFlying.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Pathfinding.h"
#include "Map.h"

EnemyFlying::EnemyFlying() : Entity(EntityType::ENEMYFLYING)
{
	name.Create("EnemyFlying");
}

EnemyFlying::~EnemyFlying() {

}

bool EnemyFlying::Awake() {

	// Initialize Enemy parameters
	// Get Enemy parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool EnemyFlying::Start() {

	//idle Anim
	for (int i = 0; i < 4; i++) {
		idle.PushBack({ 0 + (i * 48), 144, 48, 48 });
	}
	idle.loop = true;
	idle.speed = 0.1f;

	//move Anim
	for (int i = 0; i < 4; i++) {
		move.PushBack({ 0 + (i * 48), 192, 48, 48 });
	}
	move.loop = true;
	move.speed = 0.1f;

	//death Anim
	for (int i = 0; i < 2; i++) {
		death.PushBack({ 0 + (i * 48), 96, 48, 48 });
	}
	for (int i = 0; i < 4; i++) {
		death.PushBack({ 0 + (i * 48), 48, 48, 48 });
	}
	death.loop = false;
	death.speed = 0.075f;

	currentAnimation = &idle;

	// Add physics to the enemy - initialize physics body
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 12, bodyType::DYNAMIC);

	// Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	//deathFx = app->audio->LoadFx("Assets/Audio/Fx/biker_hurt.wav");

	return true;
}

bool EnemyFlying::Update()
{
	//posInicialY = position.y;

	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	currentAnimation->Update();

	// Render the texture
	/*if (!destroyed) {
		app->render->DrawTexture(texture, position.x, position.y - 16, &rect);
	}*/
	app->render->DrawTexture(texture, position.x, position.y - 16, &rect);

	// Add physics to the enemy - updated enemy position using physics
	//int speed = 10;
	//b2Vec2 vel = b2Vec2(0, 0);

	// idleAnim condition
	//currentAnimation = &idleL;

	// Set the velocity of the pbody of the player
	//pbody->body->SetLinearVelocity(vel);

	// Update enemy position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//SDL_Rect textSection = { 0,0,48,48 };

	return true;
}

bool EnemyFlying::CleanUp()
{
	return true;
}

// Define OnCollision function for the enemy. Check the virtual function on Entity class
void EnemyFlying::OnCollision(PhysBody* physA, PhysBody* physB) {

	// Detect the type of collision
	int enemyhead = app->scene->player->position.y + 10; //Variable que hace que el jugador deba estar encima un numero de pixeles minimo para matar al enemigo
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("ENEMY Collision PLAYER");

		LOG("ENEMY Y: %d", position.y);
		LOG("PLAYER Y: %d", enemyhead);
		if (!app->scene->player->GodMode && enemyhead >= position.y) {
			if (!destroyed) {
				app->scene->player->dead = true;
			}
			//app->audio->PlayFx(deathFx);
		}
		else {
			currentAnimation = &death;
			destroyed = true;
			b2Vec2(0, -GRAVITY_Y);
		}
		break;
	case ColliderType::PLATFORM:
		LOG("ENEMY Collision PLATFORM");
		break;
	}
}

void EnemyFlying::moveLeft() {
	LOG("LEFT");
	currentAnimation = &move;
	pbody->body->SetLinearVelocity(b2Vec2(-1, 0));
}

void EnemyFlying::moveRight() {
	LOG("RIGHT");
	currentAnimation = &move;
	pbody->body->SetLinearVelocity(b2Vec2(1, 0));
}

void EnemyFlying::moveUp() {
	LOG("UP");
	currentAnimation = &move;
	pbody->body->SetLinearVelocity(b2Vec2(0, -1));
}

void EnemyFlying::moveDown() {
	LOG("DOWN");
	currentAnimation = &move;
	pbody->body->SetLinearVelocity(b2Vec2(0, 1));
}