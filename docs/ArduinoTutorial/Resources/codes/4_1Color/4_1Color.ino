#include <Arduino.h>
#include <Sentry.h>  // Import the Sentry vision sensor library

typedef Sengo1 Sengo;  // Create an alias Sengo for the Sengo1 type to simplify subsequent usage

// Select the communication method (enable one by uncommenting)
#define SENGO_I2C  // I2C communication is currently enabled
// #define SENGO_UART   // Alternative option: UART serial communication

#ifdef SENGO_I2C
#include <Wire.h>  // Libraries required for I2C communication
#endif

#ifdef SENGO_UART
#include <SoftwareSerial.h>               // Soft serial port library (for non-hardware serial ports)
#define TX_PIN 11                         // Customize the TX pin
#define RX_PIN 10                         // Customize the RX pin
SoftwareSerial mySerial(RX_PIN, TX_PIN);  // Create a soft serial port object
#endif

#define VISION_TYPE Sengo::kVisionColor  // Define the type of visual algorithm (color recognition)
Sengo sengo;                             // Create a Sengo sensor object

void setup() {
  sentry_err_t err = SENTRY_OK;  // Error status variable

  Serial.begin(9600);  // Initialize the serial port for debugging the output
  Serial.println("Waiting for sengo initialize...");

  // Initialize the sensor according to the selected communication method
#ifdef SENGO_I2C
  Wire.begin();  // Initialize I2C bus
  // Keep trying to connect until done
  while (SENTRY_OK != sengo.begin(&Wire)) {
    yield();  // Allow other tasks to run while waiting
  }
#endif

#ifdef SENGO_UART
  mySerial.begin(9600);  // Initialize the soft serial port
  while (SENTRY_OK != sengo.begin(&mySerial)) {
    yield();
  }
#endif

  sentry_object_t param;  // Parametric structure
  Serial.println("Sengo begin Success.");
  //Set the x-coordinate of the detection area
  param.x_value = 50;
  //Set the y-coordinate of the detection area
  param.y_value = 50;
  //Set the width of the detection area
  param.width = 20;
  //Set the height of the detection area
  param.height = 20;
  //Write the parameters into the sensor
  err = sengo.SetParam(VISION_TYPE, &param);
  //Error handling
  if (err) {
    Serial.print("sengo.SetParam ");
    if (err) {
      Serial.print("Error: 0x");
    } else {
      Serial.print("Success: 0x");
    }
    Serial.println(err, HEX);  // Print hexadecimal error code
    for (;;)
      ;  // Infinite loop blocking (manual restart required)
  }

  // Enable the visual recognition algorithm
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVisionColor) ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);  // Output the initialization result
}

void loop() {
  // Read the number of detected objects (kStatus is the acquisition status)
  int obj_num = sengo.GetValue(VISION_TYPE, kStatus);

  if (obj_num > 0) {  // If an object is detected
    Serial.print("Totally ");
    Serial.print(obj_num);
    Serial.println(" objects");
    // Obtain the label (color ID) of the object
    int label = sengo.GetValue(VISION_TYPE, kLabel, 1);
    Serial.print('|');
    Serial.print(label);    // Output label (such as |1|3|5|)
    Serial.println("|");  // End mark
  }
  delay(500);  // Delay 500ms to reduce the refresh rate
}