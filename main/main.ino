#include <SPI.h>         // Pour communiquer via le bus SPI avec la matrice
#include <Arduino.h>
#include <LiquidCrystal.h> // Pour gérer l'affichage sur LCD

// --- Instances d'écrans LCD (pins RS, E, D4, D5, D6, D7) ---
LiquidCrystal lcd1(22, 23, 24, 25, 26, 27);
LiquidCrystal lcd2(28, 29, 30, 31, 32, 33);
LiquidCrystal lcd3(34, 35, 36, 37, 38, 39);

// --- Brochage et constantes ---
#define PIN_CE         53      // Sortie "Chip Enable" pour la matrice via SPI
#define BUZZER_PIN     5       // Buzzer pour feedback sonore
#define RED_LED_PIN    10      // LED rouge (erreur)
#define GREEN_LED_PIN  9       // LED verte (bonne réponse)
const int BLUE_LED_PINS[] = {8, 7, 6}; // LEDs bleues (indicateur de vies)
const int BUTTON_PINS[]   = {13,12,11}; // 3 boutons pour la sélection de réponse

// Fréquences pour signal sonore
#define WIN_FREQUENCY  1000
#define LOSE_FREQUENCY 400

// Couleurs pour la matrice (3 bits : R/G/B)
#define NO_COLOR 0b000
#define RED      0b100
#define GREEN    0b010
#define BLUE     0b001
#define CYAN     0b011

// Modes de jeu
#define MODE_NORMAL       0
#define MODE_TIME_LIMIT   1
#define MODE_SUDDEN_DEATH 2
#define TIME_LIMIT_MS     4000  // 4 secondes max pour le mode Time Limit

