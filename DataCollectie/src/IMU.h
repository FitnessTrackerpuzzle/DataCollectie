#include "Wire.h"
// library from https://github.com/bolderflight/MPU9250
#include "MPU9250.h"
MPU9250 imu(Wire, 0x68);
#define NUM_AXES 6
#define ACCEL_THRESHOLD 5
#define INTERVAL 10
#define NUM_SAMPLES 30
// sometimes you may get "spikes" in the readings
// set a sensible value to truncate too large values
#define TRUNCATE_AT 20

double features[NUM_SAMPLES * NUM_AXES];

double baseline[NUM_AXES];

void imu_setup() {
    Wire.begin();
    imu.begin();
}

void imu_read(float *ax, float *ay, float *az,float *gX, float *gY, float *gZ) {
    imu.readSensor();

    *ax = imu.getAccelX_mss();
    *ay = imu.getAccelY_mss();
    *az = imu.getAccelZ_mss();
    *gX = imu.getGyroX_rads();
    *gY = imu.getGyroY_rads();
    *gZ = imu.getGyroZ_rads();
}

void calibrate(){
    float ax, ay, az, gx, gy, gz;

    for (int i=0; i <10;i++){
    imu_read(&ax, &ay, &az, &gx, &gy, &gz);
    delay(100);
    }

    baseline[0]= ax;
    baseline[1]= ay;
    baseline[2] = az;
    baseline[3] = gx;
    baseline[4] = gy;
    baseline[5] = gz;

    Serial.println("Calibration ready");
}

bool motionDetected(float ax, float ay, float az){
    return(abs(ax) + abs(ay) + abs(az))> ACCEL_THRESHOLD;
}

void recordIMU() {
    float ax, ay, az, gx, gy, gz;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        imu_read(&ax, &ay, &az, &gx, &gy, &gz);

    ax = constrain(ax-baseline[0], -TRUNCATE_AT, TRUNCATE_AT);
    ay = constrain(ay-baseline[1], -TRUNCATE_AT, TRUNCATE_AT);
    az = constrain(az-baseline[2], -TRUNCATE_AT, TRUNCATE_AT);
    gx = gx-baseline[3];
    gy = gy-baseline[4];
    gz = gz-baseline[5];


        features[i * NUM_AXES + 0] = ax;
        features[i * NUM_AXES + 1] = ay;
        features[i * NUM_AXES + 2] = az;
        features[i * NUM_AXES + 3] = gx;
        features[i * NUM_AXES + 4] = gy;
        features[i * NUM_AXES + 5] = gz;
        delay(INTERVAL);
    }
}

void printFeatures() {
    const uint16_t numFeatures = sizeof(features) / sizeof(double);

    for (int i = 0; i < numFeatures; i++) {
        if((i+1)%6==0){
            Serial.println(features[i]);
        }
        else{
          Serial.print(features[i]);  
          Serial.print(',');
        }
       // Serial.print(features[i]);
        
        //Serial.print(i == numFeatures - 1 ? 'n' : ',');
    }
}




