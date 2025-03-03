#include <SPI.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

//  Pins liaison 3 Displays
LiquidCrystal lcd1(22, 23, 24, 25, 26, 27);
LiquidCrystal lcd2(28, 29, 30, 31, 32, 33); 
LiquidCrystal lcd3(34, 35, 36, 37, 38, 39);

// Constantes matérielles (LED, Buzzer, Boutons)
const int PIN_CE = 53;
const int BUTTON_PINS[] = {13, 12, 11}; 
const int RED_LED_PIN    = 10;
const int GREEN_LED_PIN  = 9;
const int BLUE_LED_PINS[] = {8, 7, 6}; 
const int BUZZER_PIN     = 5;

// Couleurs et Fréquences
#define WIN_FREQUENCY   1000
#define LOSE_FREQUENCY  400

#define NO_COLOR 0b000
#define RED      0b100
#define GREEN    0b010
#define BLUE     0b001
#define CYAN     0b011
#define MAGENTA  0b101
#define YELLOW   0b110
#define WHITE    0b111

// Modes de jeu
#define MODE_NORMAL       0
#define MODE_TIME_LIMIT   1
#define MODE_SUDDEN_DEATH 2

// Limite de temps pour Time Limit / Sudden Death
#define TIME_LIMIT_MS   4000

// Images
const uint8_t images[][8][8] = {
  // 0) Grenouille
  {
    { NO_COLOR, GREEN, GREEN, GREEN, NO_COLOR, GREEN, GREEN, GREEN },
    { NO_COLOR, GREEN, 0b011, GREEN, NO_COLOR, GREEN, 0b011, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, RED, RED, RED, RED, RED, RED, RED },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, NO_COLOR, GREEN, GREEN, GREEN, NO_COLOR, GREEN } },
  // 1) Oiseau
  {
    { NO_COLOR, NO_COLOR, 0b011, 0b011, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, 0b011, 0b011, 0b011, 0b011, NO_COLOR, NO_COLOR, NO_COLOR },
    { RED, 0b011, BLUE, 0b011, 0b011, NO_COLOR, NO_COLOR, GREEN },
    { NO_COLOR, 0b011, 0b011, 0b011, GREEN, GREEN, GREEN, GREEN },
    { NO_COLOR, 0b011, 0b011, 0b011, GREEN, GREEN, GREEN, GREEN },
    { NO_COLOR, NO_COLOR, 0b011, 0b011, 0b011, 0b011, 0b011, NO_COLOR },
    { NO_COLOR, NO_COLOR, 0b011, 0b011, 0b011, 0b011, 0b011, NO_COLOR },
    { NO_COLOR, NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, NO_COLOR, NO_COLOR } },
  // 2) Arbre
  {
    { NO_COLOR, NO_COLOR, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, NO_COLOR, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR } },
  // 3) Coeur
  {
    { NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { NO_COLOR, RED, RED, RED, RED, RED, RED, NO_COLOR },
    { NO_COLOR, NO_COLOR, RED, RED, RED, RED, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR } },
  // 4) Serpent
  {
    { NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, GREEN },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { NO_COLOR, GREEN, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, GREEN, NO_COLOR, NO_COLOR },
    { 0b011, GREEN, 0b011, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
    { RED, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR } },
  // 5) Tête de mort
  {
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, NO_COLOR, RED, RED, RED, NO_COLOR, RED, RED },
    { RED, RED, RED, NO_COLOR, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, NO_COLOR },
    { NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, NO_COLOR } },
  // 6) Dinosaure
  {
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { GREEN, NO_COLOR, GREEN, NO_COLOR, GREEN, NO_COLOR, NO_COLOR, NO_COLOR },
    { GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, GREEN, CYAN, GREEN, GREEN, NO_COLOR, GREEN, NO_COLOR },
    { NO_COLOR, NO_COLOR, CYAN, CYAN, GREEN, NO_COLOR, GREEN, NO_COLOR },
    { NO_COLOR, NO_COLOR, CYAN, CYAN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, GREEN, NO_COLOR, GREEN, NO_COLOR, NO_COLOR, NO_COLOR } },
  // 7) Cadenas
  {
    { NO_COLOR, NO_COLOR, BLUE, BLUE, BLUE, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, BLUE, NO_COLOR, NO_COLOR, NO_COLOR, BLUE, NO_COLOR, NO_COLOR },
    { NO_COLOR, BLUE, NO_COLOR, NO_COLOR, NO_COLOR, BLUE, NO_COLOR, NO_COLOR },
    { BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, NO_COLOR },
    { BLUE, BLUE, BLUE, NO_COLOR, BLUE, BLUE, BLUE, NO_COLOR },
    { BLUE, BLUE, BLUE, NO_COLOR, BLUE, BLUE, BLUE, NO_COLOR },
    { BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, NO_COLOR },
    { NO_COLOR, NO_COLOR, BLUE, BLUE, BLUE, NO_COLOR, NO_COLOR, NO_COLOR } },
};

