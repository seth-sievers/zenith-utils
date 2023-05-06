#include <Arduino.h>
#include <Adafruit_LSM6DSOX.h>
#include <SD.h>
#include <SPI.h>

/* ---------------------------- GLOBAL_VARIABLES ---------------------------- */
Adafruit_LSM6DSOX sox; 
volatile int counts = 0; 
int lastCount = 0;
volatile long unsigned int lastCountMS = 0;  
long unsigned int lastLoopMS = 0; 
const int chipSelect = BUILTIN_SDCARD; 
int state = 0; 
long unsigned int ledTurnOnMS = 0; 
/* -------------------------------------------------------------------------- */

/* --------------------------- FUNCTION_PROTOTYPES -------------------------- */
void geigerPulse(); 
/* -------------------------------------------------------------------------- */

/* ---------------------------------- SETUP --------------------------------- */
void setup()
{
        // setup serial comms
        Serial.begin(9600);
        while (!Serial) {;} 

        //initialize imu
        if (!sox.begin_I2C()){
                while (true) {
                        Serial.println("Failed to Find Accelerometer / Gyro chip");
                        delay(50); 
                }
        } else {
                Serial.println("Accelerometer / Gyro Found");
        }

        //set accel range to 16G and gyro to 2000dps
        sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
        if (sox.getAccelRange() == LSM6DS_ACCEL_RANGE_16_G) { Serial.println("Accelerometer set to 16G range"); }
        sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS );
        if (sox.getGyroRange() == LSM6DS_GYRO_RANGE_2000_DPS) { Serial.println("Gyro set to 2000dps"); }

        //set data output rate
        sox.setAccelDataRate(LSM6DS_RATE_52_HZ);
        if (sox.getAccelDataRate() == LSM6DS_RATE_52_HZ) { Serial.println("Accelerometer set to 52hz"); }
        sox.setGyroDataRate(LSM6DS_RATE_52_HZ);
        if (sox.getGyroDataRate() == LSM6DS_RATE_52_HZ) { Serial.println("Gyro set to 52hz"); }

        //initialize SD card
        if (!SD.begin(chipSelect)) {
                while (true) {
                        Serial.println("SD Card failed or not present");
                        delay(50); 
                }
        } else {
                Serial.println("SD Card Initialized"); 
        }

        //setup Geiger Interrupt 
        pinMode(13, OUTPUT);
        pinMode(23, INPUT);
        attachInterrupt(digitalPinToInterrupt(23), geigerPulse, RISING);
        digitalWrite(13, LOW);
        counts = 0; //start out with no pulses

        //write data start string to sd card
        File dataFile = SD.open("data.csv", FILE_WRITE);
        dataFile.println("START OF NEW DATA"); 
        dataFile.println("millis, accel-x, accel-y, accel-z, gyro-x, gyro-y, gyro-z, temp, counts");
        dataFile.close(); 

        Serial.println("Beginning Data Logging"); 
}
/* -------------------------------------------------------------------------- */

/* ---------------------------------- LOOP ---------------------------------- */
void loop()
{
        // we want a 50hz loop -> 20ms
        if ((millis() - lastLoopMS) > 100){
                lastLoopMS = millis(); 

                //open SD for writing
                File dataFile = SD.open("data.csv", FILE_WRITE); 

                // construct a string that will be written to a new line of sd card
                String dataString = ""; 

                //read data from accel / gyro 
                sensors_event_t accel;
                sensors_event_t gyro; 
                sensors_event_t temp; 
                sox.getEvent(&accel, &gyro, &temp); 
                
//SD FORMAT: millis, accel x, accel y, accel z, gyro x, gyro y, gyro z, temp, counts
                // add data to dataString
                dataString += String(millis());
                dataString += ("," + String(accel.acceleration.x));
                dataString += ("," + String(accel.acceleration.y));
                dataString += ("," + String(accel.acceleration.z));
                dataString += ("," + String(gyro.gyro.x));
                dataString += ("," + String(gyro.gyro.y));
                dataString += ("," + String(gyro.gyro.z));
                dataString += ("," + String(temp.temperature));
                dataString += ("," + String(counts)); 
                counts = 0; //after recording counts reset to zero 

                //write to sd card and close
                dataFile.println(dataString);
                dataFile.close(); 
        }


        //flash red led to display count
        switch (state)
        {
                case 0:
                        if (counts > lastCount) {
                                digitalWrite(13, HIGH); 
                                state = 1; 
                                ledTurnOnMS = millis(); 
                        }
                        break;
                case 1:
                        if ((millis() - ledTurnOnMS) > 25){
                                digitalWrite(13, LOW); 
                                state = 0; 
                        }
                        break; 
        }
}
/* -------------------------------------------------------------------------- */

/* ------------------------------- geigerPulse ------------------------------ */
void geigerPulse()
{
        //increment the count variable and then perform some basic debouncing 
        if ((millis() - lastCountMS) > 7) {
                counts++; 
                lastCountMS = millis(); 
        }
        return; 
}
/* -------------------------------------------------------------------------- */

