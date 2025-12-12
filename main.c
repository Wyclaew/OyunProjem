#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "raylib.h"

//  enum kendi değişken türümüzü yaratmamızı sağlıyor
typedef enum {
    MENU,       //  giriş ekranı
    GAMEPLAY,       //  oyun 
    GAME_OVER      //  lose ekranı
} GameState;

//  düşman için struct tanımlama
typedef struct {
    Vector2 pos;
    Color color;
    float speed;
    float health;
    bool active;
} enemy;

//  mermi yapısı
typedef struct{
    Vector2 pos;    //  merminin konumu
    Vector2 dir;    //  gideceği yön
    float speed;    //  hızı
    bool active;   //  mermi havada mı
    float radius;   //  mermi büyüklüğü
    
} projectile;


const int screenWidth = 800;    //  ekran genişlik
const int screenHeight = 600;   //  ekran yükseklik
GameState currentState = MENU;  //  oyuun başlatınca ilk hangi ekran gelsin

#define max_bullets 50

//  düşman sayısını istediğimiza zaman değiştirmek için ve düşman sayısını değiştirilemez yapmak için
#define max_enemies 10

//  düşmanlar için dizi
enemy enemies[max_enemies];

float enemyRadius = 12.0f;

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

        
    } 
    while(distance < minDistance);
    return spawnPos;
}




//  kullanacağımız fonksiyonları önceden tanıtma
void updateGame(float dt);  //  matematiksel şeylerin döneceği yer
void drawGame(void);    //  çizim işlerinin dönceği yer
void UpdatePlayerMovement(float dt);
void UpdateEnemyAI(float dt);
void UpdateBullets(float dt);
void CheckCollisions();
void UpdateShootingSystem(float dt);
void UpdateMenu();
void UpdateMenuDrawings();
void UpdateGameplayDrawings();
void UpdateGameoverDrawings();
void UpdateCollisions(projectile bullets[], int bullet_count, enemy enemies[], int enemy_count);
//  eklenecek düşmanların hareketini ve çizimini farklı yerlerde hesaplayıp optimize etmek için



    //  OYUNCU TANIMLAMA (global)
    Vector2 playerPos = { (float)screenWidth/2, (float)screenHeight/2};

    //  kare başına oyuncunun hareket hızı (5.0f deki f ordaki değerin double değil float olduğunu açıklamak için yardımcı olma)
    float playerSpeed = 300.0f;

    float playerRadius = 20.0f;



        //  mermi havuzu başlatma
    projectile bullets[max_bullets];


    // mermi mekaniği boş mermi varmı diye kontrol edip ateşleme
void FireBullet(projectile bullets[], Vector2 playerPos, Vector2 targetPos){

    for (int i = 0; i < max_bullets; i++){
        if(!bullets[i].active){   //  mermi aktif kullanılıyor mu

            bullets[i].active = true;   //  artık kullanılıyor
            bullets[i].pos = playerPos; //  oyuncunun olduğu yerden çıkıcak
            bullets[i].speed =200.0f;   //mermi hızı
            bullets[i].radius = 4.0f;

            //  hedef yönü hesaplama(düşman takip mantığının aynısı)
            float dx = targetPos.x - playerPos.x;
            float dy = targetPos.y - playerPos.y;
            float distance = sqrt(dx * dx + dy * dy);

            //  sıfıra bölünme hatasını engelleme
            if(distance > 0){
                //  birim vektöre dönüştürme
                bullets[i].dir.x = dx / distance;
                bullets[i].dir.y = dy / distance;
            }
            else {
                bullets[i].dir = (Vector2){1, 0};
            }
            break;  //  bir mermi için yapıp döngüden çıkıyoruz
        }

    }
    
}


        


    //  saldırı zamanlayıcısı
    float shootTimer = 0.0f;
    float shootCooldown = 0.5f; //  yarım saniyede bir ateş et

    //  mermi ve düşman çarpışma kontrolcüsü
    void UpdateCollisions(projectile bullets[], int bullet_count, enemy enemies[], int enemy_count){
        

        //  mermileri tek tek gezme
        for (int i = 0; i < bullet_count; i++){
            //  eğer mermi aktif ekranda değilse o mermiyi direkt geçiyor (optimizasyon)
            if(!bullets[i].active) continue;

            //  düşmanları gezme
            for (int j = 0; j < enemy_count; j++){

                //  eğer düşman ölüyse mermi ona çarpmaz
                if(!enemies[j].active) continue;

                //  çarpışma anı 
                //  mermi ile düşman birbirlerine değdi mi ?
                if(CheckCollisionCircles(bullets[i].pos, bullets[i].radius, enemies[j].pos, enemyRadius)){

                    //  vurulma anı mermiyi yoketme
                    bullets[i].active = false;

                    //  düşman yok oldu
                    enemies[j].active = false;
                    // !!!!! ileride xp ve altın düşürme kodu gelicek
                    //  !!!!!!! ileride sound effect gelicek (patlama sesi)
                    //  mermi bir düşmanı buldu ve yok oldu diğerlerine bakmaya gerek yok döngüden çıkıyoruz
                    break;
                }
            }
            
        }
        
    }
    

