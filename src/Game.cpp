#include "Classes.h"


void Game::DrawMenu() {
    DrawText("CRAB GAB SPACESHIP", WIDTH/2 - MeasureText("CRAB GAB SPACESHIP", 40)/2, HEIGHT/3, 40, GREEN);
    
    
    
    int promptFontSize = 20;
    
    DrawText("PRESS ENTER TO START AND H FOR HYPER", WIDTH/2 - MeasureText("PRESS ENTER TO START AND H FOR HYPER", promptFontSize)/2, HEIGHT/2 + 40, promptFontSize, ColorAlpha(BLACK, 0.5f )); // la formule donne un chiffre entre 0.5 et 1
}

void Game::AddGameObject(GameObject* obj) {
    GameInstances.push_back(obj);
}

void Game::RemoveGameObject(GameObject* obj) {
    for (int i = 0; i < GameInstances.size(); i++) {
        if (GameInstances[i] == obj) {
            GameInstances.erase(GameInstances.begin() + i);
            
            delete obj;
            
            break;
        }
    }
}

void Game::SpawnAsteroids(int count) {
    for (int i = 0; i < count; i++) {
        Vector2 a_Position;
        a_Position.x = GetRandomValue(50, WIDTH - 50);
        a_Position.y = GetRandomValue(50, HEIGHT - 50);
        while (abs(playerShip->GetPosition().x - a_Position.x) < 100 || abs(playerShip->GetPosition().y - a_Position.y) < 100) {
            a_Position.x = GetRandomValue(50, WIDTH - 50);
            a_Position.y = GetRandomValue(50, HEIGHT - 50);
        }

        int size = GetRandomValue(1, 3);
        float speed = 50.0f + (3 - size) * 25.0f; // +petit = +vite
        
        Asteroid* asteroid = new Asteroid(a_Position, size, speed);
        AddGameObject(asteroid);
    }
}

void Game::Initialize()
{
    if (playerShip == nullptr) {
        Vector2 screenCenter = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        playerShip = new Ship(screenCenter, 0.0f, 200.0f, 150.0f);
    } else {
        Vector2 screenCenter = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        playerShip->SetPosition(screenCenter);
    }

    GameInstances.clear();

    SpawnAsteroids(level + 2);

    currentState = PLAYING;

}

void Game::HandlePlayingInput() {
    if (playerShip != nullptr) {
        playerShip->Update(GetFrameTime());
    }
}

void Game::HandleGameOverInput() {
    if (IsKeyPressed(KEY_ENTER)) {
        score = 0;
        lives = 3;
        
        for (int i = 0; i < GameInstances.size(); i++) {
            GameObject* obj = GameInstances[i];
            delete obj;
        }
        GameInstances.clear();
        
        Initialize();
    }
}

void Game::HandleMenuInput() {
    if (IsKeyPressed(KEY_ENTER)) {
        Initialize();
        currentState = PLAYING;
    }
}

void Game::Update() {
    if (currentState == MENU) {
        HandleMenuInput();
    } 
    
    else if (currentState == PLAYING) {
        if (CheckIfLevelIsFinished()) {
            if (score >= 40000 && score <= 60000) level++; // Le level c'est un peu un concept abstrait. Je me sert seulement de lui pour spawn les asteroids.
            //Si j'ai bien compris, le jeu commence seulement a gagner en difficulté à partir de 40000 et 60000 donc tant que le joueur est en bas de 40000, seulement 2 asteroids vont spawn.
            StartNewLevel();
        };
        CheckGainLife();
        HandlePlayingInput();
        for (int i = 0; i < GameInstances.size(); i++) {
            GameInstances[i]->Update(GetFrameTime());
        }
        ShipCollisions();
        ProjectileCollisions();
        if (score >= 99990) {score = 0; GainLifeCheckpoint = 10000; level = 0;} //quand le player atteint le score max ca reset
    } 
   
    else if (currentState == GAME_OVER) {
        HandleGameOverInput();
    }
}

void Game::Draw() {

    DrawTexture(
        BackgroundTexture, 
        0,
        0,
        WHITE
    );

    if (currentState == MENU) {
        DrawMenu();
    } 
    else if (currentState == PLAYING) {
        for (int i = 0; i < GameInstances.size(); i++) {
            GameInstances[i]->Draw();
        }
        playerShip->Draw();
        DrawScore();
        DrawLives();
    }
    else if (currentState == GAME_OVER) {
        DrawText("GAME OVER", WIDTH/2 - MeasureText("GAME OVER", 40)/2, HEIGHT/2 - 40, 40, RED);
        DrawText(TextFormat("FINAL SCORE: %d", score), WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 20)/2, HEIGHT/2 + 20, 20, WHITE); // measure text est utilier pour centrer le texte
        DrawText("PRESS ENTER TO PLAY AGAIN", WIDTH/2 - MeasureText("PRESS ENTER TO PLAY AGAIN", 20)/2, HEIGHT/2 + 60, 20, WHITE);
    }
}

void Game::ShipCollisions() {

    if (playerShip->ShipIsImmune()) return;

    Vector2 Points[3];
    Points[0] = playerShip->GetPoint(0);
    Points[1] = playerShip->GetPoint(1);
    Points[2] = playerShip->GetPoint(2);

    for (int i = 0; i < GameInstances.size(); i++) {
        if (GameInstances[i]->GetObjectType() == ASTEROID) {
            Asteroid* currentAsteroid = dynamic_cast<Asteroid*>(GameInstances[i]);
            
            if (currentAsteroid != nullptr) {
                float asteroidRadius = currentAsteroid->GetAsteroidRadius();
                Vector2 asteroidCenter = currentAsteroid->GetPosition();

                for (int pointIndex = 0; pointIndex < 3; pointIndex++) {
                    float distance = sqrt(
                        pow(Points[pointIndex].x - asteroidCenter.x, 2) +
                        pow(Points[pointIndex].y - asteroidCenter.y, 2)
                    );

                    if (distance < asteroidRadius * currentAsteroid->GetAsteroidSize()) {
                        if (playerShip->ShipIsImmune()) break;
                        playerShip->MakeImmune(3);
                        LoseLife();
                        currentAsteroid->SplitAsteroid();
                        break;
                    }
                }
            }
        }
    }
}



