#include <Arduino.h>
#include "IMU.h"

#define ONBOARD_LED  2

const float accelerationThreshold = 25; // threshold of significant in G's // 25 voor punch // 40 voor wave
const int numSamples = 95; // 95 maal 20 msec = 1900 ms // de andere 100 ms gaat naar ledje

int samplesRead = numSamples;
int samples;

void setup() {
  Serial.begin(115200);
  while (!Serial);

int status1 = imu.begin();
  if (status1<0) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

/* Set sample rate divider for 50 Hz */
bool status = imu.setSrd(19);
if (!status) {
  // ERROR
}
 
// setup led
  pinMode(ONBOARD_LED,OUTPUT);

samples =1;

  // print the header
  Serial.println("aX,aY,aZ,gX,gY,gZ");
}

void loop() {

  digitalWrite(ONBOARD_LED,HIGH);
  delay(100);
  digitalWrite(ONBOARD_LED,LOW);
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  while (samplesRead == numSamples) {
      // read the acceleration data
      imu_read(&aX, &aY, &aZ, &gX, &gY, &gZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
    
      // read the acceleration and gyroscope data
      
      imu_read(&aX, &aY, &aZ, &gX, &gY, &gZ);
      
      samplesRead++;

      // print the data in CSV format
      Serial.print(aX, 3);
      Serial.print(',');
      Serial.print(aY, 3);
      Serial.print(',');
      Serial.print(aZ, 3);
      Serial.print(',');
      Serial.print(gX, 3);
      Serial.print(',');
      Serial.print(gY, 3);
      Serial.print(',');
      Serial.print(gZ, 3);
      Serial.println();

      if (samplesRead == numSamples) {
        // add an empty line if it's the last sample
        Serial.println();
      }
      delay(20);
    }
    Serial.println(samples);
    samples ++;
  }