// --- Tableaux d'images affichables sur la matrice (codés en R/G/B) ---
const uint8_t images[][8][8] = {
    // Chaque image est un tableau 8x8, chaque case indique la couleur

    // Grenouille
  {
    { NO_COLOR, GREEN, GREEN, GREEN, NO_COLOR, GREEN, GREEN, GREEN },
    { NO_COLOR, GREEN, CYAN, GREEN, NO_COLOR, GREEN, CYAN, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, RED, RED, RED, RED, RED, RED, RED },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN },
    { GREEN, GREEN, NO_COLOR, GREEN, GREEN, GREEN, NO_COLOR, GREEN }
  },
      // Oiseau
  {
    { NO_COLOR, NO_COLOR, CYAN, CYAN, NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, CYAN, CYAN, CYAN, CYAN, NO_COLOR, NO_COLOR, NO_COLOR },
    { RED, CYAN, BLUE, CYAN, CYAN, NO_COLOR, NO_COLOR, GREEN },
    { NO_COLOR, CYAN, CYAN, CYAN, GREEN, GREEN, GREEN, GREEN },
    { NO_COLOR, CYAN, CYAN, CYAN, GREEN, GREEN, GREEN, GREEN },
    { NO_COLOR, NO_COLOR, CYAN, CYAN, CYAN, CYAN, CYAN, NO_COLOR },
    { NO_COLOR, NO_COLOR, CYAN, CYAN, CYAN, CYAN, CYAN, NO_COLOR },
    { NO_COLOR, NO_COLOR, RED, NO_COLOR, RED, NO_COLOR, NO_COLOR, NO_COLOR }
  },
      // Arbre
  {
    { NO_COLOR, NO_COLOR, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR },
    { NO_COLOR, NO_COLOR, GREEN, GREEN, GREEN, GREEN, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR }
  },
      // Coeur
  {
    { NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { RED, RED, RED, RED, RED, RED, RED, RED },
    { NO_COLOR, RED, RED, RED, RED, RED, RED, NO_COLOR },
    { NO_COLOR, NO_COLOR, RED, RED, RED, RED, NO_COLOR, NO_COLOR },
    { NO_COLOR, NO_COLOR, NO_COLOR, RED, RED, NO_COLOR, NO_COLOR, NO_COLOR }
  },
  // Serpent
  {
    { NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,GREEN },
    { NO_COLOR,GREEN,  GREEN,  GREEN,  GREEN,  GREEN,  GREEN,  GREEN },
    { NO_COLOR,GREEN,  NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR },
    { NO_COLOR,GREEN,  GREEN,  GREEN,  GREEN,  GREEN,  NO_COLOR,NO_COLOR },
    { NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,GREEN,  NO_COLOR,NO_COLOR },
    { CYAN,  GREEN, CYAN,  GREEN,  GREEN,  GREEN,  NO_COLOR,NO_COLOR },
    { GREEN,  GREEN,  GREEN,  NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR },
    { RED,    NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR,NO_COLOR }
  },
  // Tête de mort
  {
    { RED,RED,RED,RED,RED,RED,RED,RED },
    { RED,RED,RED,RED,RED,RED,RED,RED },
    { RED,RED,RED,RED,RED,RED,RED,RED },
    { RED,NO_COLOR,RED,RED,RED,NO_COLOR,RED,RED },
    { RED,RED,RED,NO_COLOR,RED,RED,RED,RED },
    { RED,RED,RED,RED,RED,RED,RED,RED },
    { NO_COLOR,RED,NO_COLOR,RED,NO_COLOR,RED,NO_COLOR,NO_COLOR },
    { NO_COLOR,RED,NO_COLOR,RED,NO_COLOR,RED,NO_COLOR,NO_COLOR }
  },
  // Dinosaure
  {
    { NO_COLOR,GREEN, GREEN, GREEN, GREEN, GREEN, NO_COLOR,NO_COLOR },
    { GREEN,   NO_COLOR,GREEN, NO_COLOR,GREEN, NO_COLOR,NO_COLOR,NO_COLOR },
    { GREEN,   GREEN,  GREEN, GREEN, GREEN, NO_COLOR,NO_COLOR,NO_COLOR },
    { NO_COLOR,NO_COLOR,GREEN, GREEN, GREEN, NO_COLOR,NO_COLOR,NO_COLOR },
    { NO_COLOR,GREEN,  CYAN,  GREEN,GREEN, NO_COLOR,GREEN, NO_COLOR },
    { NO_COLOR,NO_COLOR,CYAN, CYAN,GREEN, NO_COLOR,GREEN, NO_COLOR },
    { NO_COLOR,NO_COLOR,CYAN, CYAN,GREEN, GREEN, NO_COLOR,NO_COLOR },
    { NO_COLOR,NO_COLOR,GREEN, NO_COLOR,GREEN,NO_COLOR,NO_COLOR,NO_COLOR }
  },
  // Cadenas
  {
    { NO_COLOR,NO_COLOR,BLUE,  BLUE, BLUE, NO_COLOR,NO_COLOR,NO_COLOR },
    { NO_COLOR,BLUE,    NO_COLOR,NO_COLOR,NO_COLOR,BLUE, NO_COLOR,NO_COLOR },
    { NO_COLOR,BLUE,    NO_COLOR,NO_COLOR,NO_COLOR,BLUE, NO_COLOR,NO_COLOR },
    { BLUE,    BLUE,    BLUE,   BLUE, BLUE, BLUE,  BLUE,   NO_COLOR },
    { BLUE,    BLUE,    BLUE,   NO_COLOR,BLUE,BLUE,BLUE,   NO_COLOR },
    { BLUE,    BLUE,    BLUE,   NO_COLOR,BLUE,BLUE,BLUE,   NO_COLOR },
    { BLUE,    BLUE,    BLUE,   BLUE, BLUE, BLUE,  BLUE,   NO_COLOR },
    { NO_COLOR,NO_COLOR,BLUE,   BLUE, BLUE, NO_COLOR,NO_COLOR,NO_COLOR }
  }
};

// On associe chaque image à une réponse textuelle
static const int NB_IMAGES = sizeof(images) / sizeof(images[0]);
const char* answers[] = {
  "Grenouille","Oiseau","Arbre","Coeur","Serpent",
  "Tete de mort","Dinosaure","Cadenas"
};

// Variables de jeu
int imageOrder[NB_IMAGES], questionIndex, currentImage=-1, correctButton;
String options[3];
int lives=3, gameMode=0;
bool timeLimitActive=false;
unsigned long lastPressTime=0, gameStartTime=0;
long bestScore=0;

// --- Fonctions déclarées plus bas ---
void selectGameMode();
void resetGame();
void shuffleImages();
void nextQuestion();
void generateQuestion();
void displayMatrix();
void displayRow(uint8_t row);
void checkButtonPress();
long computeScore(unsigned long totalTimeMs,int finalLives);
void showFullColor(uint8_t color, unsigned long ms);
void playVictoryTune();
void playGameOverTune();

