#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "raylib.h"

//  enum kendi değişken türümüzü yaratmamızı sağlıyor
typedef enum {
    menu,       //  giriş ekranı
    gameplay,       //  oyun 
    game_over,      //  lose ekranı
} GameState;

//  düşmanın konumu ve rengi için struct tanımalama
typedef struct {
    Vector2 pos;
    Color color;
    float speed;
    float healt;
    bool active;
} enemy;

const int screenWidth = 800;    //  ekran genişlik
const int screenHeight = 600;   //  ekran yükseklik
GameState currentState = menu;  //  oyuun başlatınca ilk hangi ekran gelsin

//  düşman sayısını istediğimiza zaman değiştirmek için ve düşman sayısını değiştirilemez yapmak için
#define max_enemies 10

//  düşmanlar için dizi
enemy enemies[max_enemies];

float enemyRadius = 15.0f;

//  oyuncunun içinde düşman doğmama mekaniği
Vector2 GetSafeSpawnPosition (Vector2 playerPos, float minDistance){
    Vector2 spawnPos;
    float distance = 0.0f;

    do{
        spawnPos.x = (float)GetRandomValue(0, screenWidth);
        spawnPos.y = (float)GetRandomValue(0, screenHeight);

        float dx = playerPos.x - spawnPos.x;
        float dy = playerPos.y - spawnPos.y;
        distance = sqrt(dx * dx + dy * dy);

        return spawnPos;
    } 
    while(distance < minDistance);
}




//  kullanacağımız fonksiyonları önceden tanıtma
void updateGame(void);  //  matematiksel şeylerin döneceği yer
void drawGame(void);    //  çizim işlerinin dönceği yer
//  eklenecek düşmanların hareketini ve çizimini farklı yerlerde hesaplayıp optimize etmek için


    //  OYUNCU TANIMLAMA (global)
    Vector2 playerPos = { (float)screenWidth/2, (float)screenHeight/2};

    //  kare başına oyuncunun hareket hızı (5.0f deki f ordaki değerin double değil float olduğunu açıklamak için yardımcı olma)
    float playerSpeed = 5.0f;

    float playerRadius = 20.0f;
    


//  programın başladığı yer
int main(void){
    //  işletim sisteminden grafik belleği isteme
    InitWindow(screenWidth, screenHeight, "Circle Survivor - v0.1.1");

    //  fps limitleme
    SetTargetFPS(60);

    //  mevcut ekranı menuye alıyoruz
    GameState currentState = menu;


    //  düşmanlara rastgele başlangıç değeri verme
    for (int i = 0; i < max_enemies; i++){
        enemies[i].pos.x = GetRandomValue(0, screenWidth);
        enemies[i].pos.y = GetRandomValue(0, screenHeight);

        //düşman rengi
        enemies[i].color = RED;
    }

    //  düşmanları güvenli bir yere koyma
    for (int i = 0; i < max_enemies; i++){
        //  oyuncudan en az 100 piksel uzağa koy
        enemies[i].pos = GetSafeSpawnPosition(playerPos, 100.0f);
        enemies[i].speed = GetRandomValue(2, 4);
        enemies[i].color = RED;
        enemies[i].active = true;
    }
    

    //  oyun döngüsü
    while(!WindowShouldClose()){
        //  ilk adım hesaplama klavye girdileri, hareketler, canlar vs.
        updateGame();
        
        //  ikinci adım ekrana oyunun son halini çizdirme
        drawGame();

    }
    //  aldığımız belliği geri verme
    CloseWindow();

    return 0;

}

