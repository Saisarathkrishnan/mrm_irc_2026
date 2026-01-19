#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BNO08x.h>
#include <math.h>

// adress
#define BNO055_ADDR 0x28
#define BNO08X_ADDR 0x4A

Adafruit_BNO055 bno055 = Adafruit_BNO055(55, BNO055_ADDR);
Adafruit_BNO08x bno08x(BNO08X_ADDR);

sh2_SensorValue_t sensorValue;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  delay(1000);

  // BNO055
  if (!bno055.begin()) {
    while (1);
  }
  bno055.setExtCrystalUse(true);

  // BNO08X
  if (!bno08x.begin_I2C(BNO08X_ADDR)) {
    while (1);
  }

  // Rotation vector gives fused orientation
  bno08x.enableReport(SH2_ROTATION_VECTOR, 5000);
}

void loop() {

  // -------- BNO055 Euler --------
  sensors_event_t event;
  bno055.getEvent(&event);

  float yaw055   = event.orientation.x; 
  float pitch055 = event.orientation.y;
  float roll055  = event.orientation.z;

  float yaw08x = 0, pitch08x = 0, roll08x = 0;

  if (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {

      float qw = sensorValue.un.rotationVector.real;
      float qx = sensorValue.un.rotationVector.i;
      float qy = sensorValue.un.rotationVector.j;
      float qz = sensorValue.un.rotationVector.k;

      roll08x = atan2(2.0 * (qw*qx + qy*qz),
                      1.0 - 2.0 * (qx*qx + qy*qy));

      pitch08x = asin(2.0 * (qw*qy - qz*qx));

      yaw08x = atan2(2.0 * (qw*qz + qx*qy),
                     1.0 - 2.0 * (qy*qy + qz*qz));

      roll08x  *= 180.0 / M_PI;
      pitch08x *= 180.0 / M_PI;
      yaw08x   *= 180.0 / M_PI;
    }
  }

  Serial.print("yl1_");
  Serial.print(yaw055);   
  Serial.print("_");
  Serial.print(pitch055); 
  Serial.print("_");
  Serial.print(roll055);
  Serial.print("_l2_");
  Serial.print(yaw08x);   
  Serial.print("_");
  Serial.print(pitch08x); 
  Serial.print("_");
  Serial.println(roll08x);



  delay(50);
}