// --- Initialisation des pins et setup des LCD ---
void setup(){
  pinMode(PIN_CE, OUTPUT);
  SPI.begin(); // Initialisation du bus SPI pour la matrice

  // Boutons en INPUT_PULLUP : ils seront à l'état LOW quand pressés
  for(int i=0;i<3;i++)
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);

  // LEDs en sortie (rouge, verte, bleues = vies)
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  for(int i=0;i<3;i++) 
    pinMode(BLUE_LED_PINS[i], OUTPUT);

  // Extinction par défaut (LEDs en HIGH car câblage inversé)
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);
  for(int i=0;i<3;i++)
    digitalWrite(BLUE_LED_PINS[i], HIGH);

  // Initialisation des LCD 16×2
  lcd1.begin(16,2);
  lcd2.begin(16,2);
  lcd3.begin(16,2);

  // On demande à l'utilisateur de choisir le mode de jeu
  selectGameMode();
}

void loop(){
  // Affichage en continu de la matrice (pour éviter flicker)
  displayMatrix();
  // Contrôle si un bouton est pressé
  checkButtonPress();
}

// --- Choix du mode de jeu via les boutons ---
void selectGameMode(){
  // Affiche un titre sur chaque LCD
  lcd1.clear(); lcd1.print("Mode Normal");    lcd1.setCursor(0,1); lcd1.print("(Btn1)");
  lcd2.clear(); lcd2.print("Time Limit");     lcd2.setCursor(0,1); lcd2.print("(Btn2)");
  lcd3.clear(); lcd3.print("Mort subite");    lcd3.setCursor(0,1); lcd3.print("(Btn3)");

  // Boucle jusqu'au choix
  while(true){
    for(int i=0;i<3;i++){
      if(!digitalRead(BUTTON_PINS[i])){ // Pression bouton
        delay(200); // Anti-rebond simple
        if(i==0){ 
          gameMode=MODE_NORMAL;      
          lcd1.clear(); lcd1.print("Normal OK!");
        }
        if(i==1){ 
          gameMode=MODE_TIME_LIMIT;  
          lcd2.clear(); lcd2.print("TimeLim OK!");
        }
        if(i==2){ 
          gameMode=MODE_SUDDEN_DEATH;
          lcd3.clear(); lcd3.print("MortSub OK!");
        }
        delay(500);
        resetGame(); 
        return;
      }
    }
  }
}

// --- Réinitialise les paramètres pour une nouvelle partie ---
void resetGame(){
  questionIndex = 0;
  currentImage  = -1;
  correctButton = 0;
  lastPressTime = 0;
  timeLimitActive = false;

  // Vies selon le mode
  if(gameMode == MODE_SUDDEN_DEATH) 
    lives = 1;
  else 
    lives = 3;

  // Affiche les vies (LEDs bleues) ; si i < lives → LED ON (LOW)
  for(int i=0;i<3;i++)
    digitalWrite(BLUE_LED_PINS[i], (i<lives) ? LOW : HIGH);

  // En mode normal, on démarre un chrono pour le score final
  if(gameMode == MODE_NORMAL)
    gameStartTime = millis();

  // Mélange l'ordre des images et démarre la première question
  shuffleImages();
  nextQuestion();
}

// --- Mélange le tableau d'images de façon aléatoire ---
void shuffleImages(){
  for(int i=0; i<NB_IMAGES; i++)
    imageOrder[i] = i;
  for(int i=0; i<NB_IMAGES; i++){
    int j = random(NB_IMAGES);
    int tmp = imageOrder[i];
    imageOrder[i] = imageOrder[j];
    imageOrder[j] = tmp;
  }
}

// --- Passe à la question suivante ---
void nextQuestion(){
  if(questionIndex >= NB_IMAGES){
    // Plus d'images = victoire
    lcd1.clear(); lcd1.print("BRAVO!");
    lcd2.clear(); lcd2.print("BRAVO!");
    lcd3.clear(); lcd3.print("BRAVO!");
    showFullColor(GREEN, 1000); // Écran vert
    playVictoryTune();

    // Si mode normal, calcul du score final
    if(gameMode == MODE_NORMAL){
      unsigned long totalTime = millis() - gameStartTime;
      long s = computeScore(totalTime, lives);
      lcd1.setCursor(0,1); lcd1.print("Score:"); lcd1.print(s);
      // On vérifie si c'est un record
      if(s > bestScore){
        bestScore = s;
        lcd2.setCursor(0,1); lcd2.print("Nouveau record!");
      }
    }
    delay(3000);
    selectGameMode(); // Retour au choix de mode
    return;
  }
  // Sélectionne la prochaine image
  currentImage = imageOrder[questionIndex++];
  generateQuestion();

  // En mode Time Limit ou Mort subite, on active le délai
  timeLimitActive = (gameMode == MODE_TIME_LIMIT || gameMode == MODE_SUDDEN_DEATH);
  lastPressTime   = millis();
}

