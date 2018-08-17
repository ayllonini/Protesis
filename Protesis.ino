/*Declaracion de Variables de ServoMotor*/
#include <Servo.h>
Servo WristServo; //Servo muñeca
int WristCentral = 100; // ServoMotor angle setting
const int PinWrist = 3; //Pin muñeca configurado en el pin 3
/*Declarion de pines de motor DC*/
#define MOT_A1_PIN 5 //movimiento de pulgar
#define MOT_A2_PIN 4 //movimiento de pulgar
#define MOT_B1_PIN 9 //movimiento de meñique y anular
#define MOT_B2_PIN 8 //movimiento de meñique y anular
/*Declaracion de pines de botones*/
int ButtonConfig = 6; // configurar el boton en el pin 4
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0; // current state of the button
int lastButtonState = 0;     // previous state of the button
/*Entrada de sensor*/
const int analogInPin0  = A0;
int sensorValue = 0;  // value read from the sensor
int sensorMax = 0; // variable Maxima del sensor para efectuar movimiento
/*Se declaran las variables de abierto y cerrado*/
int OPEN = 1; 
int CLOSED = 0;
int bandera = 1;
int modoAbierto = 0;
int OPEN2 = 1;
int CLOSED2 = 0;
int bandera2 = 1;
int modoAbierto2 = 0;
int OPEN3 = 1; //Variable para el servo
int CLOSED3 = 0;
int bandera3 = 1;
/*Declaraciones de led*/
int PinRojo = 10;
int PinAzul = 12;
int PinVerde = 11;
void setup() {
  Serial.begin(9600); 
  pinMode(MOT_A1_PIN, OUTPUT); pinMode(MOT_A2_PIN, OUTPUT);// Configurar como salida el pin del pulgar
  pinMode(MOT_B1_PIN, OUTPUT); pinMode(MOT_B2_PIN, OUTPUT);// Configurar como salida el pin del meñique
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW); digitalWrite(MOT_B2_PIN, LOW);
  WristServo.attach(PinWrist); // Configurar el servo de la muñeca en el pin3
  pinMode(ButtonConfig,INPUT); // Configurar el boton de configuracion en el pin 4
  /*Declarar variables de LED como salida*/
  pinMode(PinRojo, OUTPUT);
  pinMode(PinAzul, OUTPUT);
  pinMode(PinVerde, OUTPUT);
}//setup