void Game::ProjectileCollisions() {
    for (int i = 0; i < GameInstances.size(); i++) {
        if (GameInstances[i]->GetObjectType() == PROJECTILE) {
            Projectile* currentProjectile = dynamic_cast<Projectile*>(GameInstances[i]);
            
            if (!currentProjectile) continue;
            
            Vector2 projectilePos = currentProjectile->GetPosition();
            float projectileRadius = currentProjectile->GetRadius();
            
            if (currentProjectile->IsPlayerProjectile()) {
                for (int j = 0; j < GameInstances.size(); j++) {
                    if (i == j) continue;
                    if (GameInstances[j]->GetObjectType() == ASTEROID) {
                        Asteroid* currentAsteroid = dynamic_cast<Asteroid*>(GameInstances[j]);
                        if (!currentAsteroid) continue;
                        
                        Vector2 asteroidPos = currentAsteroid->GetPosition();
                        float asteroidRadius = currentAsteroid->GetAsteroidRadius() * currentAsteroid->GetAsteroidSize();
                        
                        float distance = sqrt(
                            pow(projectilePos.x - asteroidPos.x, 2) +
                            pow(projectilePos.y - asteroidPos.y, 2)
                        );
                        
                        if (distance < (projectileRadius + asteroidRadius)) {
                            if (currentAsteroid->GetAsteroidSize() != 0) {
                                score += 250 / currentAsteroid->GetAsteroidSize();
                            }
                            currentAsteroid->SplitAsteroid();
                            RemoveGameObject(currentProjectile);
                            i--;
                            break;
                        }
                    }
                    //Si il touche une soucoupe
                  
                }
            }
          
            else {
               if (playerShip && !playerShip->ShipIsImmune()) {
                Vector2 shipPos = playerShip->GetPosition();
                
                float shipRadius = 15.0f;
                
                float distance = sqrt(
                    pow(shipPos.x - projectilePos.x, 2) +
                    pow(shipPos.y - projectilePos.y, 2)
                );
                
                if (distance < (projectileRadius + shipRadius)) {
                    playerShip->MakeImmune(3);
                    LoseLife();
                    RemoveGameObject(currentProjectile);
                }
            }
                
                for (int j = 0; j < GameInstances.size(); j++) {
                    if (i == j || i < 0 || i >= GameInstances.size()) continue;
                    
                    if (GameInstances[j]->GetObjectType() == ASTEROID) {
                        Asteroid* currentAsteroid = dynamic_cast<Asteroid*>(GameInstances[j]);
                        if (!currentAsteroid) continue;
                        
                        Vector2 asteroidPos = currentAsteroid->GetPosition();
                        float asteroidRadius = currentAsteroid->GetAsteroidRadius() * currentAsteroid->GetAsteroidSize();
                        
                        float distance = sqrt(
                            pow(projectilePos.x - asteroidPos.x, 2) +
                            pow(projectilePos.y - asteroidPos.y, 2)
                        );
                        
                        if (distance < (projectileRadius + asteroidRadius)) {
                            currentAsteroid->SplitAsteroid();
                            RemoveGameObject(currentProjectile);
                            i--;
                            break;
                        }
                    }
                }
            }
        }
    }
}

Ship* Game::GetPlayerShip() {
    return playerShip;
}

void Game::DrawLives() {
    DrawText(TextFormat("LIVES: %d", lives), 20, 50, 20, WHITE);
    
    for (int i = 0; i < lives; i++) {
        Vector2 lifePosition = { 120.0f + (i * 30.0f), 55.0f };
        
        Vector2 p1 = { lifePosition.x, lifePosition.y - 10.0f };
        Vector2 p2 = { lifePosition.x - 8.0f, lifePosition.y + 5.0f };
        Vector2 p3 = { lifePosition.x + 8.0f, lifePosition.y + 5.0f };
        
        DrawTriangle(p1, p2, p3, WHITE);
    }
}

void Game::DrawScore() {
    DrawText(TextFormat("SCORE: %d", score), 20, 20, 20, WHITE);
}



void Game::LoseLife() {
    lives--;
    if (lives <= 0) {
        currentState = GAME_OVER;
        level = 0;
    }
}

void Game::StartNewLevel() {
    for (int i = 0; i < GameInstances.size(); i++) {
        if (GameInstances[i]->GetObjectType() != SHIP) {
            delete GameInstances[i];
            GameInstances.erase(GameInstances.begin() + i);
            i--;
        }
    }
    
    int asteroidCount = 2 + level;
    SpawnAsteroids(asteroidCount);
    
    
    playerShip->MakeImmune(2.0f);
    
    Vector2 screenCenter = { WIDTH/2, HEIGHT/2 };
    playerShip->SetPosition(screenCenter);
}

bool Game::CheckIfLevelIsFinished() {
    for (int i = 0; i < GameInstances.size(); i++) {
        if (GameInstances[i]->GetObjectType() == ASTEROID) {
            return false;
        }
    }
    return true; // Si aucun asteroid n'a été trouvé dans le vector
}



void Game::CheckGainLife() {
    if (score > GainLifeCheckpoint) {
        GainLifeCheckpoint += 10000;
        lives++;
    }
}