// Tableau de réponses (même ordre que 'images')
const char* answers[] = {
  "Grenouille", "Oiseau", "Arbre", "Coeur", "Serpent", "Tete de mort", "Dinosaure", "Cadenas"
};
static const int NB_IMAGES = sizeof(images) / sizeof(images[0]);

// Variables de Jeu
int imageOrder[NB_IMAGES];
int questionIndex  = 0;
int currentImage   = -1;
int correctButton  = 0;
String options[3];

int lives      = 3;
int gameMode   = MODE_NORMAL;

unsigned long lastPressTime = 0;
bool timeLimitActive        = false;

unsigned long gameStartTime = 0;
long bestScore              = 0;

// ---------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------
void selectGameMode();
void resetGame();
void shuffleImages();
void nextQuestion();
void generateQuestion();
void displayMatrix();
void displayRow(uint8_t row);
void checkButtonPress();
long computeScore(unsigned long totalTimeMs, int finalLives);

void setup() {

  pinMode(PIN_CE, OUTPUT);
  SPI.begin();

  for (int i = 0; i < 3; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(BLUE_LED_PINS[i], OUTPUT);
  }
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);

  for (int i = 0; i < 3; i++) {
    digitalWrite(BLUE_LED_PINS[i], HIGH);
  }

  lcd1.begin(16,2);
  lcd2.begin(16,2);
  lcd3.begin(16,2);

  selectGameMode();
}

void loop() {
  displayMatrix(); 
  checkButtonPress();
}

void selectGameMode() {
  // LCD1 propose "Mode Normal"
  lcd1.clear();
  lcd1.setCursor(0,0); 
  lcd1.print("Mode Normal"); 
  lcd1.setCursor(0,1); 
  lcd1.print("(Btn1)");

  // LCD2 propose "Time Limit"
  lcd2.clear();
  lcd2.setCursor(0,0); 
  lcd2.print("Time Limit");
  lcd2.setCursor(0,1); 
  lcd2.print("(Btn2)");

  // LCD3 propose "SuddenDeath"
  lcd3.clear();
  lcd3.setCursor(0,0); 
  lcd3.print("Mort subite");
  lcd3.setCursor(0,1); 
  lcd3.print("(Btn3)");

  // Attente du choix
  while(true){
    for(int i=0; i<3; i++){
      if(digitalRead(BUTTON_PINS[i]) == LOW){
        delay(200); // anti-rebond

        if(i==0) { gameMode=MODE_NORMAL;       lcd1.clear(); lcd1.print("Normal OK!"); }
        if(i==1) { gameMode=MODE_TIME_LIMIT;   lcd2.clear(); lcd2.print("TimeLim OK!");}
        if(i==2) { gameMode=MODE_SUDDEN_DEATH; lcd3.clear(); lcd3.print("MortSub OK!");}
        
        delay(500);
        resetGame();
        return;
      }
    }
  }
}

void resetGame() {
  questionIndex   = 0;
  currentImage    = -1;
  correctButton   = 0;
  lastPressTime   = 0;
  timeLimitActive = false;

  if(gameMode == MODE_SUDDEN_DEATH)  lives=1;
  else                               lives=3;

  for (int i = 0; i < 3; i++) {
    if (i < lives) digitalWrite(BLUE_LED_PINS[i], LOW);
    else           digitalWrite(BLUE_LED_PINS[i], HIGH);
  }

  if(gameMode==MODE_NORMAL){
    gameStartTime = millis();
  }

  shuffleImages();
  nextQuestion();
}

void shuffleImages() {
  for(int i=0; i<NB_IMAGES; i++){
    imageOrder[i]= i;
  }
  for(int i=0; i<NB_IMAGES; i++){
    int j=random(NB_IMAGES);
    int tmp=imageOrder[i];
    imageOrder[i]=imageOrder[j];
    imageOrder[j]=tmp;
  }
}

void nextQuestion() {
  if(questionIndex>=NB_IMAGES) {
    // Fini toutes les images => Victoire
    lcd1.clear(); lcd1.print("BRAVO!");
    lcd2.clear(); lcd2.print("BRAVO!");
    lcd3.clear(); lcd3.print("BRAVO!");

    if(gameMode==MODE_NORMAL) {
      unsigned long totalTime=millis()-gameStartTime;
      long finalScore=computeScore(totalTime,lives);

      lcd1.setCursor(0,1);
      lcd1.print("Score:");
      lcd1.print(finalScore);

      if(finalScore>bestScore) {
        bestScore=finalScore;
        lcd2.setCursor(0,1);
        lcd2.print("Nouveau record!");
      }
    }

    delay(3000);
    selectGameMode();
    return;
  }

  currentImage=imageOrder[questionIndex];
  questionIndex++;

  generateQuestion();

  timeLimitActive = (gameMode==MODE_TIME_LIMIT || gameMode==MODE_SUDDEN_DEATH);
  lastPressTime=millis();
}

