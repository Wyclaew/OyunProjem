#include "raylib.h"

//  enum kendi değişken türümüzü yaratmamızı sağlıyor
typedef enum {
    menu;       //  giriş ekranı
    gameplay;       //  oyun 
    game_over;      //  lose ekranı
} GameState;

const int screenWidth = 800;    //  ekran genişlik
const int screenHeight = 600;   //  ekran yükseklik
GameState currentState = menu;  //  oyuun başlatınca ilk hangi ekran gelsin

//  kullanacağımız fonksiyonları önceden tanıtma
void updateGame(void);  //  matematiksel şeylerin döneceği yer
void drawGame(void);    //  çizim işlerinin dönceği yer
//  eklenecek düşmanların hareketini ve çizimini farklı yerlerde hesaplayıp optimize etmek için


//  programın başladığı yer
int main(void){
    //  işletim sisteminden grafik belleği isteme
    InitWindow(screenWidth, screenHeight, "Circle Survivor - v0.1");

    //  fps limitleme
    SetTargetFPS(60);

    //  oyun döngüsü
    while(!WindowShouldCLose()){
        //  ilk adım hesaplama klavye girdileri, hareketler, canlar vs.
        updateGame();

        //  ikinci adım ekrana oyunun son halini çizdirme
        drawGame();

    }
    //  aldığımız belliği geri verme
    CloseWindow();

    return 0;

}