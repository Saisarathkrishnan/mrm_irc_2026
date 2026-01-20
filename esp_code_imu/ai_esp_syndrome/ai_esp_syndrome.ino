#include <HardwareSerial.h>
#include "arm_IK.h"
#include "driveM.h"
#include "driveA.h"

//---ESP32_PIN_CONFIG---
// Arm
#define dir_swivel  16
#define pwm_swivel  33
#define dir_link1   32
#define pwm_link1   14
#define dir_link2   13
#define pwm_link2   23

// Drive
#define Rdir  19
#define Ldir  18
#define Rpwm  27
#define Lpwm  26

// SERIAL
HardwareSerial SerialPort(1);  // use UART1
HardwareSerial Sender(0);      // default serial (UART0)

const int BUFFER_SIZE = 128;
char rxBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Pins
const int pwm_pin[] = { pwm_swivel, pwm_link1, pwm_link2, Rpwm, Lpwm };
const int dir_pin[] = { dir_swivel, dir_link1, dir_link2, Rdir, Ldir };

long prevT = 0;
int changeMode = 0;

// Arm variables
float linkLength1 = 39.8, linkLength2 = 37.5;
int L = 0, R = 0, T=0, U=0;
int freq = 8000, Lchannel = 0, Rchannel = 1, resolution = 8;

// Class instances
IK   ik;
DriveM driveM;
DriveA driveA;

const int freqLinks = 8000;     // 5 kHz frequency
const int ledChannelLink1 = 15;  // PWM channel (0-15)
const int resolutionLink1 = 8;

const int ledChannelLink2 = 14;  // PWM channel (0-15)
const int resolutionLink2 = 8;

void arm(int x, int y, int z, int Reset) {
  long currT = micros();
  float deltaT = (float)(currT - prevT) / 1.0e6;
  prevT = currT;
  ik.arm_control(x, y, z, deltaT, Reset);
}

void gripper(int grip) {
  switch (grip) {
    case 0: Sender.write('Q'); break;
    case 1: Sender.write('W'); break;
    case 2: Sender.write('E'); break;
    case 3: Sender.write('R'); break;
    case 4: Sender.write('T'); break;
    case 5: Sender.write('Y'); break;
    case 6: Sender.write('U'); break;
    case 7: Sender.write('I'); break;
    case 8: Sender.write('V'); break;
    case 9: Sender.write('B'); break;
    default: Sender.write('Q'); break;
  }
}


void setup() {
  Wire.begin();
  Serial.begin(115200);
  Sender.begin(115200, SERIAL_8N1, 1, 3);
  SerialPort.begin(115200, SERIAL_8N1, 2, 0);

  driveM.setDrivePins(dir_pin[3], dir_pin[4], pwm_pin[3], pwm_pin[4]);
  ik.setArmPins(dir_pin[0], pwm_pin[0],
                dir_pin[1], pwm_pin[1],
                dir_pin[2], pwm_pin[2]);


  pinMode(dir_link2, OUTPUT);
  ledcSetup(ledChannelLink1, freqLinks, resolutionLink1);
  ledcAttachPin(pwm_link2, ledChannelLink1);

  pinMode(dir_link1, OUTPUT);
  ledcSetup(ledChannelLink2, freqLinks, resolutionLink2);
  ledcAttachPin(pwm_link1, ledChannelLink2);

  Serial.println("Pins set!");
  delay(100);

  ik.set_link_length(linkLength1, linkLength2);
  ik.set_pid();
  Serial.println("PID set");
  delay(10);
}

