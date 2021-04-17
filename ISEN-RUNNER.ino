// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 9, 8);


#define PIN_BUTTON 2 // Bouton pour l'interruption

/*
** BYTES D'ANIMATION
*/
#define ANIM_RUN1 1 // Byte de l'animation de course 1
#define ANIM_RUN2 2 // Byte de l'animation de course 2
#define ANIM_JUMP 3 // Byte de l'animation de saut 1
#define ANIM_JUMP_TOP '.' // Tête
#define ANIM_JUMP_BOTTOM 4 // Byte de l'animation de saut 2 (bas du corps puisque la tête est représentée par un point)

/*
** CONSTANTES EN RAPPORT AVEC LE PERSONNAGE
*/

#define MAN_POSITION 1 // Position du personnage (2ème colonne)

#define HERO_POSITION_RUN_BOTTOM_1 1 // Cours en bas (pose 1)
#define HERO_POSITION_RUN_BOTTOM_2 2 //              (pose 2)

#define HERO_POSITION_JUMP_1 3 // Position saut 1
#define HERO_POSITION_JUMP_2 4 // Position saut 2        

#define HERO_POSITION_RUN_TOP_1 11 // Cours en haut (pose 1)
#define HERO_POSITION_RUN_TOP_2 12 //               (pose 2)

/*
** MURS
*/

#define LEFT_WALL 14
#define FULL_WALL 15
#define RIGHT_WALL 13

/*
** Comme il est impossible de comparer le LCD à un byte par exemple, on crée 2 représentations :
**    - Le LCD qui permet l'affichage
**    - 2 tableaux (2 lignes de LCD) pour pouvoir faire les compraraisons nécéssaires
*/

static char terrainTOP[17];
static char terrainBOTTOM[17];

/*
** BOUTON D'INTERRUPTION
*/

static bool buttonInterrupted = false;

// Déclaration de variables globales
bool alive = false;
int timeDelay = 250;
int time = 500;
int tailleMur;
bool creation = false;
int pos;
int update;
static byte heroPos = HERO_POSITION_RUN_BOTTOM_1; // Position de départ du bonhomme
static int counterJump=0; // Conteur pour savoir si saut ou pas
static int counterTop=0; // Conteur pour savoir si saut ou pas
static int distance =0;


/*
** FONCTION D'AFFICHAGE DU SCORE QUI PREND EN PARAMÈTRES LE SCORE (CALCULÉ AVEC LA DISTANCE)
** ET LA LIGNE SUR LAQUELLE S'AFFICHER (SOIT 1 SOIT 2 SELON SI MODE ATTENTE OU JEU)
*/

void displayScore(int score, int ligne){
  // Gestion du nombre de caractères
  if (score<10){
    lcd.setCursor(11,ligne);
    lcd.print(score);
  } else if (score<100){
    lcd.setCursor(10,ligne);
    lcd.print(score);
  } else if (score<1000){
    lcd.setCursor(9,ligne);
    lcd.print(score);
  } else if (score<10000){
    lcd.setCursor(8,ligne);
    lcd.print(score);
  } else if (score<100000){
    lcd.setCursor(7,ligne);
    lcd.print(score);
  }
  lcd.setCursor(13,ligne);
  lcd.print("Pts");
}

/*
** Interruption du bouton pour les actions :
**    - Démarrer la partie (sortir du mode attente)
**    - Sauter en jeu 
*/
void buttonInterrupt() {
  buttonInterrupted = true;
}

void setup(){
  pinMode(PIN_BUTTON, INPUT); // Bouton pour sauter
  lcd.begin(16, 2); // Configuration du LCD
  attachInterrupt(0, buttonInterrupt, FALLING); // Interruption 
  randomSeed(analogRead(0)); // Initialisation du random

  initGraph(); // Initialiser les animations, bytes et tableaux
  
}

