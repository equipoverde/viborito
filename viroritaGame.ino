
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>


// datos de la serpiente
struct serp_dat {
  int id;
  int read[2] = {512, 512}; // Joysticks
  int joystick[2]; // Joysticks, pines de x y y
  int head[2]; // punta de serpiente
  LinkedList<int> tail = LinkedList<int>(); // posicion de la serp
  uint32_t color;
  uint32_t headColor;
  char direcc; // direccion de serpiente
};

const int SIDE = 16; // tamañode matriz por lado
const int jugadores = 2; // jugadores participantes
//funcion para que sean inversas una de la otra serpiente
const char DICT[2][4] = {{'d', 'u', 'r', 'l'}, {'u', 'd', 'l', 'r'}};

const int LED_PIN = 3; // pin de salida de matriz

Adafruit_NeoPixel strip = Adafruit_NeoPixel(256, LED_PIN, NEO_GRB + NEO_KHZ800); 

// variables de serpientes (colores y pines de mando en arduino)
                            //  X   Y
const int JOYSTICK_PINS_0[2] = {A0, A1};
const int JOYSTICK_PINS_1[2] = {A2, A3};

const int INITIAL_HEAD_POS_0[2] = {0, 0};
const int INITIAL_HEAD_POS_1[2] = {15, 15};
const int INITIAL_TAIL_0[5] = {0, 1, 2, 3, 4};
const int INITIAL_TAIL_1[5] = {240, 241, 242, 243, 244};
//  colores de serpientes y color de contador
                                         //(R   G   B)
const uint32_t SNAKE_COLOR_0 = strip.Color(52, 152, 219);
const uint32_t SNAKE_COLOR_1 = strip.Color(247, 220, 111);
const uint32_t contador = strip.Color(108, 52, 131);


// conteo de 3 - 1 en matriz
const int yew_tres[13] = {89, 88, 87, 86, 73, 54, 56, 55, 41, 22, 25, 24, 23};
const int yew_dos[14] = {89, 88, 87, 86, 73, 57, 56, 55, 54, 38, 25, 24, 23, 22};
const int yew_uno[10] = {23, 40, 55, 72, 87, 24, 39, 56, 71, 88};

serp_dat serps[jugadores];
int ganador;
int comida;
//valor para el retardo
int delayValue;

void setup() {
  randomSeed(analogRead(1));
  strip.begin();
  strip.setBrightness(90);
  strip.show(); 

  inicioValores();
}

void inicioValores() {
  setupSerp(&serps[0], 0, JOYSTICK_PINS_0, INITIAL_HEAD_POS_0, INITIAL_TAIL_0, strip.Color(52, 152, 219), strip.Color(52, 152, 219), 'l');
  setupSerp(&serps[1], 1, JOYSTICK_PINS_1, INITIAL_HEAD_POS_1, INITIAL_TAIL_1, strip.Color(247, 220, 111), strip.Color(247, 220, 111), 'r');
  delayValue = 300;
  iniciarCont();
  setComidaPos();
  printCom();
}


void setupSerp(serp_dat *serpiente, int id, int joystick[2], int head[2], int tail[4], uint32_t color, uint32_t headColor, char direction) {
  serpiente->id = id;
  serpiente->joystick[0] = joystick[0];
  serpiente->joystick[1] = joystick[1];
  serpiente->head[0] = head[0];
  serpiente->head[1] = head[1];

  for (int i = 0; i < 5; i++) {
    serpiente->tail.add(tail[i]);
  }

  serpiente->color = color;
  serpiente->headColor = headColor;
  serpiente->direcc = direction;
}
//inicia contador de inicio
void iniciarCont() {
  // 3
  for (int i = 0; i < 13; i++) {
    strip.setPixelColor(yew_tres[i], contador);
  }
  strip.show();
  delay(900);
  turnOffLeds();  

  //  2
  for (int i = 0; i < 14; i++) {
    strip.setPixelColor(yew_dos[i], contador);
  }
  
  strip.show();
  delay(900);
  turnOffLeds(); 

  //  1
  for (int i = 0; i < 10; i++) {
    strip.setPixelColor(yew_uno[i], contador);
  }
 
  strip.show();
  delay(900);
  turnOffLeds(); 
}

