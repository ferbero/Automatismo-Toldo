/* Programa para xestionar o movemento dun toldo (pregue ou despregue)
 *
 * Un pulsador (pin 8) fará o seguinte:
 * - Se o toldo está quieto, moveráo en sentido contrario ó movemento anterior
 * - Se o toldo está en movemento, párao
 *
 * Un fin de carreira de pregue (pin 9) detén o movemento de pregado do toldo
 * Un fin de carreira de despregue (pin 10) detén o movemento de despregado do toldo
 *
 * A velocidade de movemento do toldo variara según a luminosidade detectada por unha resistencia fotoeléctica.
 * LDR: Pin A0
 * O sentido de xiro do motor controlarase mediante un circuito integrado L293D
 * L293D: pins 5 a 7
 *
 * Autor: Fer
 * Data: marzo de 2024
 *
 */


// Definición de entradas e saídas
#define LDR A0

#define MARCHA 4
#define POTENCIA 5
#define DEXTRO 6
#define LEVO 7

#define PULSADOR 8
#define FC_DESPREGUE 9
#define FC_PREGUE 10

// Definición de valores medidos empíricamente
#define V_MIN 258
#define V_MAX 1015
// Número de niveis
#define N_NIVELES 5

bool inversion = false; // Indica se o motor xira nun sentido ou no inverso
bool motorActivo = false; // Indica se o motor está en marcha
byte velocidade = 0; // Velocidade do motor do toldo
byte nivel = 0; // Nivel de velocidade do motor

void setup()
{
  // Indícase o modo de funcionamento das entradas e saídas dixitais
  pinMode(DEXTRO, OUTPUT);
  pinMode(LEVO, OUTPUT);
  
  pinMode(PULSADOR, INPUT);
  pinMode(FC_PREGUE, INPUT);
  pinMode(FC_DESPREGUE, INPUT);
  Serial.begin(9600);
}

void loop()
{
  if (digitalRead(PULSADOR)) lerPulsador(); // Se hay pulsación, xestionase coa funcion lerPulsador()
  if (inversion){
    if (digitalRead(FC_PREGUE)) finCarrera();
  } else if (digitalRead(FC_DESPREGUE)) finCarrera();
  if (motorActivo) moverToldo();
  while (digitalRead(PULSADOR)) delay(20);
  delay(200);
}



/* Definición das funcións utilizadas para controlar o comportamento do toldo:
 * - Cando se preme o pulsador
 * - Cando se detecta un fin de carreira
 * - Estado do movemento
 * - Velocidade de movemento
 */

void lerPulsador(){
  if (motorActivo) pararMotor();
  else {
    inversion = !inversion;
    digitalWrite(MARCHA, HIGH);
  }
  motorActivo = !motorActivo;
  Serial.println("PULSACION");
  Serial.println(log(V_MAX));
  Serial.println(log(V_MIN));
}

void finCarrera(){
  motorActivo = false;
  pararMotor();
}

void moverToldo(){
  velocidade = calcularVelocidade();
  xirarMotor();
}

int calcularVelocidade(){
  nivel = map(analogRead(LDR), V_MIN, V_MAX, 5, 0); // Mapea tensión a nivel
  velocidade = nivel== 0? 5: nivel*20; // Calcula velocidade segun nivel:
  Serial.println(analogRead(LDR));
  return velocidade*255/100;
}

void xirarMotor(){ // Xira o motor según o estado de inversion e velocidade
  digitalWrite(LEVO, inversion); 
  digitalWrite(DEXTRO, !inversion);
  analogWrite(POTENCIA, velocidade);
}

// Detén o motor
void pararMotor(){
  digitalWrite(MARCHA, LOW); // desactiva o Enable
}
