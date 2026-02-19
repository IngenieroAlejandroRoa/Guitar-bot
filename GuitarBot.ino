//Final FInal


#include <ESP32Servo.h>

// ————— Servos de acordes y rasgueo —————
Servo servos[9];
const int pinesServos[9]  = {12, 14, 27, 26, 25, 33, 32,  2,  4};
Servo servoStrum;
const int pinServoStrum   = 18;

// ————— Pines y configuración compartida —————
const float stepsPerMm    = 6.2348;     // Pasos por mm
int   stepDelay_us        = 580;        // µs entre pulsos

// ————— Paso a paso para rasgueo —————
const int pinStepStrum    = 23;
const int pinDirStrum     = 22;
const int pinEndstopStrum = 21;
long  pasosStrum60mm      = (long)( 95.0 * stepsPerMm);   // 60 mm
long  pasosStrum105mm     = (long)(105.0 * stepsPerMm);   // 105 mm
long pasosStrum07mm       = (long)(1*stepsPerMm);

// ————— Paso a paso para punteo —————
const int pinStepPick     = 5;
const int pinDirPick      = 15;
const int pinEndstopPick  = 13;
// Secuencia de punteo: -1 = homing; >0 = avanzar mm
const int punteoSeq[]     = { -1, 35, -1, -1, 35, -1, -1, 35, 66, -1, 35, 66 };
const int punteoLen       = sizeof(punteoSeq) / sizeof(punteoSeq[0]);

// ————— Acordes —————
const int numChords = 4;
int chordAngles[numChords][9] = {
  { 90,  90,  90,   0, 180,  90,  90,  90,  90},
  { 90,  90,  90,  90,   0,  90, 180,  90,  90},
  { 90,  90,  90, 180,  90,  90,   0,  90, 180},
  { 90,  90,  90,  90, 180, 180,  90,  90,   0}
};

// ————— Prototipos —————
void stepMotorStrum(), moveStepsStrum(long), performHomingStrum();
void stepMotorPick(), moveStepsPick(long),  performHomingPick(), performMovePickMm(int);
void executePunteo();

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== ROBOT GUITARRA: ACORDES, RASGUEO & PUNTEO ===");

  // Inicializar servos de acordes
  for (int i = 0; i < 9; i++) {
    servos[i].setPeriodHertz(50);
    servos[i].attach(pinesServos[i], 500, 2500);
    servos[i].write(90);
  }
  // Servo de rasgueo
  servoStrum.setPeriodHertz(50);
  servoStrum.attach(pinServoStrum, 500, 2500);
  servoStrum.write(90);

  // Pines paso a paso (rasgueo)
  pinMode(pinStepStrum,    OUTPUT);
  pinMode(pinDirStrum,     OUTPUT);
  pinMode(pinEndstopStrum, INPUT_PULLUP);
  // Pines paso a paso (punteo)
  pinMode(pinStepPick,     OUTPUT);
  pinMode(pinDirPick,      OUTPUT);
  pinMode(pinEndstopPick,  INPUT_PULLUP);

  // Homing inicial
  performHomingStrum();
  performHomingPick();

  // Posicionar rasgueo a 105 mm
  Serial.println("→ Posicionar rasgueo a 105 mm inicial...");
  servoStrum.write(23);
  digitalWrite(pinDirStrum, HIGH);
  moveStepsStrum(pasosStrum105mm);
  Serial.println("   Listo.\n");
}

