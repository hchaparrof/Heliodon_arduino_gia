// definiciones funcionamiento brazo
//boton parar
const int EMERGENCIA = 27;
//
const int MILLIS_ESPERA = -1;
const int PULSOS = 4000;
int pulsos_acumulados = 0;
long int delta_time = 0;
long int millis_anterior = 0;
long int millis_actual = 0;
long int acumulado = 0;
bool funcionando = 0;
//const int 
////////
//////////////configuracion de los pines
///////entradas
////carro
const int CARRO_MAS = 25; 
const int CARRO_MENOS = 26;
const int PWMCARRO = -1;
bool carro_andando = false;
const int DURACION_CARRO_MILLIS = 2000;
long int acumulado_carro = 0;
////arco
const int ARCO_MAS = 35;
const int ARCO_MENOS = 39;
//// encoder
const int ENCODERA = -1;  // Cambia según tu configuración
const int ENCODERB = -1;   // Cambia según tu configuración
const int POSICIONAL_CARRO = -1;
////pot
const int POT_ARCO = 33;
////emergencia
//const int EMERGENCIA = 1;
////botones 
const int BOT_ARCO_IZQ = 32;
const int BOT_ARCO_DER = 35;
const int BOT_CARRO_IZQ = 34; //abajo izq
const int BOT_CARRO_DER = 36; // abajo der
///////
///////salidas
////paso a paso brazo
const int ENABLE = 4;
const int DIRECCION = 16;
const int PULSO = 17;
////carro motor normal
const int DIR_MAS = -1;
const int DIR_MENOS = -1;
const int PWM = -1;
////regulador luz
const int REGULADOR_LUZ = -1;
const int AUX_CEL = -1;

// El LED integrado de la ESP32 está en el pin 2
const int ledPin = 2;
long int milis_actuales = 0;
long int milis_acumulados = 0;
//bool funcionando = false;
bool ahora = false;
int contador = 0;
int pulsos_dados = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("hola");
  Serial.println("hola_21");

  Serial.println("hola_22");
  pinMode(BOT_ARCO_IZQ, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(BOT_ARCO_IZQ), button_4_red_arco, FALLING); 
  pinMode(BOT_ARCO_DER, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(BOT_ARCO_DER), button_6_red_arco, FALLING); 
  pinMode(BOT_CARRO_IZQ, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(BOT_CARRO_IZQ), button_2_red_carro, FALLING); 
  pinMode(BOT_CARRO_DER, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(BOT_CARRO_DER), button_8_red_carro, FALLING); 
  pinMode(EMERGENCIA, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(EMERGENCIA), button_black_emergencia, FALLING); 