void updateGame(void){
    switch(currentState){

            //  enter tuşuna bastığımızda oyun durumu menüden gameplaye geçicek 
            //  IsKeyPressed: tuşa basıldığı anda true döner
            case menu:
            if (IsKeyPressed(KEY_ENTER)){
                //  oyunu sıfırlama
                playerPos = (Vector2){screenWidth / 2, screenHeight / 2};
                for (int i = 0; i < max_enemies; i++){
                    enemies[i].pos = GetSafeSpawnPosition(playerPos, 100.0f);
                } 
                currentState = gameplay;
            }
                break;

            //  hareket mantığı ve oyun içi
            //  IsKeyDown: tuşa basılı tutulduğu sürece true döner
            case gameplay:
            
            //  yukarı
            if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;

            // aşağı
            if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed;

            // sağa
            if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

            //  sola
            if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;

            //  ekranın solundan dışarıya çıkamaması için
            if(playerPos.x < 20) playerPos.x = 20;

            //  sağından çıkamaması için
            if(playerPos.x > 780) playerPos.x = 780;

            //  yukardan çıkmaması için
            if(playerPos.y < 20) playerPos.y = 20;

            //  aşağıdan çıkmaması için
            if(playerPos.y > 580) playerPos.y = 580;

            //  döngü ile ekrandaki tüm düşmanları tek tek kontrol etme
            for (int i = 0; i < max_enemies; i++){
                //  düşman ve oyuncu arasındaki y mesafesi
                float dx = playerPos.x - enemies[i].pos.x;

                //  düşman ve oyuncu arasındaki x mesafesi
                float dy = playerPos.y - enemies[i].pos.y;

                //  düşman ve oyuncu arasındaki mesafeyi bulma
                float enemyDistance = sqrt(dx * dx + dy * dy);

                /*  hareket normalizasyonu eğer mesafe sıfır ise yani üst üstelerse
                işlem yaptırmayacağız yoksa 0'a bölme hatası alırız */
                if(enemyDistance > 0){
                    float enemySpeed = 2.0f; // düşman hızı
                    /*  !! düşman pozisyonuna direkt olarak dx ve dy yi eklersek düşman bize ne kadar
                    yakın olursa o kadar yavaş ne kadar uzak olursa da o kadar hızlı gelirdi*/
                    //  birim vektör elde etme
                    enemies[i].pos.x += (dx / enemyDistance) * enemySpeed;
                    enemies[i].pos.y += (dy / enemyDistance) * enemySpeed;

                } 
            }

            //  düşman çarpışma kontrolü
            for (int i = 0; i < max_enemies; i++){
                if (CheckCollisionCircles(playerPos, playerRadius, enemies[i].pos, enemyRadius)){
                //  çarpışma yaşandığı için oyunu bitirme
                currentState = game_over;
            }
                
            }
            
            

            if(IsKeyPressed(KEY_P)) currentState = game_over;

            break;

            case game_over:
            if(IsKeyPressed(KEY_ENTER)){
                currentState = menu;
            }
            break;
        }
}


    void drawGame(void){
        BeginDrawing();

        //  arkaplanı her karede temizleme temizlemezsek önceki karedeki çizim kalır ve arkamızda iz bırakırız
        ClearBackground(RAYWHITE);

        switch(currentState){

            //  menüdeki yazılar
            case menu:
            const char *menuText = "Oyuna baslamak için ENTER'A bas";

            int menuFontSize = 30; // menü yazısının font büyüklüğü

            int menuTextWidth = MeasureText(menuText, menuFontSize); // metnin kaç piksel kapladığı

            int menuCenterX = (screenWidth / 2) - (menuTextWidth / 2);

            int menuCenterY = (screenHeight / 2) - (menuFontSize / 2);

            DrawText(menuText, menuCenterX, menuCenterY, menuFontSize, LIGHTGRAY);

            break;

            // oyun içi çizimler oyuncu düşmanlar vs.
            case gameplay:
            DrawText("Hareket için W, A, S, D", 10, 10, 20, LIGHTGRAY);
            
            //  oyuncumuz
            DrawCircleV(playerPos, playerRadius, BLUE);

            //  düşman ekleme
            for (int i = 0; i < max_enemies; i++){
                DrawCircleV(enemies[i].pos, enemyRadius, enemies[i].color);
            }
            

            break;

            //  oyun bitişi
            case game_over:
            const char *gameOverText = "öldün !\n menü için enter";
              int gameOverFontSize = 40;

             //  yazının genişliği
             int gameOverTextWidth = MeasureText(gameOverText, gameOverFontSize);

             int gameOverCenterX = (screenWidth / 2) - (gameOverTextWidth / 2);
            
             int gameOverCenterY = (screenHeight / 2) - (gameOverFontSize / 2);

             DrawText(gameOverText, gameOverCenterX, gameOverCenterY, gameOverFontSize, RED);
            break;
    }
    // çizim işlemini bitirme
        EndDrawing();
    }