void loop(){
   delay(1000); //Dormir 1s para evitar los ruidos.
   while (1){//Bucle de Calibracion
      
      sensorValue = lecturaSensor();
      Serial.println(sensorValue);
      
      if(sensorValue > sensorMax)
      {
       sensorMax =  sensorValue;
      }
      
      buttonState = digitalRead(ButtonConfig); // Lectura del pulsador
      if (buttonState != lastButtonState) {
         if (buttonState == HIGH) { 
          Serial.println("on");
            
        } else {
          Serial.println("off");
          //Serial.println(sensorMax); //Imprimir Variable maxima
          WristServo.write(WristCentral); // Posicionar la muñeca 
         
          break;
        }
      }
      lastButtonState = buttonState;
  }
  while (1){ 
          
      buttonState = digitalRead(ButtonConfig); // Lectura del pulsador
      if (buttonState != lastButtonState) { 
        if (buttonState == HIGH) { 
          buttonPushCounter++;
          if(buttonPushCounter == 5){
            buttonPushCounter= 1;
          }
          Serial.println("Encendido");
        } else {
          Serial.println("Apagado");
        }
          delay(50);
      }
      lastButtonState = buttonState;  // guarde el estado actual como último estado, para la próxima vez a través del ciclo
      
      sensorValue = lecturaSensor();
      
      if (buttonPushCounter == 1) {// condicion de mover todos los dedos
        setColor(255, 0, 0);
        if(modoAbierto2 == 1){
           Serial.println("Abertura de modo pinza de la mano, modoAbierto = 1");
           AberturaPinza();
           modoAbierto2 = 0;
           bandera2 = OPEN2;
         }else if (modoAbierto2 == 0){
              if(bandera == OPEN && sensorValue >= sensorMax){
                Serial.println("Cierre total de la mano");
                cierreMano();
                bandera = CLOSED;
                modoAbierto = 1;
              }else if( bandera == CLOSED && sensorValue > sensorMax){
                Serial.println("Abertura de total de la mano");
                AberturaMano();
                bandera = OPEN;
                modoAbierto = 0;
                //modoAbierto = 1;
              }
         }
      }else if (buttonPushCounter == 2){ //condicion de mover la muñeca.
         setColor(0, 0, 255); // blue
         if(sensorValue >= sensorMax & bandera3 == OPEN3){
            Serial.print("Giro de muñeca");
            servoGiro();
            delay(50);
            bandera3 = CLOSED3;
         }else if(sensorValue >= sensorMax & bandera3 == CLOSED3){
            Serial.print("Posicion normal de la muñeca");
            servoVuelta();
            delay(50);
            bandera3 = OPEN3;
         }
      }else if (buttonPushCounter == 3){ //condicion de mover los dedos meñique y anular.
         setColor(0, 255, 0); // green
         if(modoAbierto == 1){
           Serial.println("Abertura total de la mano en condicion boton 2");
           AberturaMano();
           modoAbierto = 0;
           bandera = OPEN;
         }else if (modoAbierto == 0){
            if(sensorValue >= sensorMax & bandera2 == OPEN2){
                Serial.println("Cierre modo pinza");
                CierrePinza();
                bandera2 = CLOSED2;
                modoAbierto2 = 1;
               // modoAbierto = 1;
            }else if(sensorValue >= sensorMax & bandera2 == CLOSED2){
              Serial.println("Abertura modo pinza");
              AberturaPinza();
              bandera2 = OPEN2;
              modoAbierto2 = 0;
             // modoAbierto = 1;
            }
         }   
      } else if (buttonPushCounter == 4){ //condicion de mover la muñeca.
         setColor(0, 0, 255); // blue
         if(sensorValue >= sensorMax & bandera3 == OPEN3){
            Serial.print("Giro de muñeca");
            servoGiro();
            delay(50);
            bandera3 = CLOSED3;
         }else if(sensorValue >= sensorMax & bandera3 == CLOSED3){
            Serial.print("Posicion normal de la muñeca");
            servoVuelta();
            delay(50);
            bandera3 = OPEN3;
         }
      }
      Impresion();
   }//while
}//loop

//Lectura del sensor
int lecturaSensor() { 
  int i;
  int sval;
  
  for (i = 0; i < 20; i++) {
    sval = sval + abs(analogRead(analogInPin0));  
    
  }
  sval = sval / 20;
  return sval;
}

/*Seleccion de color*/
void setColor(int red, int green, int blue) {
  analogWrite(PinRojo, red);
  analogWrite(PinVerde, green);
  analogWrite(PinAzul, blue);
}
/*Gito de la muñeca*/
void servoGiro(){
 for (int i = 99; i <= 160; i++)
  {
    WristServo.write(i); 
    delay(25);
  }
}
/*Posicion normal de la muñeca*/
void servoVuelta(){
 for (int i = 161; i >= 100; i--)
  {
    WristServo.write(i); 
    delay(25);
  }
}


/*Cierre de mano total*/
void cierreMano(){
  digitalWrite(MOT_A1_PIN, HIGH); digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, HIGH); digitalWrite(MOT_B2_PIN, LOW);
  delay(1800);
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW); digitalWrite(MOT_B2_PIN, LOW);
}

/*Abertura de mano*/
void AberturaMano(){
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, HIGH);
  digitalWrite(MOT_B1_PIN, LOW); digitalWrite(MOT_B2_PIN, HIGH);
  delay(1600);
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW); digitalWrite(MOT_B2_PIN, LOW);
}

/*Cierre de Pinza*/
void CierrePinza(){
 digitalWrite(MOT_A1_PIN, HIGH); digitalWrite(MOT_A2_PIN, LOW);
 delay(1800); 
 digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, LOW);
}

/*Abertura de Pinza*/
void AberturaPinza(){
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, HIGH);
  delay(1600);
  digitalWrite(MOT_A1_PIN, LOW); digitalWrite(MOT_A2_PIN, LOW);
}

void Impresion(){
  Serial.print("Boton: "); Serial.print(buttonPushCounter);
  Serial.print("; Sensor: "); Serial.println(sensorValue);
}
