#include "raylib.h"
#include <math.h>

#define MAX_TRAIL 20

int main()
{
    const int largura = 800;
    const int altura = 450;

    InitWindow(largura, altura, "PONG ARCADE");
    InitAudioDevice();
    SetTargetFPS(60);

    // Sons
    Sound hit = LoadSound("audio/hit.mp3");
    Sound point = LoadSound("audio/point.mp3");
    Music music = LoadMusicStream("audio/music.mp3");
    PlayMusicStream(music);

    // Bola
    Vector2 bolaPos = { largura/2, altura/2 };
    Vector2 bolaVel = { 5, 5 };
    float bolaRaio = 10;

    // Rasto
    Vector2 trail[MAX_TRAIL] = {0};

    // Barras
    Rectangle player = { 30, altura/2 - 50, 10, 100 };
    Rectangle enemy = { largura - 40, altura/2 - 50, 10, 100 };

    int playerScore = 0;
    int enemyScore = 0;

    bool gameOver = false;

    float flashTimer = 0;
    float shakeTimer = 0;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (!gameOver)
        {
            // Movimento jogador
            if (IsKeyDown(KEY_W)) player.y -= 6;
            if (IsKeyDown(KEY_S)) player.y += 6;

            if (player.y < 0) player.y = 0;
            if (player.y + player.height > altura)
                player.y = altura - player.height;

            // IA simples
            if (enemy.y + enemy.height/2 < bolaPos.y) enemy.y += 5;
            if (enemy.y + enemy.height/2 > bolaPos.y) enemy.y -= 5;

            // Atualizar rasto
            for (int i = MAX_TRAIL-1; i > 0; i--)
                trail[i] = trail[i-1];
            trail[0] = bolaPos;

            // Movimento bola
            bolaPos.x += bolaVel.x;
            bolaPos.y += bolaVel.y;

            if (bolaPos.y <= 0 || bolaPos.y >= altura)
                bolaVel.y *= -1;

            // Colisão jogador
            if (CheckCollisionCircleRec(bolaPos, bolaRaio, player))
            {
                bolaVel.x *= -1;
                PlaySound(hit);
                flashTimer = 0.1f;
            }

            // Colisão inimigo
            if (CheckCollisionCircleRec(bolaPos, bolaRaio, enemy))
            {
                bolaVel.x *= -1;
                PlaySound(hit);
                flashTimer = 0.1f;
            }

            // Pontuação
            if (bolaPos.x < 0)
            {
                enemyScore++;
                PlaySound(point);
                bolaPos = (Vector2){ largura/2, altura/2 };
                shakeTimer = 0.3f;
            }

            if (bolaPos.x > largura)
            {
                playerScore++;
                PlaySound(point);
                bolaPos = (Vector2){ largura/2, altura/2 };
                shakeTimer = 0.3f;
            }

            if (playerScore == 5 || enemyScore == 5)
                gameOver = true;
        }
        else
        {
            if (IsKeyPressed(KEY_R))
            {
                playerScore = 0;
                enemyScore = 0;
                bolaPos = (Vector2){ largura/2, altura/2 };
                gameOver = false;
            }
        }

        // Atualizar timers
        flashTimer -= GetFrameTime();
        shakeTimer -= GetFrameTime();

        Vector2 shakeOffset = {0};
        if (shakeTimer > 0)
        {
            shakeOffset.x = GetRandomValue(-5,5);
            shakeOffset.y = GetRandomValue(-5,5);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        Camera2D camera = {0};
        camera.offset = shakeOffset;
        camera.target = (Vector2){0, 0};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;

        BeginMode2D(camera);

        //Colocar sombras na bola
        DrawCircleV(bolaPos, bolaRaio + 8, Fade(WHITE, 0.1f));
        DrawCircleV(bolaPos, bolaRaio + 4, Fade(WHITE, 0.2f));
            

        //Colocar Rasto na bola
        for (int i = 0; i < MAX_TRAIL; i++)
        {
           
            float alpha = 1.0f - (float)i/MAX_TRAIL;
            DrawCircleV(trail[i], bolaRaio, Fade(WHITE, alpha));
        }

        //Linha separadora
        for (int i = 0; i < altura; i += 20)
        {
             DrawRectangle(largura/2 - 2, i, 4, 10, Fade(WHITE, 0.5f));
        }   

        //Desenha os jogadores
        DrawRectangleRec(player, WHITE);
        DrawRectangleRec(enemy, WHITE);

        //Desenha a Pontuação 
        DrawText(TextFormat("%02d", playerScore), largura/4, 20, 60, GREEN);
        DrawText(TextFormat("%02d", enemyScore), largura*3/4, 20, 60, WHITE);

        EndMode2D();

        // Flash
        if (flashTimer > 0)
            DrawRectangle(0, 0, largura, altura, Fade(WHITE, 0.3f));

        if (gameOver)
        {
            if (playerScore == 5)
                DrawText("VOCE VENCEU!", largura/2 - 120, altura/2 - 20, 30, GREEN);
            else
                DrawText("VOCE PERDEU!", largura/2 - 120, altura/2 - 20, 30, RED);

            DrawText("Pressione R para reiniciar", largura/2 - 160, altura/2 + 20, 20, WHITE);
        }

        EndDrawing();
    }

    UnloadSound(hit);
    UnloadSound(point);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}