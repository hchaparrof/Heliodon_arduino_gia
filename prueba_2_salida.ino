// definiciones funcionamiento brazo
const int MILLIS_ESPERA = 100;
const int PULSOS = 5;
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
const int CARRO_MAS = 36; 
const int CARRO_MENOS = 34;
const int frecuencia_carro = 1
const float prop_carro = 0.5
////arco
const int ARCO_MAS = 35;
const int ARCO_MENOS = 39;
//// encoder
const int ENCODER_A = 1;
const int ENCODER_B = 1;
////pot
const int POT_ARCO = 1;
////emergencia
const int EMERGENCIA = 1;
////botones 
const int BOT_CARR_IZQ = 1;
const int BOT_CARR_DER = 1;
const int BOT_ARC_IZQ = 1;
const int BOT_ARC_DER = 1;
///////
///////salidas
////paso a paso brazo
const int ENABLE = 1;
const int DIRECCION = 1;
const int PULSO = 1;
////carro motor normal
const int DIR_MAS = 1;
const int DIR_MENOS = 1;
const int PWM = 1;
////regulador luz
const int REGULADOR_LUZ = 1;
const int AUX_CEL = 1;

// El LED integrado de la ESP32 está en el pin 2
const int ledPin = 2;
long int milis_actuales = 0;
long int milis_acumulados = 0;
bool funcionando = false;
bool ahora = false;
int contador = 0;
int pulsos_dados = 0;
void setup() {
  // Inicia la comunicación serial a 9600 baudios
  Serial.begin(9600);
  
  // Configura el pin del LED como salida
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRECCION, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(PULSO, OUTPUT);
  digitalWrite(PULSO, LOW);
  digitalWrite(ENABLE, HIGH);
  // Apaga el LED al inicio
  digitalWrite(ledPin, LOW);
}

void loop() {
  millis_actual = millis();
  delta_time = millis_actual - millis_anterior;
  millis_anterior = millis_actual;
  if(funcionando){
    acumulado += delta_time;
    if(acumulado > MILLIS_ESPERA){
      acumulado = 0;
      digitalWrite(PULSO, !digitalRead(PULSO));
      if(digitalRead(PULSO)){
        pulsos_acumulados++;
        if(pulsos_acumulados > PULSOS){
          pulsos_acumulados = 0;
          funcionando = 0;
        }
      }
    }
  }

  if (ahora){
    contador = 0;
    ahora = false;
  }else{
    ++contador;
  }
  if (contador>250000){
    ahora = true;
  }
  // Verifica si hay datos disponibles en el puerto serial
  if (Serial.available() > 0) {
    Serial.println("hola");
    // Lee el dato del puerto serial
    String inputString = Serial.readStringUntil('\n');
    
    // Convierte la cadena de entrada a un número entero
    int inputNumber = inputString.toInt();
    Serial.println(inputString);
    // Verifica si el número coincide con el esperado
    if (inputNumber == 4 || inputNumber == 6) {
      if (!digitalRead(ENABLE)){
        digitalWrite(ENABLE, HIGH);
      }
      funcionando = true;
      digitalWrite(ENABLE, HIGH);
      if(inputNumber == 4){
        digitalWrite(DIRECCION, HIGH);
      }else{
        digitalWrite(DIRECCION, LOW);
      }
      // Enciende el LED si el número es correcto
      digitalWrite(ledPin, HIGH);
    } else {
      // Apaga el LED si el número no es correcto
      digitalWrite(ledPin, LOW);
    }
  }
}
