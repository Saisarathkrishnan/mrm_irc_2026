#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BNO08x.h>
#include <utility/imumaths.h>


#define BNO055_ADDR 0x28
#define BNO08X_ADDR 0x4A
/* -------- SECOND I2C BUS -------- */
TwoWire I2C_2 = TwoWire(1);

/* Attach BNO055 to I2C_2 */
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &I2C_2);

Adafruit_BNO055 bno055 = Adafruit_BNO055(55, BNO055_ADDR);
Adafruit_BNO08x bno08x(BNO08X_ADDR);

sh2_SensorValue_t sensorValue;

bool calib = false;

void displayCalStatus() {
  uint8_t sys, gyro, accel, mag;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  Serial.printf("1calibrate → SYS:%d G:%d A:%d M:%d\n", sys, gyro, accel, mag);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  /* Start second I2C bus */
  I2C_2.begin(16, 17);     // SDA, SCL
  I2C_2.setClock(100000);  // BNO055 likes 100kHz
  Wire.begin(21, 22);
  delay(1000);

  if (!bno.begin()) {
    Serial.println("BNO055 not connected");
    while (1)
      ;
  }
  if (!bno055.begin()) {
    while (1)
      ;
  }
  bno055.setExtCrystalUse(true);
  if (!bno08x.begin_I2C(BNO08X_ADDR)) {
    while (1)
      ;
  }

  // Rotation vector gives fused orientation
  bno08x.enableReport(SH2_ROTATION_VECTOR, 5000);


  bno.setExtCrystalUse(true);
  bno.setMode(OPERATION_MODE_NDOF);

  Serial.println("\n1_360 no scope with sensor to calibrate");
}

void loop() {
  uint8_t sys, gyro, accel, mag;

  if (!calib) {
    displayCalStatus();
    bno.getCalibration(&sys, &gyro, &accel, &mag);
  }

  if (gyro == 3 && mag == 3 && !calib) {
    Serial.println("\n1calibrated");

    adafruit_bno055_offsets_t offsets;
    bno.getSensorOffsets(offsets);
    bno.setSensorOffsets(offsets);

    Serial.println("1Calibration saved");
    calib = true;
  }

  if (calib) {
    imu::Vector<3> euler =
      bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    imu::Vector<3> eulerbno2 =
      bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
    float yaw08x = 0, pitch08x = 0, roll08x = 0;
    if (bno08x.getSensorEvent(&sensorValue)) {
      if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {

        float qw = sensorValue.un.rotationVector.real;
        float qx = sensorValue.un.rotationVector.i;
        float qy = sensorValue.un.rotationVector.j;
        float qz = sensorValue.un.rotationVector.k;

        roll08x = atan2(2.0 * (qw * qx + qy * qz),
                        1.0 - 2.0 * (qx * qx + qy * qy));

        pitch08x = asin(2.0 * (qw * qy - qz * qx));

        yaw08x = atan2(2.0 * (qw * qz + qx * qy),
                       1.0 - 2.0 * (qy * qy + qz * qz));

        roll08x *= 180.0 / M_PI;
        pitch08x *= 180.0 / M_PI;
        yaw08x *= 180.0 / M_PI;
      }
      
    Serial.print("yl1_");
    Serial.print(eulerbno2.x(), 2);
    Serial.print("_");
    Serial.print(eulerbno2.y(), 2);
    Serial.print("_");
    Serial.print(eulerbno2.z(), 2);
    Serial.print("_l2_");
    Serial.print(yaw08x);
    Serial.print("_");
    Serial.print(pitch08x);
    Serial.print("_");
    Serial.print(roll08x);
    Serial.print("_rv_");
    Serial.print(yaw08x);
    Serial.print("_");
    Serial.print(pitch08x);
    Serial.print("_");
    Serial.print(roll08x);
    Serial.print("4 ");
    Serial.print(euler.x(),2);
    Serial.print("_");
    Serial.print(euler.y(),2);
    Serial.print("_");
    Serial.println(euler.z(),2);
    }

    delay(10);
  }

}