void loop() {
  ganador = isEndGame();

  if (ganador == -1)
    principal();
  else {
    reiniciaJuego();
  }
}

void principal() {
  for (int i = 0; i < jugadores; i++) {
    getMovi(&serps[i]);
    moveSerp(&serps[i]);
  }
  
  // con este delay la velocidad se ajustara si se quiere igual o que aumente conforme pase mas tiempo
  if (delayValue > 0)
    delay(delayValue = delayValue);
}

void getMovi(serp_dat *serpiente) {
  serpiente->read[0] = analogRead(serpiente->joystick[0]);
  delay(100);                 
  serpiente->read[1] = analogRead(serpiente->joystick[1]);

  if (serpiente->read[0] >= 900) {
    serpiente->direcc = DICT[serpiente->id][0];
  } else if (serpiente->read[0] < 200) {
    serpiente->direcc = DICT[serpiente->id][1];
  } else if (serpiente->read[1] < 200) {
    serpiente->direcc = DICT[serpiente->id][2];
  } else if (serpiente->read[1] >= 900) {
    serpiente->direcc = DICT[serpiente->id][3];
  } 
}

void moveSerp(serp_dat *serpiente) {
  if (!comeC(serpiente)) {
    int pos = serpiente->tail.pop();
    strip.setPixelColor(pos, strip.Color(0, 0, 0));
    strip.show();
  } else {
    setComidaPos();
    printCom();
  }

  nextRow(serpiente);
  nextColumn(serpiente);
  serpiente->tail.add(0, transform(serpiente->head[0], serpiente->head[1]));

  for (int i = 1; i < serpiente->tail.size(); i++) {
    strip.setPixelColor(serpiente->tail.get(i), serpiente->color);
  }

  strip.setPixelColor(serpiente->tail.get(0), serpiente->headColor);
  strip.show();
}

void nextRow(serp_dat *serpiente) {
  switch (serpiente->direcc) {
    case 'u':
      serpiente->head[0] += 1;
      break;
    case 'd':
      serpiente->head[0] -= 1;
      break;
  }

  if (serpiente->head[0] < 0)  
    serpiente->head[0] = SIDE - 1;
  
  serpiente->head[0] = serpiente->head[0] % SIDE;
}

void nextColumn(serp_dat *serpiente) {
  switch (serpiente->direcc) {
    case 'r': 
      serpiente->head[1] += 1;
      break;
    case 'l':
      serpiente->head[1] -= 1;
      break;
  }

  if (serpiente->head[1] < 0)  
    serpiente->head[1] = SIDE - 1;
  
  serpiente->head[1] = serpiente->head[1] % SIDE;
}

// Transform matrix position given in row and column to the actual matrix position
int transform(int row, int column) {
  if (row % 2 == 1)
    return row * SIDE + SIDE - (column + 1);
  
  return row*SIDE + column;
}

void setComidaPos() {
  comida = transform(random(SIDE), random(SIDE));

  for (int i = 0; i < jugadores; i++) {
    for (int j = 0; j < serps[i].tail.size(); j++) {
      if (comida == serps[i].tail.get(j))
        setComidaPos();
    }
  }
}
///color de la manzana
void printCom() {
  strip.setPixelColor(comida, strip.Color(211, 84, 0));
}

bool comeC(serp_dat *serpiente) {
  return serpiente->tail.get(0) == comida;
}

int isEndGame() {
  int randValue = random(2);

  if (randValue == 0) {
    if (checkWinner(&serps[0], &serps[1]))
      return 0;
    if (checkWinner(&serps[1], &serps[0]))
      return 1;
  } else {
    if (checkWinner(&serps[1], &serps[0]))
      return 1;
    if (checkWinner(&serps[0], &serps[1]))
      return 0;
  }

  return -1;
}

// comprobacion de la punta de serpiente si choca con la otra
bool checkWinner(serp_dat *snake_w, serp_dat *snake_l) {
  for (int i = 0; i < snake_w->tail.size(); i++) {
    if (snake_l->tail.get(0) == snake_w->tail.get(i)){
      return true;
    }
  }

  return false;
}


void turnOffLeds() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));   
  }

  strip.show();
}

void reiniciaJuego() {
  for (int i = 0; i < jugadores; i++) {
    serps[i].tail.clear();
  }

  inicioValores();
}