void loop() {
   if (SerialPort.available()) {
    // Read incoming data
    while (SerialPort.available()) {
      rxBuffer[bufferIndex++] = (char)SerialPort.read();
      if (bufferIndex >= BUFFER_SIZE) bufferIndex = 0;
    }

    // Parse packet markers
    char *M_index = strchr(rxBuffer, 'M');
    char *x_index = strchr(rxBuffer, 'X');
    char *y_index = strchr(rxBuffer, 'Y');
    char *P_index = strchr(rxBuffer, 'P');
    char *Q_index = strchr(rxBuffer, 'Q');
    char *A_index = strchr(rxBuffer, 'A');
    char *S_index = strchr(rxBuffer, 'S');
    char *J_index = strchr(rxBuffer, 'J');
    char *D_index = strchr(rxBuffer, 'D');
    char *L_index = strchr(rxBuffer, 'L');
    char *R_index = strchr(rxBuffer, 'R');
    char *T_index = strchr(rxBuffer, 'T');
    char *U_index = strchr(rxBuffer, 'U');
    char *G_index = strchr(rxBuffer, 'G');
    char *Z_index = strchr(rxBuffer, 'Z');
    char *E_index = strchr(rxBuffer, 'E');

    // Check if packet has 'Z' == '0' (maybe you want to check value rather than pointer)
    if (Z_index != nullptr && *(Z_index + 1) == '0') {
      if (M_index && x_index && y_index && P_index && Q_index &&
          A_index && S_index && J_index && D_index && E_index) {

        int M = *(M_index + 1) - '0';
        int x = atoi(x_index + 1);
        int y = atoi(y_index + 1);
        int X = atoi(P_index + 1);
        if (abs(X) < 3) X = 0;
        int Y = atoi(Q_index + 1);
        if (abs(Y) < 3) Y = 0;
        int grip = atoi(A_index + 1);
        int Z = atoi(S_index + 1);
        if (abs(Z) < 3) Z = 0;
        int Reset = atoi(R_index + 1);
        int Mode = atoi(D_index + 1);

        driveM.MotorCode(x, y, M);
        if (Mode == 0) {
          if (changeMode == 1) {
            Serial.println("DRIVEEEEE");
            changeMode = 0;
          }
          driveM.MotorCode(x, y, M);
          ik.beginFK(X, Y, Z);
          gripper(grip);
        }
      } else {
        Serial.println("Invalid Packet received");
      }

    } else {  // Else assume driving left/right
      if (L_index) {
        L = atoi(L_index + 1);
      }
      if (R_index) {
        R = atoi(R_index + 1);
      }
      if(T_index){
        T=atoi(T_index+1);
      }
      if(U_index){
        U=atoi(U_index+1);
      }
      ledcWrite(Rchannel, abs(L) * 2.55);
      ledcWrite(Lchannel, abs(R) * 2.55);

      if (L > 0){


        digitalWrite(Ldir, LOW);
      }      
      else if (L < 0){ digitalWrite(Ldir, HIGH);


      }
      else            digitalWrite(Ldir, LOW);

      if (R > 0)      digitalWrite(Rdir, LOW);
      else if (R < 0) digitalWrite(Rdir, HIGH);
      else            digitalWrite(Rdir, LOW);

      if(T>0){
        digitalWrite(dir_link2, HIGH);
        ledcWrite(ledChannelLink1, 160); 
      }else if(T<0){
        digitalWrite(dir_link2, LOW);
        ledcWrite(ledChannelLink1, 160); 
      }else{
        digitalWrite(dir_link2, LOW);
        ledcWrite(ledChannelLink1, 0); 
      }

      if(U>0){
        digitalWrite(dir_link1, HIGH);
        ledcWrite(ledChannelLink2, 200); 
      }else if(U<0){
        digitalWrite(dir_link1, LOW);
        ledcWrite(ledChannelLink2, 200); 
      }else{
        digitalWrite(dir_link1, LOW);
        ledcWrite(ledChannelLink2, 0); 
      }
      /*
      if(G>0){
        //gripper open
        gripper(5);
      }else if(G<0{
        //gripper close
        gripper(6);
      }else{
        gripper(0);
        //gripper nothing
      }
      
      */


    }

    bufferIndex = 0;
  }
}