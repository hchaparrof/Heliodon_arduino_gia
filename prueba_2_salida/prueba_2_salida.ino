#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Crear objeto ADS1115
Adafruit_ADS1115 ads;
volatile unsigned long lastEncoderTime = 0;
// definiciones funcionamiento brazo
//boton parar
const int EMERGENCIA = 27;
//
const int MILLIS_ESPERA = -1;
int PULSOS = 1500;
int pulsos_a_dar = 0;
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
int destino_carrito = 0; // angulo
float angulo_actual = -1;
volatile bool dirreccion_carrito = true;
volatile int direccion_carrito = 1;
const int PWMCARRO = 5;
const int canalPWM = 0;   // Canal de PWM (0-15 en ESP32)
const int frecuencia = 5000; // Frecuencia en Hz (ajusta según tu motor)
const int resolucion = 8; // Resolución de 8 bits (valores de 0 a 255)
bool carro_andando = false;
volatile int theta = 0;
volatile int theta_1 = 0;
volatile int theta_2 = 0;
const int DURACION_CARRO_MILLIS = 3500;
long int acumulado_carro = 0;
////arco
//// encoder
const int POSICIONAL_CARRO = -1;
volatile long posicion = 0;
volatile bool direccion = true;
float velocidad_carro = 200;
////pot
const int POTENCIOMETRO = 33;
////emergencia
//const int EMERGENCIA = 1;
////botones 
const int BOT_ARCO_IZQ = 32;
const int BOT_ARCO_DER = 35;
const int BOT_CARRO_IZQ = 34; //abajo izq
const int BOT_CARRO_DER = 36; // abajo der
bool automatico = false;

///////
///////salidas
////paso a paso brazo
const int ENABLE = 4;
const int DIRECCION = 16;
const int PULSO = 17;
volatile long posicion_brazo = 0;
volatile long direccion_brazo = 1;
////carro motor normal
//const int PWMCARRO = -1;
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
////posicionamiento
//brazo
float posbrazo = 0;
float PULSEPERANG = 44.44;
//carro
float poscarro = 0;
float HITPERANG = 15;

//////adc


//Adafruit_ADS1115 ads;
//////adc
//pwm
const int pwmChannel = 0;  // Canal PWM (0-15)
const int freq = 5000;      // Frecuencia del PWM en Hz
const int resolution = 8;   // Resolución de 8 bits (0-255)
struct DataPoint {
    int position;
    float angle;
};

float angulo_calibrado(float voltaje){
  float a = -15.5517; float b = 61.4428; float c = 105.6066;
  float x = voltaje;
  return a*x*x + b*x + c;
}

void debug_angulo(){
  return;
  int16_t adc0 = ads.readADC_SingleEnded(3);
  float volts = ads.computeVolts(adc0);
  angulo_actual = angulo_calibrado(volts);
  return;
}
void calcular_angulo(){ //debug
  float maximo = 4.19; float minimo = 0.98;
  float ang_max = 15; float ang_min = 170;
  int16_t adc0 = ads.readADC_SingleEnded(3);
  float volts = ads.computeVolts(adc0);
  float pendiente = (170.0-15.0)/(0.98-4.19);
  angulo_actual = pendiente * (volts - 4.19) + 15;

  // Limitar el ángulo a los valores dados
  if (angulo_actual < ang_max) angulo_actual = ang_max;
  if (angulo_actual > ang_min) angulo_actual = ang_min;

  Serial.print("Voltios: ");
  Serial.print(volts);
  Serial.print(" -> Ángulo estimado: ");
  Serial.println(angulo_actual);
}