// --- Prépare l'affichage des réponses possibles, dont une correcte ---
void generateQuestion(){
  correctButton = random(0,3);
  int idx = currentImage;
  options[correctButton] = answers[idx];

  int totalAnswers = NB_IMAGES;
  for(int i=0;i<3;i++){
    if(i != correctButton){
      int w;
      // On cherche une réponse incorrecte, pas déjà utilisée
      do { w = random(totalAnswers); }
      while(w == idx || options[0] == answers[w] || options[1] == answers[w]);
      options[i] = answers[w];
    }
  }

  // Affiche le texte de chaque option sur un LCD
  lcd1.clear(); lcd1.print(options[0].substring(0,16));
  lcd2.clear(); lcd2.print(options[1].substring(0,16));
  lcd3.clear(); lcd3.print(options[2].substring(0,16));
}

// --- Gère l'affichage de la matrice (rafraîchissement ligne par ligne) ---
void displayMatrix(){
  if(currentImage < 0) return; // Pas d'image si pas de question
  for(uint8_t r=0; r<8; r++){
    displayRow(r);
    delayMicroseconds(200); 
  }
}

// --- Affiche une ligne de la matrice (sélectionne R/G/B) ---
void displayRow(uint8_t row){
  uint8_t rm=0, gm=0, bm=0; // masques pour rouge, vert, bleu
  for(uint8_t c=0; c<8; c++){
    uint8_t col = images[currentImage][row][c];
    if(col & RED)   rm |= (1 << c);
    if(col & GREEN) gm |= (1 << c);
    if(col & BLUE)  bm |= (1 << c);
  }
  // Transmission en SPI : on envoie la "ligne" pour R, B, G
  digitalWrite(PIN_CE, LOW);
  SPI.transfer(~rm);
  SPI.transfer(~bm);
  SPI.transfer(~gm);
  SPI.transfer(1 << row);
  digitalWrite(PIN_CE, HIGH);
}

