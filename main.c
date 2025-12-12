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

typedef struct {
    Vector2 pos;
    float speed;
    float radius;
    Texture2D texture;
} Player;

//  düşman için struct tanımlama
typedef struct {
    Vector2 pos;
    Color color;
    float speed;
    float health;
    bool active;
    int type;
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

float waveTimer = 0;    //  wave için zamanı tutucak
float spawnInterval = 1.5f;    //  ne sıklıkla düşman çıkıcak

#define max_bullets 20000
#define max_enemies 20000

//  düşmanlar için dizi
enemy enemies[max_enemies];

//  oyuncu tanımlama
Player player;

//  mermiler için dizi
projectile bullets[max_bullets];

//  oyuun başlatınca ilk hangi ekran gelsin
GameState currentState = MENU;


//  saldırı zamanlayıcısı
float shootTimer = 0.0f;

//  yarım saniyede bir ateş et
float shootCooldown = 0.5f;



//  düşman büyüklüğü
float enemyRadius = 12.0f;




//  kullanacağımız fonksiyonları önceden tanıtma
void updateGame(float dt);  //  matematiksel şeylerin döneceği yer
void drawGame(void);    //  çizim işlerinin dönceği yer
void UpdatePlayerMovement(float dt);    //  oyuncu hareketi hesaplanması
void UpdateEnemyAI(float dt);   //  düşmanların hareketlerinin hesaplanması
void UpdateBullets(float dt);   //  mermilerin hareketlerinin hesaplanması
void CheckCollisions(); //  çarpışmaların olup olmadığının kontrolü
void UpdateShootingSystem(float dt);    //  mermi
void UpdateMenu();
void UpdateMenuDrawings();
void UpdateGameplayDrawings();
void UpdateGameoverDrawings();
void UpdateCollisions(projectile bullets[], int bullet_count, enemy enemies[], int enemy_count);
void SpawnEnemy(enemy enemies[]);
void ResetGame(enemy enemies[]);
Vector2 GetSafeSpawnPosition(Player player, float minDistance);
void WaveSpawner(float dt);
//  eklenecek düşmanların hareketini ve çizimini farklı yerlerde hesaplayıp optimize etmek için



//  programın başladığı yer
int main(void){
    //  işletim sisteminden grafik belleği isteme
    InitWindow(screenWidth, screenHeight, "Phase 8: The Game Flow (Waves & States)");

    //  fps limitleme
    SetTargetFPS(144);

    ResetGame(enemies);

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
            if(IsKeyDown(KEY_W)) player.pos.y -= player.speed * dt;

            // aşağı
            if(IsKeyDown(KEY_S)) player.pos.y += player.speed * dt;

            // sağa
            if(IsKeyDown(KEY_D)) player.pos.x += player.speed * dt;

            //  sola
            if(IsKeyDown(KEY_A)) player.pos.x -= player.speed * dt;

            //  ekranın solundan dışarıya çıkamaması için
            if(player.pos.x < 20) player.pos.x = 20;

            //  sağından çıkamaması için
            if(player.pos.x > 780) player.pos.x = 780;

            //  yukardan çıkmaması için
            if(player.pos.y < 20) player.pos.y = 20;

            //  aşağıdan çıkmaması için
            if(player.pos.y > 580) player.pos.y = 580;
            //  hareket mantığı ve oyun içi
            //  IsKeyDown: tuşa basılı tutulduğu sürece true döner
}



//  oyuncunun içinde düşman doğmama mekaniği
Vector2 GetSafeSpawnPosition (Player player, float minDistance){
    Vector2 spawnPos;
    float distance = 0.0f;

    do{
        spawnPos.x = (float)GetRandomValue(0, screenWidth);
        spawnPos.y = (float)GetRandomValue(0, screenHeight);

        float dx = player.pos.x - spawnPos.x;
        float dy = player.pos.y - spawnPos.y;
        distance = sqrt(dx * dx + dy * dy);

        
    } 
    while(distance < minDistance);
    return spawnPos;
}




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
                ResetGame(enemies);

                currentState = GAMEPLAY;
            }
               

}


    //  mermi ve düşman çarpışma güncelleyicisi
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




void CheckCollisions(){
                    //  düşman çarpışma kontrolü
            for (int i = 0; i < max_enemies; i++){
                if(!enemies[i].active) continue;
                if (CheckCollisionCircles(player.pos, player.radius, enemies[i].pos, enemyRadius)){
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
                float dx = player.pos.x - enemies[i].pos.x;

                //  düşman ve oyuncu arasındaki x mesafesi
                float dy = player.pos.y - enemies[i].pos.y;

                //  düşman ve oyuncu arasındaki mesafeyi bulma
                float enemyDistance = sqrt(dx * dx + dy * dy);

                /*  hareket normalizasyonu eğer mesafe sıfır ise yani üst üstelerse
                işlem yaptırmayacağız yoksa 0'a bölme hatası alırız */
                if(enemyDistance > 0){
                    float enemySpeed = enemies[i].speed; // düşman hızı
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
                    float dx = enemies[i].pos.x - player.pos.x;
                    float dy = enemies[i].pos.y - player.pos.y;
                    float dist = sqrt(dx * dx + dy * dy);
                
                    if(dist < minDistance){
                        minDistance = dist;
                        nearestEnemyIndex = i;
                    }
                }
                //  eğer düşman bulduysan ateşle
                if(nearestEnemyIndex != -1){
                    FireBullet(bullets, player.pos, enemies[nearestEnemyIndex].pos);
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
            DrawCircleV(player.pos, player.radius, BLUE);

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


void ResetGame(enemy enemies[]){
    //  oyuncuyu merkeze koy
    player.pos = (Vector2){(float) screenWidth / 2, (float) screenHeight / 2};
    player.speed = 300.0f;
    player.radius = 20.0f;

    //  tüm düşmanları silme
    for (int i = 0; i < max_enemies; i++){
        enemies[i].active = false;
    }

    //  tüm mermileri sil
    for (int i = 0; i < max_bullets; i++){
        bullets[i].active = false;
    }

    //  zamanlayıcı sıfırlama
    waveTimer = 0;
    
    shootTimer = 0;

    //  başlangıç zorluğuna dön
    spawnInterval = 1.5f;
    
}


void SpawnEnemy(enemy enemies[]){
    for (int i = 0; i < max_enemies; i++){
        if (!enemies[i].active){
            enemies[i].active = true;

            enemies[i].pos = GetSafeSpawnPosition(player, 300.0f);
            enemies[i].speed = (float)GetRandomValue(100, 200);
            enemies[i].color = RED;
            break;
        }
        
    }
    
}

void WaveSpawner(float dt){
        //  wave mantığı ekleme
        //  zamanı ilerletme
        waveTimer += dt;
        if (waveTimer >= spawnInterval){

            //  düşmanları çağırma
            SpawnEnemy(enemies);

            //  zamanlayıcıyı tekrar sıfırlama
            waveTimer = 0;

            //  zaman ilerledikçe zorlaşması için her waveden sonra diğer wave için gereken süreyi kısaltma
            spawnInterval *= 0.99f;
            
        }
}


void updateGame(float dt){


    switch(currentState){



            case MENU:

            UpdateMenu();

            break;

            case GAMEPLAY:
            
            UpdatePlayerMovement(dt);

            WaveSpawner(dt);

            UpdateEnemyAI(dt);

            CheckCollisions();

            UpdateBullets(dt);

            UpdateShootingSystem(dt);
            
            break;


            case GAME_OVER:
            if(IsKeyPressed(KEY_ENTER)){
                ResetGame(enemies);
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

    
       