//Serial.println("hola_2");
  Serial.println("hola");
  
  // Configura el pin del LED como salida
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRECCION, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(PULSO, OUTPUT);
  digitalWrite(PULSO, LOW);
  digitalWrite(ENABLE, HIGH);
  Serial.println("hola__3");
  // carro
  pinMode(CARRO_MAS, OUTPUT);
  pinMode(CARRO_MENOS, OUTPUT);
  digitalWrite(CARRO_MAS, LOW);
  digitalWrite(CARRO_MENOS, LOW);
  pinMode(PWMCARRO, OUTPUT);
  // Apaga el LED al inicio
  digitalWrite(ledPin, HIGH);
  apagar_carro();
  Serial.println("hola");
}
bool bandera_loop = false;
int commaIndex = -1;
void loop() {
  millis_actual = millis();
  delta_time = millis_actual - millis_anterior;
  millis_anterior = millis_actual;
  if(funcionando){
    acumulado += delta_time;
    if(acumulado > 0){
      acumulado = 0;
      digitalWrite(PULSO, !digitalRead(PULSO));
      if(!digitalRead(PULSO)){
        pulsos_acumulados++;
        if(pulsos_acumulados > PULSOS-1){
          apagar_arco();
        }
      }
    }
  }
  if (carro_andando){
    acumulado_carro += delta_time;
    if (acumulado_carro > DURACION_CARRO_MILLIS){
      acumulado_carro = 0;
      apagar_carro();
    }
  }

  // Verifica si hay datos disponibles en el puerto serial
  if (bandera_loop){
    Serial.println(commaIndex);
    bandera_loop = false;
  }
if (Serial.available() > 0) {
    bandera_loop = true;
    // Lee la cadena hasta encontrar un salto de línea
    String inputString = Serial.readStringUntil('\n');
    
    // Elimina espacios en blanco al inicio y final
    inputString.trim();

    // Verifica si la entrada contiene una coma (para azimut y zenit)
    commaIndex = inputString.indexOf(',');
    Serial.print(commaIndex);
    if (commaIndex != -1) {
      Serial.print("Azimut recibido: ");
      // 📌 Modo: Recepción de valores flotantes (azimut, zenit)
      
      String azimutString = inputString.substring(0, commaIndex);
      String zenitString = inputString.substring(commaIndex + 1);

      // Convierte a flotantes
      float azimut = azimutString.toFloat();
      float zenit = zenitString.toFloat();

      // Verifica que no haya errores en la conversión
      if (!(azimut == 0.0 && azimutString != "0") && 
          !(zenit == 0.0 && zenitString != "0")) {
        
        Serial.print("Azimut recibido: ");
        Serial.println(azimut);
        Serial.print("Zenit recibido: ");
        Serial.println(zenit);
        
        // 🔹 Aquí puedes procesar los valores como necesites

      } else {
        Serial.println("❌ Error: Entrada no válida.");
      }
    
    } else {  
      // 📌 Modo: Recepción de enteros (comandos)

      int inputNumber = inputString.toInt();  // Convierte la entrada a entero

      if (inputNumber >= 2 && inputNumber <= 8) {
        Serial.print("Comando recibido: ");
        Serial.println(inputNumber);
        
        switch (inputNumber) {
          case 2:
            button_2_red_carro();
            break;
          case 4:
            button_4_red_arco();
            break;
          case 5:
            button_black_emergencia();
            break;
          case 6:
            button_6_red_arco();
            break;
          case 8:
            button_8_red_carro();
            break;
        }
        digitalWrite(ledPin, HIGH);
      } else {
        Serial.println("❌ Error: Número fuera de rango.");
        digitalWrite(ledPin, LOW);
      }
    }
  }
}
void button_black_emergencia(){
  apagar_carro();
  apagar_arco();
}
void button_4_red_arco() {
  mover_arco(HIGH);
}
void button_6_red_arco() {
  mover_arco(LOW);
}
void button_2_red_carro(){
  mover_carro(true);
}
void button_8_red_carro(){
  mover_carro(false);
}
void mover_carro(bool direccion){
  //Serial.println("hola_carro");
  carro_andando = true;

  if (direccion){
    digitalWrite(CARRO_MAS, HIGH);
    digitalWrite(CARRO_MENOS, LOW);
  }else{
    digitalWrite(CARRO_MAS, LOW);
    digitalWrite(CARRO_MENOS, HIGH);
  }
}
void apagar_carro(){
    carro_andando = false;
    digitalWrite(CARRO_MAS, LOW);
    digitalWrite(CARRO_MENOS, LOW);
}
void cambiar_vel_carro(float valor){
  analogWrite(PWMCARRO, valor);
}
volatile long recorrido = 0;  // Variable para almacenar el recorrido

// Variables internas para seguimiento del estado del encoder
volatile int estadoAAnterior = 0;

/*void IRAM_ATTR actualizarRecorrido() {
  int estadoA = digitalRead(ENCODERA);
  int estadoB = digitalRead(ENCODERB);
  if (estadoA != estadoAAnterior) {  // Detecta cambio en el pin A
    if (estadoA == estadoB) {
      recorrido++;  // Giro en una dirección
    } else {
      recorrido--;  // Giro en la otra dirección
    }
  }
  estadoAAnterior = estadoA;
}*/
void mover_arco(bool direccion){
  //Serial.println("hola_arco");
      if (!digitalRead(ENABLE)){
        digitalWrite(ENABLE, HIGH);
      }
      funcionando = true;
      digitalWrite(DIRECCION, direccion);

      // Enciende el LED si el número es correcto
      digitalWrite(ledPin, direccion);
}
void apagar_arco(){
    pulsos_acumulados = 0;
    funcionando = 0;
}