void loop(){ 
  if (!alive){
    attente(); // Mode attente si la partie n'a pas commencé ou si elle s'est terminée
  }
  else {
  if (buttonInterrupted) {
    if (heroPos <= HERO_POSITION_RUN_BOTTOM_2) heroPos = HERO_POSITION_JUMP_1; // Sauter si on appuie sur le bouton 
    buttonInterrupted = false;
  }  
  wall(); // Création d'un mur
  drawMan(heroPos, terrainTOP, terrainBOTTOM); // Choix de l'animation et affichage total 

  /*
  ** UPDATES EN FONCTION DES SITUATIONS
  ** (SAUT, OBSTACLE, etc...)
  */

  if((heroPos == HERO_POSITION_RUN_BOTTOM_1 || heroPos == HERO_POSITION_RUN_BOTTOM_2 || heroPos == HERO_POSITION_JUMP_1 ) && terrainBOTTOM[2]!=' '){
    alive = false;
    pos = 16;
    creation=false; // Gestion des obstacles
    for (int j = 0; j< 16; ++j) { // Réinitialiser quand on meurt
      terrainTOP[j] = ' ';
      terrainBOTTOM[j] = ' ';
    }
    lcd.clear();
    
  } else if (heroPos == HERO_POSITION_RUN_BOTTOM_1) { 
    heroPos = HERO_POSITION_RUN_BOTTOM_2;
  } else if(heroPos == HERO_POSITION_RUN_BOTTOM_2){
    heroPos = HERO_POSITION_RUN_BOTTOM_1;
  } else if(heroPos == HERO_POSITION_JUMP_1 && counterJump==0){
    heroPos = HERO_POSITION_JUMP_2;
    counterJump=1;
  }else if(heroPos == HERO_POSITION_JUMP_1 && counterJump==1){
    heroPos = HERO_POSITION_RUN_BOTTOM_1;
    counterJump=0;
  } else if(heroPos == HERO_POSITION_JUMP_2 && counterJump==1 && terrainBOTTOM[2]==' '){
    heroPos = HERO_POSITION_JUMP_1;
  } else if(heroPos == HERO_POSITION_JUMP_2 && counterJump==1 && terrainBOTTOM[2]!=' '){
    heroPos = HERO_POSITION_RUN_TOP_1;
  } else if(heroPos == HERO_POSITION_RUN_TOP_1 && terrainBOTTOM[1]!=' '){
    heroPos = HERO_POSITION_RUN_TOP_2;
  } else if(heroPos == HERO_POSITION_RUN_TOP_2 && terrainBOTTOM[1]!=' '){
    heroPos = HERO_POSITION_RUN_TOP_1;
  } else if((heroPos == HERO_POSITION_RUN_TOP_2 || heroPos == HERO_POSITION_RUN_TOP_1) && terrainBOTTOM[1]==' '){
    heroPos = HERO_POSITION_JUMP_2;
  }
  
  // Appel de la fonction pour afficher les scores
  displayScore(distance, 0);
  
  // Le personnage a avancé d'une case donc la distance augmente de 1
  ++distance;

  // Delai d'attente
  delay(timeDelay);
  if (timeDelay>100){
    timeDelay--; // Gestion de l'accélération au fur et à mesure du jeu 
  } 
  }
}

/* 
** FONCTION DE DÉTERMINATION DU MOUVEMENT
** ET D'AFFICHAGE GLOBAL
*/

void drawMan(byte position, char* terrainTOP, char* terrainBOTTOM) {
  char TOPSave = terrainTOP[MAN_POSITION]; // Sauvegarde de la position avant changement
  char BOTTOMSave = terrainBOTTOM[MAN_POSITION];
  byte TOP, BOTTOM;

  /*
  ** Changement des positions en fonction des situations
  ** Fonctionnement en deux tableaux (pour les 2 lignes du LCD)
  */

  switch (position) {
    case HERO_POSITION_RUN_BOTTOM_1:
      TOP = ' ';
      BOTTOM = ANIM_RUN1;
      break;
    case HERO_POSITION_RUN_BOTTOM_2:
      TOP = ' ';
      BOTTOM = ANIM_RUN2;
      break;
    case HERO_POSITION_JUMP_1:
      TOP = ' ';
      BOTTOM = ANIM_JUMP;
      break;
    case HERO_POSITION_JUMP_2:
      TOP = ANIM_JUMP_TOP;
      BOTTOM = ANIM_JUMP_BOTTOM;
      break;
    case HERO_POSITION_RUN_TOP_1:
      TOP = ANIM_RUN1;
      BOTTOM = ' ';
      break;
    case HERO_POSITION_RUN_TOP_2:
      TOP = ANIM_RUN2;
      BOTTOM = ' ';
      break;
  }

  // Actualisation des position dans le tableau 
  if (TOP != ' ') {
    terrainTOP[MAN_POSITION] = TOP;
  }
  if (BOTTOM != ' ') {
    terrainBOTTOM[MAN_POSITION] = BOTTOM;
  }
  
  // Affichage des deux tableaux sur le LCD
  lcd.setCursor(0,0);
  lcd.write(terrainTOP); 
  lcd.setCursor(0,1);
  lcd.write(terrainBOTTOM);
  
  // On réaffiche le personnage comme sauvegardé
  terrainTOP[MAN_POSITION] = TOPSave;
  terrainBOTTOM[MAN_POSITION] = BOTTOMSave;
}


/*
** MODE ATTENTE AVANT LE DÉBUT DE PARTIE 
** OU APRÈS QUE LA PARTIE SOIT PERDUE
*/