//  programın başladığı yer
int main(void){
    //  işletim sisteminden grafik belleği isteme
    InitWindow(screenWidth, screenHeight, "Phase 7: Impact & Destruction");

    //  fps limitleme
    SetTargetFPS(60);

    //  mevcut ekranı menuye alıyoruz
    GameState currentState = MENU;








    

    //  oyun döngüsü
    while(!WindowShouldClose()){

        float dt = GetFrameTime();  //  geçen süre(delta time)

        //  ilk adım hesaplama klavye girdileri, hareketler, canlar vs.
        updateGame(dt);

        //  ikinci adım ekrana oyunun son halini çizdirme
        drawGame();

    }
    //  aldığımız belliği geri verme
    CloseWindow();

    return 0;

}

void UpdatePlayerMovement(float dt){
            //  yukarı
            if(IsKeyDown(KEY_W)) playerPos.y -= playerSpeed * dt;

            // aşağı
            if(IsKeyDown(KEY_S)) playerPos.y += playerSpeed * dt;

            // sağa
            if(IsKeyDown(KEY_D)) playerPos.x += playerSpeed * dt;

            //  sola
            if(IsKeyDown(KEY_A)) playerPos.x -= playerSpeed * dt;

            //  ekranın solundan dışarıya çıkamaması için
            if(playerPos.x < 20) playerPos.x = 20;

            //  sağından çıkamaması için
            if(playerPos.x > 780) playerPos.x = 780;

            //  yukardan çıkmaması için
            if(playerPos.y < 20) playerPos.y = 20;

            //  aşağıdan çıkmaması için
            if(playerPos.y > 580) playerPos.y = 580;
            //  hareket mantığı ve oyun içi
            //  IsKeyDown: tuşa basılı tutulduğu sürece true döner

}


void UpdateBullets(float dt){
            //  yeni mermileri güncelleme
            for (int i = 0; i < max_bullets; i++){
                if(!bullets[i].active) continue;
                if(bullets[i].active){
                    //  mermiyi ilerletme
                    bullets[i].pos.x += bullets[i].dir.x * bullets[i].speed * dt;
                    bullets[i].pos.y += bullets[i].dir.y * bullets[i].speed * dt;

                    //  mermi ekran dışına çıktı mı kontrol etme
                    if(bullets[i].pos.x < 0 || bullets[i].pos.x > screenWidth || bullets[i].pos.y < 0 || bullets[i].pos.y > screenHeight){
                        bullets[i].active = false;  //  mermi ekran dışına çıkarsa false durumuna alıyoruz
                    }
                }
            }
                
                 //  mermi ve düşman çarpışması kontrol etme fonksiyonu
            UpdateCollisions(bullets, max_bullets, enemies, max_enemies);


                
}


void UpdateMenu(){
            //  düşmanlara rastgele başlangıç değeri verme
    for (int i = 0; i < max_enemies; i++){
        enemies[i].pos.x = GetRandomValue(0, screenWidth);
        enemies[i].pos.y = GetRandomValue(0, screenHeight);

        //düşman rengi
        enemies[i].color = RED;
    }
            //  enter tuşuna bastığımızda oyun durumu menüden gameplaye geçicek 
            //  IsKeyPressed: tuşa basıldığı anda true döner
                if (IsKeyPressed(KEY_ENTER)){
                //  oyunu sıfırlama 
                for (int i = 0; i < max_bullets; i++){
                 bullets[i].active = false;
               }
                playerPos = (Vector2){screenWidth / 2, screenHeight / 2};

                //  düşmanları başlatma
                for (int i = 0; i < max_enemies; i++){
                    enemies[i].pos = GetSafeSpawnPosition(playerPos, 300.0f);
                    enemies[i].speed = (float)GetRandomValue(2, 4);
                    enemies[i].color = RED;
             //     enemies[i].health = 100.0f;
                    enemies[i].active = true;
                } 
                shootTimer = 0.0f;
                currentState = GAMEPLAY;
            }
               

}


void CheckCollisions(){
                    //  düşman çarpışma kontrolü
            for (int i = 0; i < max_enemies; i++){
                if(!enemies[i].active) continue;
                if (CheckCollisionCircles(playerPos, playerRadius, enemies[i].pos, enemyRadius)){
                //  çarpışma yaşandığı için oyunu bitirme
                currentState = GAME_OVER;
            }
                
            }

}
            





