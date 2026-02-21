#include "raylib.h"
#include <math.h>

#define MAX_TRAIL 20
#define MAX_SCORE 5

// Estados do jogo
typedef enum {
    MENU,          // Tela principal
    MODE_SELECT,   // Escolher 1 jogador ou 2 jogadores
    PLAYING,       // Jogo a decorrer
    GAMEOVER       // Tela de vitória
} GameState;

int main()
{
    const int largura = 800;
    const int altura = 450;

    InitWindow(largura, altura, "PONG MARIA PRO");
    InitAudioDevice();
    SetTargetFPS(60);

    // Sons
    Sound hit = LoadSound("audio/hit.mp3");
    Sound point = LoadSound("audio/point.mp3");
    Music music = LoadMusicStream("audio/music.mp3");
    PlayMusicStream(music);

    // Variáveis principais
    GameState state = MENU;
    bool versusCP = true;   // true = joga contra Computador | false = 2 jogadores
    float velocidadeInicial = 5.0f;
    float tempoEspera = 0.0f;   // Delay antes da bola mexer
    float tempo = 0.0f;

    // Bola
    Vector2 bolaPos = { largura/2, altura/2 };
    Vector2 bolaVel = { 5, 5 };
    float bolaRaio = 10;

    Vector2 trail[MAX_TRAIL] = {0};

    // Barras
    Rectangle player = { 30, altura/2 - 50, 10, 100 };
    Rectangle enemy  = { largura - 40, altura/2 - 50, 10, 100 };

    int playerScore = 0;
    int enemyScore  = 0;

    float flashTimer = 0;
    float shakeTimer = 0;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        switch(state)
        {
            // ===================== MENU =====================
            case MENU:
                if (IsKeyPressed(KEY_ENTER))
                    state = MODE_SELECT;

                if (IsKeyPressed(KEY_ESCAPE))
                    CloseWindow();
            break;

            // ===================== ESCOLHER MODO =====================
            case MODE_SELECT:
                if (IsKeyPressed(KEY_ONE))
                {
                    versusCP = true;
                    state = PLAYING;
                }
                if (IsKeyPressed(KEY_TWO))
                {
                    versusCP = false;
                    state = PLAYING;
                }
            break;

            // ===================== JOGO =====================
            case PLAYING:

                // Movimento jogador 1
                if (IsKeyDown(KEY_W)) player.y -= 6;
                if (IsKeyDown(KEY_S)) player.y += 6;

                // Limites do jogador 1
                if (player.y < 0) player.y = 0;
                if (player.y + player.height > altura)
                    player.y = altura - player.height;

                // Se for Computador
                if (versusCP)
                {
                    if (enemy.y + enemy.height/2 < bolaPos.y) enemy.y += 5;
                    if (enemy.y + enemy.height/2 > bolaPos.y) enemy.y -= 5;
                }
                else // 2 jogadores
                {
                    //Movimento do Jogador 2
                    if (IsKeyDown(KEY_UP)) enemy.y -= 6;
                    if (IsKeyDown(KEY_DOWN)) enemy.y += 6;

                    // Limites do Jogador 2
                    if (enemy.y < 0) enemy.y = 0;
                    if (enemy.y + enemy.height > altura)
                        enemy.y = altura - enemy.height;
                }

                // Atualizar rasto
                for (int i = MAX_TRAIL-1; i > 0; i--)
                    trail[i] = trail[i-1];
                trail[0] = bolaPos;

                // Movimento da bola
                 if (tempoEspera <= 0)
                {
                    bolaPos.x += bolaVel.x;
                    bolaPos.y += bolaVel.y;
                }
                else
                {
                    tempoEspera -= GetFrameTime();
                }

                // Colisão topo/fundo
                if (bolaPos.y <= 0 || bolaPos.y >= altura)
                    bolaVel.y *= -1;

                // Colisão com barras
                if (CheckCollisionCircleRec(bolaPos, bolaRaio, player) ||
                    CheckCollisionCircleRec(bolaPos, bolaRaio, enemy))
                {
                    bolaVel.x *= -1.05f;  // aumenta velocidade progressivamente
                    PlaySound(hit);
                    flashTimer = 0.1f;
                }

                // Pontuação Jogador 2
                if (bolaPos.x < 0)
                {
                    enemyScore++;
                    PlaySound(point);

                    bolaPos = (Vector2){ largura/2, altura/2 };

                    bolaVel.x = velocidadeInicial;  // Vai para direita (quem sofreu começa)
                    bolaVel.y = GetRandomValue(-4,4);

                    tempoEspera = 1.0f;  // 1 segundo parado
                    shakeTimer = 0.3f;
                }

                // Pontuação Jogador 1 
                if (bolaPos.x > largura)
                {
                    playerScore++;
                    PlaySound(point);

                    bolaPos = (Vector2){ largura/2, altura/2 };

                    bolaVel.x = -velocidadeInicial; // Vai para esquerda
                    bolaVel.y = GetRandomValue(-4,4);

                    tempoEspera = 1.0f;
                    shakeTimer = 0.3f;
                }

                // Verifica vitória
                if (playerScore == MAX_SCORE || enemyScore == MAX_SCORE)
                    state = GAMEOVER;

            break;

            // ===================== GAME OVER =====================
            case GAMEOVER:
                if (IsKeyPressed(KEY_R))
                {
                    playerScore = 0;
                    enemyScore = 0;
                    bolaVel = (Vector2){5,5};
                    bolaPos = (Vector2){ largura/2, altura/2 };
                    state = MENU;
                }
            break;
        }

        // Timers efeitos
        flashTimer -= GetFrameTime();
        shakeTimer -= GetFrameTime();

        Vector2 shakeOffset = {0};
        if (shakeTimer > 0)
        {
            shakeOffset.x = GetRandomValue(-5,5);
            shakeOffset.y = GetRandomValue(-5,5);
        }

        BeginDrawing();
        DrawRectangleGradientV(0, 0, largura, altura, DARKBLUE, BLACK);

        // Fundo preto base
        ClearBackground(BLACK);

        // Grade neon estilo retrô
        for (int i = 0; i < largura; i += 40)
        {
            DrawLine(i, 0, i, altura, Fade(PURPLE, 0.15f));
        }

        for (int j = 0; j < altura; j += 40)
        {
            DrawLine(0, j, largura, j, Fade(PURPLE, 0.15f));
        }

        ClearBackground(BLACK);

        for (int i = 0; i < largura; i++)
        {
            float wave = 20 * sinf(i * 0.01f + tempo);
            DrawLine(i, altura/2 + wave, i, altura, Fade(DARKBLUE, 0.6f));
        }       

        Camera2D camera = {0};
        camera.offset = shakeOffset;
        camera.zoom = 1.0f;

        BeginMode2D(camera);

        // Linha central
        for (int i = 0; i < altura; i += 20)
            DrawRectangle(largura/2 - 2, i, 4, 10, Fade(WHITE, 0.4f));

        // Bola glow
        DrawCircleV(bolaPos, bolaRaio + 6, Fade(WHITE, 0.2f));

        // Rasto
        for (int i = 0; i < MAX_TRAIL; i++)
        {
            float alpha = 1.0f - (float)i/MAX_TRAIL;
            DrawCircleV(trail[i], bolaRaio, Fade(WHITE, alpha));
        }

        // Barras
        DrawRectangleRec(player, WHITE);
        DrawRectangleRec(enemy, WHITE);

        // Pontuação
        if(playerScore > enemyScore)
        {
            DrawText(TextFormat("%02d", playerScore), largura/4, 20, 60, GREEN);
            DrawText(TextFormat("%02d", enemyScore), largura*3/4, 20, 60, RED);
        }
        else if(playerScore < enemyScore)
        {
            DrawText(TextFormat("%02d", playerScore), largura/4, 20, 60, RED);
            DrawText(TextFormat("%02d", enemyScore), largura*3/4, 20, 60, GREEN);
        }
        else
        {
            DrawText(TextFormat("%02d", playerScore), largura/4, 20, 60, WHITE);
            DrawText(TextFormat("%02d", enemyScore), largura*3/4, 20, 60, WHITE);
        }

        EndMode2D();

        // Flash colisão
        if (flashTimer > 0)
            DrawRectangle(0, 0, largura, altura, Fade(WHITE, 0.3f));

        // ===================== TELAS =====================
        if (state == MENU)
        {
            DrawText("PONG MARIA Pro", largura/2 - 170, 120, 50, WHITE);
            DrawText("ENTER - Jogar", largura/2 - 120, 200, 30, GREEN);
            DrawText("ESC - Sair", largura/2 - 100, 240, 30, RED);
        }

        if (state == MODE_SELECT)
        {
            DrawText("Escolha o modo:", largura/2 - 150, 150, 30, WHITE);
            DrawText("1 - Contra Computador", largura/2 - 170, 200, 25, GREEN);
            DrawText("2 - Dois Jogadores", largura/2 - 150, 240, 25, SKYBLUE);
        }

        if (state == GAMEOVER)
        {
            if (playerScore == MAX_SCORE)
                DrawText("JOGADOR 1 VENCEU!", largura/2 - 170, altura/2 - 20, 30, GREEN);
            else
                DrawText("JOGADOR 2 VENCEU!", largura/2 - 170, altura/2 - 20, 30, RED);

            DrawText("Pressione R para voltar ao menu", largura/2 - 200, altura/2 + 20, 20, WHITE);
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