// --- Vérifie si un bouton de réponse est pressé, gère la logique de victoire / erreur ---
void checkButtonPress(){
  // Gère la limite de temps si mode Time Limit / Mort subite
  if(timeLimitActive && (millis() - lastPressTime > TIME_LIMIT_MS)){
    // Temps écoulé = erreur
    digitalWrite(RED_LED_PIN, LOW);
    showFullColor(RED, 500); // Écran rouge
    tone(BUZZER_PIN, LOSE_FREQUENCY); 
    delay(500);
    digitalWrite(RED_LED_PIN, HIGH);
    noTone(BUZZER_PIN);

    // En mort subite : fin de partie directe
    if(gameMode == MODE_SUDDEN_DEATH){
      lcd1.clear(); lcd1.print("GAME OVER");
      lcd2.clear(); lcd2.print("GAME OVER");
      lcd3.clear(); lcd3.print("GAME OVER");
      showFullColor(RED,1000);
      playGameOverTune();
      delay(2000);
      selectGameMode();
      return;
    } else {
      // Sinon, on perd une vie
      if(lives>0) digitalWrite(BLUE_LED_PINS[--lives], HIGH);
      if(lives<=0){
        // Plus de vies = game over
        lcd1.clear(); lcd1.print("GAME OVER");
        lcd2.clear(); lcd2.print("GAME OVER");
        lcd3.clear(); lcd3.print("GAME OVER");
        showFullColor(RED, 1000);
        playGameOverTune();
        delay(2000);
        selectGameMode();
        return;
      }
    }
    // On passe à la prochaine question
    nextQuestion();
    return;
  }

  // Si un bouton est pressé...
  for(int i=0;i<3;i++){
    if(!digitalRead(BUTTON_PINS[i])){ // LOW = press
      delay(200); // Anti-rebond
      if(i == correctButton){
        // Bonne réponse
        digitalWrite(GREEN_LED_PIN, LOW);
        showFullColor(GREEN, 500);
        tone(BUZZER_PIN, WIN_FREQUENCY);
        delay(500);
        digitalWrite(GREEN_LED_PIN, HIGH);
        noTone(BUZZER_PIN);

        // Si c'était la dernière question
        if(questionIndex >= NB_IMAGES){
          lcd1.clear(); lcd1.print("VICTOIRE!");
          showFullColor(GREEN, 800);
          playVictoryTune();
          if(gameMode == MODE_NORMAL){
            long s = computeScore(millis()-gameStartTime, lives);
            lcd2.clear(); lcd2.print("Score:"); lcd2.print(s);
            // Vérifie record
            if(s>bestScore){
              bestScore=s;
              lcd3.clear(); lcd3.print("Nouveau record");
            }
          }
          delay(2000);
          selectGameMode();
          return;
        } else {
          nextQuestion();
        }
      } else {
        // Mauvaise réponse
        digitalWrite(RED_LED_PIN, LOW);
        showFullColor(RED, 500);
        tone(BUZZER_PIN, LOSE_FREQUENCY);
        delay(500);
        digitalWrite(RED_LED_PIN, HIGH);
        noTone(BUZZER_PIN);

        if(gameMode == MODE_SUDDEN_DEATH){
          // En mort subite, erreur = fin du jeu
          lcd1.clear(); lcd1.print("GAME OVER");
          lcd2.clear(); lcd2.print("GAME OVER");
          lcd3.clear(); lcd3.print("GAME OVER");
          showFullColor(RED, 1000);
          playGameOverTune();
          delay(2000);
          selectGameMode();
          return;
        } else {
          // Sinon, on perd une vie
          if(lives > 0) digitalWrite(BLUE_LED_PINS[--lives], HIGH);
          if(lives <= 0){
            // Plus de vies = game over
            lcd1.clear(); lcd1.print("GAME OVER");
            lcd2.clear(); lcd2.print("GAME OVER");
            lcd3.clear(); lcd3.print("GAME OVER");
            showFullColor(RED, 1000);
            playGameOverTune();
            delay(2000);
            selectGameMode();
            return;
          }
          // Passe à la question suivante
          nextQuestion();
        }
      }
      return;
    }
  }
}

// --- Calcul simple de score : dépend du temps et du nombre de vies restantes ---
long computeScore(unsigned long t, int lf){
  // On peut modifier la formule pour ajuster la difficulté
  long s = (lf * 10000L) - (long)(t / 100);
  return (s < 0) ? 0 : s;
}

// --- Affiche une couleur plein écran sur la matrice pendant 'ms' millisecondes ---
void showFullColor(uint8_t color, unsigned long ms){
  unsigned long t = millis();
  while(millis() - t < ms){
    for(uint8_t row=0; row<8; row++){
      uint8_t rm=0, gm=0, bm=0;
      // Sur chaque ligne, on active tous les pixels de la couleur choisie
      for(uint8_t col=0; col<8; col++){
        if(color & RED)   rm |= (1 << col);
        if(color & GREEN) gm |= (1 << col);
        if(color & BLUE)  bm |= (1 << col);
      }
      digitalWrite(PIN_CE, LOW);
      SPI.transfer(~rm);
      SPI.transfer(~bm);
      SPI.transfer(~gm);
      SPI.transfer(1 << row);
      digitalWrite(PIN_CE, HIGH);
      delayMicroseconds(200);
    }
  }
}

// --- Jouer un "jingle" de victoire ---
void playVictoryTune(){
  int melody[] = { 262, 294, 330, 392 }; // Do, Ré, Mi, Sol
  for(int i=0; i<4; i++){
    tone(BUZZER_PIN, melody[i], 200);
    delay(250);
  }
  noTone(BUZZER_PIN);
}

// --- Jouer un "jingle" de game over ---
void playGameOverTune(){
  int melody[] = { 392, 330, 294, 262 }; // Sol, Mi, Ré, Do (descendant)
  for(int i=0; i<4; i++){
    tone(BUZZER_PIN, melody[i], 200);
    delay(250);
  }
  tone(BUZZER_PIN, 200, 400); // Note plus grave, plus longue
  delay(450);
  noTone(BUZZER_PIN);
}
