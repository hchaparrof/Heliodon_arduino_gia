#define ENCODER_A       18 // Amarillo
#define ENCODER_B       19 // Verde
// definiciones funcionamiento brazo
//boton parar
const int EMERGENCIA = 27;
//
const int MILLIS_ESPERA = -1;
int PULSOS = 500;
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
const int PWMCARRO = 5;
const int canalPWM = 0;   // Canal de PWM (0-15 en ESP32)
const int frecuencia = 5000; // Frecuencia en Hz (ajusta según tu motor)
const int resolucion = 8; // Resolución de 8 bits (valores de 0 a 255)
bool carro_andando = false;
volatile int theta = 0;
const int DURACION_CARRO_MILLIS = 1000;
long int acumulado_carro = 0;
////arco
//const int ARCO_MAS = 35;
//const int ARCO_MENOS = 39;
//// encoder
const int POSICIONAL_CARRO = -1;
volatile long posicion = 0;
volatile int direccion = 0;
float velocidad_carro = 128;
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
float PULSEPERANG = 15;
//carro
float poscarro = 0;
float HITPERANG = 15;


//pwm
const int pwmChannel = 0;  // Canal PWM (0-15)
const int freq = 5000;      // Frecuencia del PWM en Hz
const int resolution = 8;   // Resolución de 8 bits (0-255)
struct DataPoint {
    int position;
    float angle;
};

const int numPoints = 56; // Número de datos en la tabla
DataPoint lookupTable[numPoints] = {
    {399, 84.98688624}, {690, 80.19571472}, {1153, 71.47463182}, {1635, 62.39911574}, {2080, 58.08505994},
    {2450, 53.48557076}, {3165, 46.7074986}, {3563, 41.03073654}, {3976, 38.17467354}, {4360, 30.56922412},
    {4755, 27.60667785}, {4908, 26.46488383}, {4616, 27.82409638}, {4053, 33.84534026}, {3541, 40.965844},
    {3110, 46.81419191}, {2764, 51.09586154}, {2414, 55.07549778}, {1983, 60.46121774}, {1569, 64.24472924},
    {1225, 69.90801227}, {894, 76.3902681}, {664, 80.58658583}, {437, 84.88715022}, {299, 86.88698059},
    {219, 88.74371497}, {-216, 91.7083151}, {-356, 94.06418578}, {-599, 98.28484138}, {-910, 103.7994854},
    {-1244, 110.2248594}, {-1640, 114.3947088}, {-1916, 118.384609}, {-2299, 123.550662}, {-2629, 127.759981},
    {-3059, 134.1586004}, {-3414, 136.7769871}, {-3775, 138.9692635}, {-4141, 141.0639508}, {-4467, 144.5470441},
    {-4684, 147.9380563}, {-4327, 146.3099325}, {-3816, 140.9644871}, {-3340, 135.5721924}, {-2988, 132.2085802},
    {-2649, 128.2901632}, {-2184, 122.0956482}, {-1929, 118.2677772}, {-1655, 115.1405829}, {-1336, 111.1485688},
    {-1062, 107.2967718}, {-790, 101.7923712}, {-562, 97.79207808}, {-433, 95.51147742}, {-317, 93.51375026},
    {-178, 91.00508601}
};

float interpolate(int position) {
    for (int i = 0; i < numPoints - 1; i++) {
        if (position >= lookupTable[i].position && position <= lookupTable[i + 1].position) {
            float x0 = lookupTable[i].position;
            float y0 = lookupTable[i].angle;
            float x1 = lookupTable[i + 1].position;
            float y1 = lookupTable[i + 1].angle;
            return y0 + (y1 - y0) * (position - x0) / (x1 - x0);
        }
    }
    // Si la posición está fuera de los límites, devuelve el valor más cercano
    if (position < lookupTable[0].position) return lookupTable[0].angle;
    if (position > lookupTable[numPoints - 1].position) return lookupTable[numPoints - 1].angle;
    return 0; // En caso de error (no debería ocurrir)
}


void setup() {
  Serial.begin(115200);
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
  pinMode(ENCODERA, INPUT);  // Configuramos el pin del botón como entrada
  attachInterrupt(digitalPinToInterrupt(ENCODERA), button_black_emergencia, FALLING);   
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A),leerEncoder,RISING);  

  //ledcSetup(pwmChannel, freq, resolution);  // Configura el canal PWM
  //ledcAttachPin(PWMCARRO, pwmChannel);        // Asigna el pin al canal PWM
  //ledcWrite(pwmChannel, 255*0.2); 

  pinMode(PWMCARRO, OUTPUT);
  analogWrite(PWMCARRO, 255*0.2);  // 50% de 255 (8 bits) = 127
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
        if(pulsos_acumulados > pulsos_a_dar - 1){
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
          float diferencia = valores[0] - posbrazo;  // Diferencia con signo
          int pulsos_a_dar = abs(diferencia) * PULSEPERANG;  // Conversión a pulsos

          // Determinar dirección y mover
          if (diferencia > 0) {
              mover_arco(true);  // Dirección positiva
          } else if (diferencia < 0) {
              mover_arco(false);  // Dirección negativa
          }

          // ✅ Actualizar la posición en base a los pulsos realmente ejecutados
          posbrazo += (pulsos_a_dar / PULSEPERANG) * (diferencia > 0 ? 1 : -1);
          Serial.print("Nueva posición actualizada del brazo: ");
          Serial.println(posbrazo);

        // 🔴 **Caso 2: Valores negativos -> Modo calibración**
        } else if (azimut < 0.0 && zenit < 0.0) {
          Serial.println("🔧 Modo Calibración: Procesando valores absolutos.");

          float absAzimut = fabs(azimut);  // Convierte a positivo
          float absZenit = fabs(zenit);

          Serial.print("Azimut calibrado: ");
          Serial.println(absAzimut);
          Serial.print("Zenit calibrado: ");
          Serial.println(absZenit);

          float valoresCalibracion[2];
          transformarCoordenadas(absAzimut, absZenit, valoresCalibracion);

          Serial.print("Ángulo calibrado: ");
          Serial.println(valoresCalibracion[0]);
          posbrazo = valoresCalibracion[0];
          Serial.print("Carrito calibrado: ");
          Serial.println(valoresCalibracion[1]);
          poscarro = valoresCalibracion[1];
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
    carro_andando = false;
    analogWrite(CARRO_MAS, 0);
    analogWrite(CARRO_MENOS, 0);
}
void cambiar_vel_carro(float valor){
  velocidad_carro = valor;
}
volatile long recorrido = 0;  // Variable para almacenar el recorrido

// Variables internas para seguimiento del estado del encoder
void leerEncoder(){

  //Lectura de Posición  

    int b = digitalRead(ENCODER_B);
    if(b > 0){
      //Incremento variable global
      theta++;
    }
    else{
      //Decremento variable global
      theta--;
    }
  }

// Rutina de interrupción para el canal B
void IRAM_ATTR encoderB() {
    if (digitalRead(ENCODERA) == HIGH) {
        posicion--;  // Dirección antihoraria
        direccion = -1;
    } else {
        posicion++;  // Dirección horaria
        direccion = 1;
    }
}
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