void loop() {
  static int  strumCount  = 0;
  static bool at105Strum  = true;

  // Al completar 4 acordes × 4 rasgueos = 16, hacer punteo
  if (strumCount == numChords * 4) {
    executePunteo();
    // Reestablecer todos los servos a 90°
    for (int i = 0; i < 9; i++) servos[i].write(90);
    servoStrum.write(90);
    Serial.println("→ Todos los servos en 90°\n");
    strumCount = 0;
    delay(500);
    return;
  }

  // Seleccionar acorde cada 4 rasgueos
  if (strumCount % 4 == 0) {
    int idx = (strumCount / 4) % numChords;
    for (int s = 0; s < 9; s++) {
      servos[s].write(chordAngles[idx][s]);
    }
    Serial.printf("→ Acorde %d\n", idx);
  }

  // Ejecutar rasgueo: alterna 105 ↔ 45 mm
  if (at105Strum) {
    servoStrum.write(147);
    digitalWrite(pinDirStrum, LOW);
    moveStepsStrum(pasosStrum60mm);
    Serial.println("   Rasgueo: 105→45 mm");
  } else {
    servoStrum.write(23);
    digitalWrite(pinDirStrum, HIGH);
    moveStepsStrum(pasosStrum60mm);
    Serial.println("   Rasgueo: 45→105 mm");
  }
  at105Strum = !at105Strum;
  strumCount++;
  //delay(200);
}

// ————— Funciones rasgueo —————
void stepMotorStrum() {
  digitalWrite(pinStepStrum, HIGH);
  delayMicroseconds(stepDelay_us);
  digitalWrite(pinStepStrum, LOW);
  delayMicroseconds(stepDelay_us);
}
void moveStepsStrum(long n) {
  while (n--) stepMotorStrum();
}
void performHomingStrum() {
  Serial.println("→ HOMING rasgueo...");
  digitalWrite(pinDirStrum, LOW);
  while (digitalRead(pinEndstopStrum) == HIGH) {
    stepMotorStrum();
  }
  Serial.println("   Fin de carrera rasgueo OK");
  delay(100);
}

// ————— Funciones punteo —————
void stepMotorPick() {
  digitalWrite(pinStepPick, HIGH);
  delayMicroseconds(stepDelay_us);
  digitalWrite(pinStepPick, LOW);
  delayMicroseconds(stepDelay_us);
}
void moveStepsPick(long n) {
  while (n--) stepMotorPick();
}
void performHomingPick() {
  Serial.println("→ HOMING punteo...");
  digitalWrite(pinDirPick, LOW);
  while (digitalRead(pinEndstopPick) == HIGH) {
    stepMotorPick();
  }
  Serial.println("   Fin de carrera punteo OK");
  delay(100);
}
void performMovePickMm(int mm) {
  long pasos = (long)(mm * stepsPerMm);
  Serial.printf("→ MovePick: %d mm → %ld pasos\n", mm, pasos);
  digitalWrite(pinDirPick, HIGH);
  moveStepsPick(pasos);
  delay(100);
}

// ————— Ejecuta la secuencia de punteo —————
void executePunteo() {
  Serial.println("\n=== INICIANDO PUNTEO ===");
  Serial.println("→ Posicionar rasgueo a 105 mm inicial...");
  servoStrum.write(90);
  performHomingStrum();
  digitalWrite(pinDirStrum, HIGH);
  moveStepsStrum((long)(74.5 * stepsPerMm));                                                                  //Rasgueao puntepo
  Serial.println("   Listo.\n");
  servoStrum.write(90);
  for (int i = 0; i < punteoLen; i++) {
    if (punteoSeq[i] < 0) {
      performHomingPick();
    } else {
      performMovePickMm(punteoSeq[i]);
    }
    // Cada vez que el punteo se posicione, llevar servo en pin 27 (índice 2) a 0°
    servos[2].write(0);
    delay(200);
    servoStrum.write(170);                                                                                  //Grado para punteo
    delay(500);
    servos[2].write(90);



    digitalWrite(pinDirStrum, LOW);
    delay(10);
    moveStepsStrum(pasosStrum07mm);                                                              //Prueba para devolverse 





    servoStrum.write(90);



    
    digitalWrite(pinDirStrum, HIGH);
    delay(10);
    moveStepsStrum(pasosStrum07mm);

  }
  //performHomingStrum();
  performHomingPick();
  Serial.println("=== PUNTEO FINALIZADO ===\n");
  //servos[2].write(90);
  //servoStrum.write(90);
}
