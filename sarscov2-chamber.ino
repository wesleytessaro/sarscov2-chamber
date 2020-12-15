/*______Import Libraries_______ */
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

/*ID de tela
  idTela=1 - Tela Bem vindo
  idTela=2 - Tela escolha tempo
  idTela=3 - Luz ligada
*/
int idTela = 1;

/*Coordenadas tela*/
int X, Y;



/*______Define LCD pins (I have asigned the default values)_______*/
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
/*_______End of defanitions______*/

/*______Assign names to colors and pressure_______*/
#define WHITE   0x0000 //Black->White
#define YELLOW    0x001F //Blue->Yellow
#define CYAN     0xF800 //Red->Cyan
#define PINK   0x07E0 //Green-> Pink
#define RED    0x07FF //Cyan -> Red
#define GREEN 0xF81F //Pink -> Green 
#define BLUE  0xFFE0 //Yellow->Blue
#define BLACK   0xFFFF //White-> Black
#define MINPRESSURE 10
#define MAXPRESSURE 1000
/*_______Assigned______*/

/*____Calibrate TFT LCD_____*/
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

/*______End of Calibration______*/
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 is the sensitivity
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Start communication with LCD

//Luz ligada
unsigned long millisAnterior;
unsigned long millisAtual;
int tempoLuzLigada;
#define PORTALED 22

#define PORTABUZZER 52
#define PORTALEDVERDE 49
void setup()
{
  pinMode(PORTALED, OUTPUT);
  pinMode(PORTABUZZER, OUTPUT);
  pinMode(PORTALEDVERDE, OUTPUT);
  digitalWrite(PORTALED, LOW);
  digitalWrite(PORTALEDVERDE, LOW);
  delay(2000);
  tft.reset();          //Always reset at start
  tft.begin(0x9341);    // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(1);   // I just roated so that the power jack faces up - optional
  tft.fillScreen(WHITE);
  WelcomeScreen();
  Serial.begin(9600);

}


void loop() {

  //Para detectar os botoes
  TSPoint p = waitTouch();
  X = p.x; Y = p.y;
  DetectButtons();
  delay(10);
}
void WelcomeScreen()
{
  idTela = 1;
  tft.fillScreen(WHITE);
  tft.setCursor (0, 30);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Iniciando sistema ...");
  tft.fillRect(0, 10, 50, 20, BLACK);
  delay(1000);
  tft.fillRect(0, 10, 100, 20, BLACK);
  delay(1000);
  tft.fillRect(0, 10, 280, 20, BLACK);
  delay(1000);
  tft.setCursor (0, 80);
  tft.setTextSize (5);
  tft.setTextColor(BLUE);
  tft.println("BEM VINDO!");
  tft.setTextSize (2);
  tft.println("Camara Anti Sars-Cov2");
  delay(2000);
  tela2();
}
void tela2()
{
  idTela = 2;
  //Criar Quadrados
  tft.fillRect(0, 0, 320, 80, BLUE);
  tft.fillRect(0, 80, 320, 80, PINK);
  tft.fillRect(0, 160, 320, 80, RED);

  //Escrever nos quadrados
  tft.setCursor (80, 40);
  tft.setTextSize (2);
  tft.setTextColor(WHITE);
  tft.println("30 SEGUNDOS");

  tft.setCursor (80, 120);
  tft.setTextSize (2);
  tft.setTextColor(WHITE);
  tft.println("60 SEGUNDOS");

  tft.setCursor (80, 200);
  tft.setTextSize (2);
  tft.setTextColor(WHITE);
  tft.println("90 SEGUNDOS");
}

TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (idTela == 3) { 
      telaLuzLigada();
    }

    
  }
  while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);;

  
  return p;
}

void DetectButtons()
{
  Serial.println(X);
  Serial.println(Y);


  if (idTela == 2) { //Se tiver na tela de escolha de tempo
    if (Y >= 160) {//Serial.println("90 segundos");
      tempoLuzLigada=90;
    }
    else if (Y >= 80) {//Serial.println("60 segundos");
      tempoLuzLigada=60;
    }
    else {//Serial.println("30 segundos");
      tempoLuzLigada=30;
    }
    DRAWtela3();
    ligarLuz();
    
  }

  else if (idTela == 3) { //Se tiver na tela Luz ligada
    if (Y > 160) { //Se clicar no cancelar
      desligarLuz();
      tela2();
    }

  }


}

void DRAWtela3(){
  idTela = 3;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 60, BLUE);
  tft.setCursor (30, 20);
  tft.setTextSize (3);
  tft.println("Tempo Restante");


  tft.fillRect(0,200, 320, 40, CYAN);
  tft.setCursor (80, 210);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.println("FINALIZAR");
  millisAnterior=millis();
  telaLuzLigada();
}

void telaLuzLigada(){
  
  
  millisAtual=millis();
  if((millisAtual-millisAnterior)>1000){
    millisAnterior=millisAtual;
    tempoLuzLigada--;  
    tft.fillRect(0, 60, 320, 140, BLACK);
    tft.setCursor (80, 95);
    tft.setTextSize (13);
    tft.setTextColor(WHITE);
    tft.println(tempoLuzLigada);
    if(tempoLuzLigada==0){
      desligarLuz();
      tela2();
      //acionar buzzer
    }
   }
  
  
  
 

}

void desligarLuz (){
  digitalWrite(PORTALED, LOW);
  acionarBuzzer();
}
void ligarLuz (){
digitalWrite(PORTALED, HIGH);
}

void acionarBuzzer(){
digitalWrite(PORTALEDVERDE,HIGH);
tone(PORTABUZZER, 2000);
delay(1000);
noTone(PORTABUZZER);
delay(200);
tone(PORTABUZZER, 2000);
delay(1000);
noTone(PORTABUZZER);
digitalWrite(PORTALEDVERDE,LOW);
}