void setup() {
  Serial.begin(115200);
    if (!ads.begin()) {
    Serial.println("Error al encontrar ADS1115");
    while (1);
  }
  ads.setGain(GAIN_TWOTHIRDS);
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

  //ledcSetup(pwmChannel, freq, resolution);  // Configura el canal PWM
  //ledcAttachPin(PWMCARRO, pwmChannel);        // Asigna el pin al canal PWM
  //ledcWrite(pwmChannel, 255*0.2); 

  pinMode(PWMCARRO, OUTPUT);
  analogWrite(PWMCARRO, 255*0.2);  // 50% de 255 (8 bits) = 127
  //ads.begin();
  //digitalWrite(PWMCARRO, 127);
//Serial.println("hola_2");
  
  // Configura el pin del LED como salida
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRECCION, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(PULSO, OUTPUT);
  digitalWrite(PULSO, LOW);
  digitalWrite(ENABLE, HIGH);
  // carro
  pinMode(CARRO_MAS, OUTPUT);
  pinMode(CARRO_MENOS, OUTPUT);
  digitalWrite(CARRO_MAS, LOW);
  digitalWrite(CARRO_MENOS, LOW);
  // Apaga el LED al inicio
  digitalWrite(ledPin, HIGH);
  apagar_carro();
  Serial.println("hola");
}
bool bandera_loop = false;
int commaIndex = -1;
void loop() {
  int16_t adc0, adc1, adc2, adc3;
  //adc0 = ads.readADC_SingleEnded(0);
  //adc1 = ads.readADC_SingleEnded(1);
  //adc2 = ads.readADC_SingleEnded(2);
  //adc3 = ads.readADC_SingleEnded(3);
  //Serial.print("AIN0: "); Serial.println(adc0);
  //int16_t results = ads.readADC_Differential_0_1();
  //Serial.print("Diferencial: "); 
  //Serial.print(results);
  //poscarro = float(theta_1)*cal_theta_1 - float(theta_2);
  if (automatico){

  }else{
    int valor_pot = analogRead(POTENCIOMETRO);
    cambiar_vel_carro(255*valor_pot/4000);
  }
  millis_actual = millis();
  delta_time = millis_actual - millis_anterior;
  millis_anterior = millis_actual;
  if(funcionando){ //arco moviendose
    acumulado += delta_time;
    if(acumulado > 0){
      acumulado = 0;
      digitalWrite(PULSO, !digitalRead(PULSO));
      if(!digitalRead(PULSO)){
        pulsos_acumulados++;
        posicion_brazo += 1*direccion_brazo;
        posbrazo += (1/PULSEPERANG) * direccion_brazo;
        if(pulsos_acumulados > pulsos_a_dar - 1){
          Serial.println(pulsos_acumulados);
          Serial.println(pulsos_a_dar);
          Serial.println("apagando_arco");
          apagar_arco();
        }
      }
    }
  }
  if (carro_andando){
    calcular_angulo();
    if(!automatico){
      if (acumulado_carro > DURACION_CARRO_MILLIS){
        acumulado_carro = 0;
        apagar_carro();
      }
    }else{
      if (direccion_carrito == 1){
        if (angulo_actual > destino_carrito){
          apagar_carro();
        }
      }else{
        if (angulo_actual < destino_carrito){
          apagar_carro();
        }
      }
    }
  }

  // Verifica si hay datos disponibles en el puerto serial
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
        // 📌 Modo: Recepción de valores flotantes (azimut, zenit)
        
        String azimutString = inputString.substring(0, commaIndex);
        String zenitString = inputString.substring(commaIndex + 1);

        // Convierte a flotantes
        float azimut = azimutString.toFloat();
        float zenit = zenitString.toFloat();

        // Verifica que no haya errores en la conversión
        if (!(azimut == 0.0 && azimutString != "0") && 
            !(zenit == 0.0 && zenitString != "0")) {}
          
          // 🟢 **Caso 1: Valores normales**
          if (azimut >= 0 && zenit >= 0) {
            calcular_angulo();
            automatico = true;
            Serial.print("Azimut recibido: ");
            Serial.println(azimut);
            Serial.print("Zenit recibido: ");
            Serial.println(zenit);

            float valores[2];
            transformarCoordenadas(azimut, zenit, valores);

            Serial.print("Ángulo: ");
            Serial.println(valores[0]);
            Serial.print("Carrito: ");
            Serial.println(valores[1]);
            float angulo_a_ir_carrito = valores[1];
            destino_carrito = angulo_a_ir_carrito + 90.0;
            Serial.println("destino_carrito:");
            Serial.println(destino_carrito);
            Serial.println("angulo actual");
            Serial.println(angulo_actual);
            if (angulo_actual != destino_carrito){
              if (angulo_actual > destino_carrito){
                direccion_carrito = -1;
                button_8_red_carro();
              }else{
                direccion_carrito = 1;
                button_2_red_carro();
              }
            }
            // apagar_carro();

            float diferencia = -(valores[0] - posbrazo);  // Diferencia con signo
            Serial.println(diferencia);
            pulsos_a_dar = abs(diferencia) * PULSEPERANG;  // Conversión a pulsos
            Serial.print(pulsos_a_dar);
            Serial.println(" pulsos");
            direccion_brazo = diferencia/abs(diferencia);

            // Determinar dirección y mover
            if (diferencia < 0) {
              Serial.println("moviendo_arco");
                mover_arco(false);  // Dirección positiva
            } else {
                mover_arco(true);  // Dirección negativa
            }

            // ✅ Actualizar la posición en base a los pulsos realmente ejecutados
            Serial.print("posición: ");
            Serial.println(posbrazo);

          // 🔴 **Caso 2: Valores negativos -> Modo calibración**
          } else if (azimut < 0.0 && zenit < 0.0) {
        } else {}
      }else{

          // 📌 Modo: Recepción de enteros (comandos)
        float valor_float = inputString.toFloat();

        int inputNumber = inputString.toInt();  // Convierte la entrada a entero
        if (float(inputNumber)/valor_float != 1){
          //cal_theta_1 = valor_float;
          inputNumber = -1;
          Serial.println("calibracion_cambiada");
        }

        if ((inputNumber >= 2 && inputNumber <= 8)) {
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
  automatico = false;
  apagar_carro();
  apagar_arco();
}
void button_4_red_arco() {
  direccion_brazo = 1;
  pulsos_a_dar = PULSOS;
  mover_arco(HIGH);
}
void button_6_red_arco() {
  direccion_brazo = -1;
  pulsos_a_dar = PULSOS;
  mover_arco(LOW);
}
void button_2_red_carro(){
  mover_carro(true);
}
void button_8_red_carro(){
  mover_carro(false);
}
void mover_carro(bool direccion){
  Serial.println("prendiendo_carro");
  dirreccion_carrito = direccion;
  //Serial.println("hola_carro");
  carro_andando = true;

  if (direccion){
    //digitalWrite(CARRO_MAS, HIGH);
    analogWrite(CARRO_MAS, velocidad_carro);
    digitalWrite(CARRO_MENOS, LOW);
  }else{
    digitalWrite(CARRO_MAS, LOW);
    analogWrite(CARRO_MENOS, velocidad_carro);
  }
}
void apagar_carro(){
  Serial.println("apagando carro");
    carro_andando = false;
    analogWrite(CARRO_MAS, 0);
    analogWrite(CARRO_MENOS, 0);
    calcular_angulo();
    Serial.println("angulo_actual");
    Serial.println(angulo_actual);
    debug_angulo();
    //Serial.println(cal_theta_1);
}
void cambiar_vel_carro(float valor){
  velocidad_carro = valor;
}

// Variables internas para seguimiento del estado del encoder



// Rutina de interrupción para el canal B
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
    automatico = false;
    Serial.println(posbrazo);
}

// Función para convertir grados a radianes
float toRadians(float degrees) {
    return degrees * (M_PI / 180.0);
}

// Función para convertir radianes a grados
float toDegrees(float radians) {
    return radians * (180.0 / M_PI);
}
void transformarCoordenadas(float az, float zen, float resultado[2]) {
    // Convertir a radianes
    az = toRadians(az);
    zen = toRadians(zen);

    // Convertir a coordenadas cartesianas en el sistema original (Y vertical)
    float x = cos(zen) * cos(az);
    float y = cos(zen) * sin(az);
    float z = sin(zen);

    // Calcular los ángulos en el nuevo sistema
    float angulo = atan(x / z);
    float carrito = atan2(y, sqrt(x * x + z * z));

    // Guardar los valores en el array
    resultado[0] = toDegrees(angulo);
    resultado[1] = toDegrees(carrito);
}