void UpdateEnemyAI(float dt){


                //  döngü ile ekrandaki tüm düşmanları tek tek kontrol etme
            for (int i = 0; i < max_enemies; i++){

                //  eğer düşman aktif değilse çizmeye devam etme
                if(!enemies[i].active) continue;

                //  düşman ve oyuncu arasındaki y mesafesi
                float dx = playerPos.x - enemies[i].pos.x;

                //  düşman ve oyuncu arasındaki x mesafesi
                float dy = playerPos.y - enemies[i].pos.y;

                //  düşman ve oyuncu arasındaki mesafeyi bulma
                float enemyDistance = sqrt(dx * dx + dy * dy);

                /*  hareket normalizasyonu eğer mesafe sıfır ise yani üst üstelerse
                işlem yaptırmayacağız yoksa 0'a bölme hatası alırız */
                if(enemyDistance > 0){
                    float enemySpeed = 100.0f; // düşman hızı
                    /*  !! düşman pozisyonuna direkt olarak dx ve dy yi eklersek düşman bize ne kadar
                    yakın olursa o kadar yavaş ne kadar uzak olursa da o kadar hızlı gelirdi*/
                    //  birim vektör elde etme
                    enemies[i].pos.x += (dx / enemyDistance) * enemySpeed * dt;
                    enemies[i].pos.y += (dy / enemyDistance) * enemySpeed * dt;

                }
            }

}


void UpdateShootingSystem(float dt){
    //  oto saldırı mantığı en yakındakini bulup ateş etme

            shootTimer += dt;   //  sayacı arttırma
            
            if(shootTimer >= shootCooldown){
                int nearestEnemyIndex = -1;
                float minDistance = 9999999.0f;
                for (int i = 0; i < max_enemies; i++){  //  sadece aktif ve varsayılan düşmanlara bakacağız
                    if(!enemies[i].active) continue;
                    float dx = enemies[i].pos.x - playerPos.x;
                    float dy = enemies[i].pos.y - playerPos.y;
                    float dist = sqrt(dx * dx + dy * dy);
                
                    if(dist < minDistance){
                        minDistance = dist;
                        nearestEnemyIndex = i;
                    }
                }
                //  eğer düşman bulduysan ateşle
                if(nearestEnemyIndex != -1){
                    FireBullet(bullets, playerPos, enemies[nearestEnemyIndex].pos);
                    shootTimer = 0.0f;   //  sayacı sıfırla                }
                
            }
        }



            

}


void UpdateMenuDrawings(){
            const char *menuText = "Oyuna baslamak için ENTER'A bas";

            int menuFontSize = 30; // menü yazısının font büyüklüğü

            int menuTextWidth = MeasureText(menuText, menuFontSize); // metnin kaç piksel kapladığı

            int menuCenterX = (screenWidth / 2) - (menuTextWidth / 2);

            int menuCenterY = (screenHeight / 2) - (menuFontSize / 2);

            DrawText(menuText, menuCenterX, menuCenterY, menuFontSize, LIGHTGRAY);

}


void UpdateGameplayDrawings(){
                DrawText("Hareket için W, A, S, D", 10, 10, 20, LIGHTGRAY);

                //  debug ekranı ekleme
            int activeBullets = 0;
            int activeEnemys = 0;

            for (int i = 0; i < max_bullets; i++){
                if( bullets[i].active) activeBullets++;

            }

            for (int i = 0; i < max_enemies; i++){
                if(enemies[i].active) activeEnemys++;
            }
            

            char debugText[100];
            sprintf(debugText, "Fps : %d  |  Mermiler : %d  |  Dusmanlar : %d", GetFPS(), activeBullets, activeEnemys);
            DrawText(debugText, 10, 40, 20, GREEN);



            //  oyuncumuzun çizimi
            DrawCircleV(playerPos, playerRadius, BLUE);

            //  düşman çizimi
            for (int i = 0; i < max_enemies; i++){
            
                //  sadece aktif düşmanları çiz
                if(!enemies[i].active) continue;

                DrawCircleV(enemies[i].pos, enemyRadius, enemies[i].color);
            }
        

            //  mermilerin çizimi
            for (int i = 0; i < max_bullets; i++){
                if(bullets[i].active){
                    DrawCircleV(bullets[i].pos, bullets[i].radius, BLACK);
                }
            }


}


void UpdateGameoverDrawings(){
                const char *gameOverText = "öldün !\n menü için enter";
              int gameOverFontSize = 40;

             //  yazının genişliği
             int gameOverTextWidth = MeasureText(gameOverText, gameOverFontSize);

             int gameOverCenterX = (screenWidth / 2) - (gameOverTextWidth / 2);
            
             int gameOverCenterY = (screenHeight / 2) - (gameOverFontSize / 2);

             DrawText(gameOverText, gameOverCenterX, gameOverCenterY, gameOverFontSize, RED);
}


void updateGame(float dt){


    switch(currentState){



            case MENU:

            UpdateMenu();

            break;

            case GAMEPLAY:
            
            UpdatePlayerMovement(dt);

            UpdateEnemyAI(dt);

            CheckCollisions();

            UpdateBullets(dt);

            UpdateShootingSystem(dt);
            
            break;


            case GAME_OVER:
            if(IsKeyPressed(KEY_ENTER)){
                currentState = MENU;
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
            case MENU:

            UpdateMenuDrawings();

            break;

            // oyun içi çizimler oyuncu düşmanlar vs.
            case GAMEPLAY:

            UpdateGameplayDrawings();
            
            break;

            //  oyun bitişi
            case GAME_OVER:

            UpdateGameoverDrawings();

            break;
    }
    // çizim işlemini bitirme
        EndDrawing();
    }

    
       