void attente(){
  int i = 0;
  while (!alive){ // Boucle tant que le bouton n'est pas préssé
    lcd.clear();
    // Différentes animations d'attente
    if (i%2==0){
      lcd.setCursor(1,0);
      lcd.write("PRESS TO START");  
    }
    if (i<5){
      lcd.setCursor(i-5,1);
      lcd.write("ISEN");
    }
    else{
      lcd.setCursor(0,1);
      lcd.write("ISEN");
    }
    // Affichage du score (0 si partie n'est pas commencée, score de la partie précédente si elle a été perdue)
    displayScore(distance, 1);  
    walk(i,time); // Animation de marche
    i++;
    if (i==9) i=0;
    alive = buttonInterrupted; // Gestion de la sortie de la boucle si le bouton est préssé (interruption)
  }
  /*
  ** BOUTON PRÉSSÉ
  */
  initGraph(); // Bouton préssé donc on initialise le jeu 
  distance = 0; // Réinitialisation du score
}


/*
** FONCTION DE MARCHE EN MODE ATTENTE
*/

void walk(int i, int time){
  lcd.setCursor(i, 1);
  lcd.write(ANIM_RUN1);
  delay(time/2);

  lcd.setCursor(i,1);
  lcd.write(" ");
  lcd.setCursor(i+1,1);
  lcd.write(ANIM_RUN2);
  delay(time/2);
}


/*
** INITIALISATION DU MODE DE JEU
*/

void initGraph(){
  // Création des bytes pour les animations
  byte graphics1[8] = {
    // Course position 1
    B01100,
    B01100,
    B00000,
    B01110,
    B11100,
    B01100,
    B11010,
    B10011,
  };
  byte graphics2[8] = {
    // Course position 2
    B01100,
    B01100,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B01110,
  };
  byte graphics3[8] = {
    // Saut
    B01100,
    B01100,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
  };
  byte graphics4[8] = {
    // Saut corps (part 2)
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B00000,
    B00000,
    B00000,
  };
  byte leftWall[8]={
    // arbre gauche moitié plein
    B00011,
    B00111,
    B00111,
    B00111,
    B00011,
    B00001,
    B00001,
    B00011
  };   
  byte fullWall[8]={
    // arbre milieu plein
    B11011,
    B11111,
    B11111,
    B11111,
    B11011,
    B10001,
    B10001,
    B11011
  };       
  byte rightWall[8]={
    // arbre droite moitié plein
    B11000,
    B11100,
    B11100,
    B11100,
    B11000,
    B10000,
    B10000,
    B11000,
  };   
  int i = 0;
  
  // Implémentation des bytes pour le LCD
  lcd.createChar(1, graphics1);
  lcd.createChar(2, graphics2);
  lcd.createChar(3, graphics3);
  lcd.createChar(4, graphics4);
  lcd.createChar(LEFT_WALL,leftWall);
  lcd.createChar(FULL_WALL,fullWall);
  lcd.createChar(RIGHT_WALL,rightWall);

  // Réinitialisation du tableau de Bytes
  for (i = 0; i < 16; ++i) {
    terrainTOP[i] = ' ';
    terrainBOTTOM[i] = ' ';
  }
}


/*
** FONCTION DE CRÉATION DE MURS ET D'ACTUALISATION 
*/

void wall(){
  if(creation == false){ // Si pas de mur en cours de création, on en défini un
    if (random(1,4)==2){ // Création de murs de manière aléatoire
      tailleMur = random(1,7);
      pos = 16;
      update = 15;
      creation = true;
      wall();
    }
  }
  else{ // Si un tableau est initialisé, il faut le créer et l'afficher
    if (pos != 15 - tailleMur){ // Si le mur n'est pas créé entièrement, il faut continuer de créer des morceaux
      createWall();  
    }
    else{ // S'il est entièrement créé, il faut l'actualiser à chaque tour
      updateWall();
    }
  }
}

/*
** CRÉATION DU MUR
*/

void createWall(){
  terrainBOTTOM[pos]=LEFT_WALL; // On affiche un demi arbre 
  if (tailleMur != 1){          // S'il faut plus d'un arbre, on affiche des arbres pleins
    terrainBOTTOM[pos + 1]=FULL_WALL;
  }
  
  pos--; // On retire 1 à pos pour écrire avant si nécessaire
}

 /*
 ** ACTUALISATION DU MUR POUR LE FAIRE AVANCER ET DISPARAITRE
 */

void updateWall(){
  terrainBOTTOM[update + 1] = ' '; // On retire le dernier morceau d'arbre pour le faire avancer
  if(update - tailleMur >= 0){ // si on est pas encore à l'extrémité de l'écran
    terrainBOTTOM[update - tailleMur] = LEFT_WALL; // On ajoute un demi arbre au début
  }
  if((tailleMur != 1) && (update >0)){   // s'il faut plus d'un arbre, il faut ajouter des arbres pleins au millieu
    terrainBOTTOM[update - tailleMur + 1] = FULL_WALL; 
  }
  terrainBOTTOM[update] = RIGHT_WALL; // On ajoute un demi arbre à la fin
  if(update == -1) {
    creation = false; // Si le mur a disparu, creation = false pour pouvoir recréer un mur le tour après
	terrainBOTTOM[0] = ' ';  // on supprime le dernier morceau d'arbre restant
  }else{
    update--; //On retire 1 à update pour faire un avancement
  }
}