void generateQuestion() {
  correctButton = random(0,3);
  int idxImage  = currentImage;
  options[correctButton]= answers[idxImage];

  int totalAnswers=sizeof(answers)/sizeof(answers[0]);
  for(int i=0; i<3; i++){
    if(i != correctButton){
      int wrongAnswer;
      do{
        wrongAnswer = random(totalAnswers);
      } while(
        wrongAnswer==idxImage 
        || options[0]==answers[wrongAnswer]
        || options[1]==answers[wrongAnswer]
      );
      options[i]= answers[wrongAnswer];
    }
  }

  // Afficher sur LCD1 => options[0]
  lcd1.clear();
  lcd1.print(options[0].substring(0,16)); // Tronqué à 16 caractères

  // Afficher sur LCD2 => options[1]
  lcd2.clear();
  lcd2.print(options[1].substring(0,16));

  // Afficher sur LCD3 => options[2]
  lcd3.clear();
  lcd3.print(options[2].substring(0,16));
}

void displayMatrix() {
  if(currentImage<0) return;
  for(uint8_t row=0; row<8; row++){
    displayRow(row);
    delayMicroseconds(200);
  }
}

void displayRow(uint8_t row) {
  uint8_t redMask=0, greenMask=0, blueMask=0;
  for(uint8_t col=0; col<8; col++){
    uint8_t color= images[currentImage][row][col];
    if(color&RED)   redMask   |= (1<<col);
    if(color&GREEN) greenMask |= (1<<col);
    if(color&BLUE)  blueMask  |= (1<<col);
  }

  digitalWrite(PIN_CE, LOW);
  SPI.transfer(~redMask);
  SPI.transfer(~blueMask);
  SPI.transfer(~greenMask);
  SPI.transfer(1<<row);
  digitalWrite(PIN_CE, HIGH);
}

void checkButtonPress() {
  if(timeLimitActive){
    unsigned long curTime=millis();
    if(curTime-lastPressTime>TIME_LIMIT_MS){
      // Temps écoulé => Mauvaise rep
      digitalWrite(RED_LED_PIN,LOW);
      tone(BUZZER_PIN,LOSE_FREQUENCY);
      delay(500);
      digitalWrite(RED_LED_PIN,HIGH);
      noTone(BUZZER_PIN);

      if(gameMode==MODE_SUDDEN_DEATH){
        // Game over direct
        lcd1.clear(); lcd1.print("GAME OVER");
        lcd2.clear(); lcd2.print("GAME OVER");
        lcd3.clear(); lcd3.print("GAME OVER");
        delay(2000);
        selectGameMode();
        return;
      }
      else {
        if(lives>0) {
          digitalWrite(BLUE_LED_PINS[--lives],HIGH);
        }
        if(lives<=0){
          lcd1.clear(); lcd1.print("GAME OVER");
          lcd2.clear(); lcd2.print("GAME OVER");
          lcd3.clear(); lcd3.print("GAME OVER");
          delay(2000);
          selectGameMode();
          return;
        }
      }
      nextQuestion();
      return;
    }
  }

  for(int i=0; i<3; i++){
    if(digitalRead(BUTTON_PINS[i])==LOW){
      delay(200);

      if(i== correctButton){
        digitalWrite(GREEN_LED_PIN,LOW);
        tone(BUZZER_PIN,WIN_FREQUENCY);
        delay(500);
        digitalWrite(GREEN_LED_PIN,HIGH);
        noTone(BUZZER_PIN);

        if(questionIndex>=NB_IMAGES){
          lcd1.clear(); lcd1.print("VICTOIRE!");
          // Score si normal
          if(gameMode==MODE_NORMAL){
            unsigned long totalTime=millis()-gameStartTime;
            long finalScore= computeScore(totalTime,lives);
            lcd2.clear(); 
            lcd2.print("Score:");
            lcd2.print(finalScore);
            if(finalScore>bestScore){
              bestScore=finalScore;
              lcd3.clear();
              lcd3.print("Nouveau record");
            }
          }
          delay(2000);
          selectGameMode();
          return;
        }
        else {
          nextQuestion();
        }
      }
      else {

        digitalWrite(RED_LED_PIN,LOW);
        tone(BUZZER_PIN,LOSE_FREQUENCY);
        delay(500);
        digitalWrite(RED_LED_PIN,HIGH);
        noTone(BUZZER_PIN);

        if(gameMode==MODE_SUDDEN_DEATH){

          lcd1.clear(); lcd1.print("GAME OVER");
          lcd2.clear(); lcd2.print("GAME OVER");
          lcd3.clear(); lcd3.print("GAME OVER");
          delay(2000);
          selectGameMode();
          return;
        }
        else {
          if(lives>0){
            digitalWrite(BLUE_LED_PINS[--lives],HIGH);
          }
          if(lives<=0){
            lcd1.clear(); lcd1.print("GAME OVER");
            lcd2.clear(); lcd2.print("GAME OVER");
            lcd3.clear(); lcd3.print("GAME OVER");
            delay(2000);
            selectGameMode();
            return;
          }
          nextQuestion();
        }
      }
      return;
    }
  }
}

long computeScore(unsigned long totalTimeMs, int finalLives){
  long score=(finalLives*10000L)-(long)(totalTimeMs/100);
  if(score<0) score=0;
  return score;
}
