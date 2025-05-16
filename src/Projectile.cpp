#include "Classes.h"

ObjectType Projectile::GetObjectType()
{
    return objType;
}

void Projectile::Draw() {
    DrawCircle(Position.x, Position.y, Radius, isPlayerProjectile ? BLUE : RED);
}

void Projectile::Update(float dt) {
    LifeTime += dt;
    
    extern Game* game;
    if (LifeTime >= MaxLifeTime) {
        game->RemoveGameObject(this);
        return;
    }
    
    Position.x += Velocity.x * Speed * dt;
    Position.y += Velocity.y * Speed * dt;
    
    if (Position.x > WIDTH) {
        Position.x = 0;
    } else if (Position.x < 0) {
        Position.x = WIDTH;
    }
    
    if (Position.y > HEIGHT) {
        Position.y = 0;
    } else if (Position.y < 0) {
        Position.y = HEIGHT;
    }
}

float Projectile::GetRadius() {
    return Radius;
}

bool Projectile::IsPlayerProjectile() {
    return isPlayerProjectile;
}