#include "Classes.h"

ObjectType Asteroid::GetObjectType()
{
    return objType;
}

void Asteroid::Draw()
{//javais essayer de le faire avec drawtexture pro mais pas reussi
    //  Rectangle source = { 0.0f, 0.0f, (float)m_asteroidTexture.width, (float)m_asteroidTexture.height };
    
    // // Taille affichée selon la taille de l'astéroïde
    // float scale = 1.0f;
    // if (AsteroidSize == 3) scale = 1.0f;
    // else if (AsteroidSize == 2) scale = 0.66f;
    // else if (AsteroidSize == 1) scale = 0.33f;

    // Rectangle dest = { Position.x, Position.y, m_asteroidTexture.width * scale, m_asteroidTexture.height * scale };

    // Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

    // // Calcule l'angle de direction
    // float rotation = atan2f(Direction.y, Direction.x) * RAD2DEG;

    // DrawTexturePro(m_asteroidTexture, source, dest, origin, rotation, WHITE);
    DrawCircle(Position.x, Position.y, AsteroidSize * AsteroidRadius, DARKGRAY);
}

void Asteroid::Update(float dt)
{
    // Move the asteroid
    Position.x += Direction.x * Speed * dt;
    Position.y += Direction.y * Speed * dt;

    if (Position.x - (AsteroidSize * AsteroidRadius) > WIDTH) {
        Position.x = 0 - (AsteroidSize * AsteroidRadius) + 1;
    }
    else if (Position.x + (AsteroidSize * AsteroidRadius) < 0) {
        Position.x = WIDTH + (AsteroidSize * AsteroidRadius) - 1;
    }

    if (Position.y - (AsteroidSize * AsteroidRadius) > HEIGHT) {
        Position.y = 0 - (AsteroidSize * AsteroidRadius) + 1;
    }
    else if (Position.y + (AsteroidSize * AsteroidRadius) < 0) {
        Position.y = HEIGHT + (AsteroidSize * AsteroidRadius) - 1;
    }
}

int Asteroid::GetAsteroidSize() {
    return AsteroidSize;
}

float Asteroid::GetSpeed() {
    return Speed;
}

float Asteroid::GetAsteroidRadius() {
    return AsteroidRadius;
}

void Asteroid::SplitAsteroid() {
    extern Game* game;
    if (AsteroidSize - 1 == 0) {game->RemoveGameObject(this); return;};
    int NumberOfChilds = GetRandomValue(2, 3);
    for (int i = 0; i < NumberOfChilds; i++) {
        int RandomDirection = GetRandomValue(0, 360);
        int RandomDistance = GetRandomValue(10, 20);

        Vector2 SpawnOffset = {
            (cosf(RandomDirection * DEG2RAD) * RandomDistance),
            (sinf(RandomDirection * DEG2RAD) * RandomDistance)
        };

        Vector2 NewPosition = {
            Position.x + SpawnOffset.x,
            Position.y + SpawnOffset.y
        };

        Asteroid* NewAsteroid = new Asteroid(
            NewPosition,
            AsteroidSize - 1,
            Speed * 1.2f
           
        );
        
        game->AddGameObject(NewAsteroid);
    }
        game->RemoveGameObject(this);
}