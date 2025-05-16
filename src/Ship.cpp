#include "Classes.h"

ObjectType Ship::GetObjectType()
{
    return objType;
}

void Ship::Draw()
{
   Rectangle source = { 0.0f, 0.0f, (float)m_shipTexture.width, (float)m_shipTexture.height };
    Rectangle dest = { Position.x, Position.y, 30.0f, 30.0f }; // Taille affichée
    Vector2 origin = { 15.0f, 15.0f }; // Centre pour rotation
    float rotation = 0.0f; 
    DrawTexturePro(m_shipTexture, source, dest, origin, rotation, WHITE);

}
void Ship::RotateShip(float angle)
{
    RotationAngle += angle;

    // Optionnel : maintenir l'angle entre 0 et 360 degrés
    if (RotationAngle >= 360.0f) RotationAngle -= 360.0f;
    if (RotationAngle < 0.0f)    RotationAngle += 360.0f;
}



void Ship::Update(float dt) {

    VerifyImmuneTimer();

    // Les Bordures de la map
    if (Position.x - radius > WIDTH) {
        Position.x = 0 - radius;
    }
    else if (Position.x + radius < 0) {
        Position.x = WIDTH + radius;
    }

    if (Position.y - radius > HEIGHT) {
        Position.y = 0 - radius;
    }
    else if (Position.y + radius < 0) {
        Position.y = HEIGHT + radius;
    }


    if (Thrusting) {
 
        Velocity = LerpVelocityChange(GetDirectionUnit(), 0.1f); // Empeche le vaisseau de faire des rotation ou changement brusques
    
        Speed += Acceleration * dt;
        if (Speed > MaxSpeed) Speed = MaxSpeed;
    } else {
        Speed *= 0.99;
    }
    Position.x += Velocity.x * Speed * dt;
    Position.y += Velocity.y * Speed * dt;

    if (IsKeyPressed(KEY_H)) {
        Hyperspace();
    } // Peser sur H pour hyperspace
    
    if (IsKeyDown(KEY_A)) {
        Rotate(-270 * dt);
    }
    if (IsKeyDown(KEY_D)) {
        Rotate(270 * dt);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        if (!IsImmune) {
            Shoot();
        }
    }
    if (IsKeyDown(KEY_S)) {
        Speed -= (Acceleration * dt / 2);
        if (Speed < 0) Speed = 0;
    }
    if (IsKeyDown(KEY_W)) {
        Thrusting = true;
    } else {
        Thrusting = false;
    }
}

Vector2 Ship::LerpVelocityChange(Vector2 DesiredVelocityDirection, float t) {
    return {
        Velocity.x + (DesiredVelocityDirection.x - Velocity.x) * t,
        Velocity.y + (DesiredVelocityDirection.y - Velocity.y) * t
    };
}

Vector2 Ship::GetPoint(int Index) {
    return ShipTrianglePoints[Index];
}

void Ship::MakeImmune(float _ImmuneTimer) {
    ImmunityTimer = _ImmuneTimer;
    IsImmune = true;
}

bool Ship::ShipIsImmune() {
    return IsImmune;
}

void Ship::VerifyImmuneTimer() {
    if (IsImmune && ImmunityTimer <= 0) {
        IsImmune = false;
        ImmunityTimer = 0;
    } else if (IsImmune) {
        ImmunityTimer -= GetFrameTime();
    }
}

Vector2 Ship::GetDirectionUnit() {
    return Vector2 {
        cosf((270.0f + Rotation) * DEG2RAD),
        sinf((270.0f + Rotation) * DEG2RAD)
    };
}

void Ship::Shoot() {
    extern Game* game;

    Vector2 ProjectilePosition = {
        ShipTrianglePoints[0].x,
        ShipTrianglePoints[0].y
    };
    Vector2 ProjectileVelocity = {
        GetDirectionUnit().x,
        GetDirectionUnit().y
    };
    Projectile* newProjectile = new Projectile(ProjectilePosition, ProjectileVelocity, true, 3, 250);
    game->AddGameObject(newProjectile);
}

void Ship::Hyperspace() {
    extern Game* game;
    
    if (IsImmune) return;
    //Je veut pas qu'il puisse le faire si il est immune
    //Ca marche comme un teleport. D'un sens on peut dire qu'il disparait

    Position.x = GetRandomValue(30, WIDTH - 30);
    Position.y = GetRandomValue(30, HEIGHT - 30);
    
    Speed = 0;
    Velocity = {0, 0};
    
    // ca va draw une valuer de 1 a 100 donc 5% de chance de perdre une vie
    if (GetRandomValue(1, 100) <= 5) {
        MakeImmune(3.0f);
        game->LoseLife();
    }
}