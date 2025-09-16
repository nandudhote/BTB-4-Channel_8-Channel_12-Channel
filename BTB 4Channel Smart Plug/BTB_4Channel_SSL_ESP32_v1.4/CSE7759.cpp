#include "CSE7759.h"  // Include the CSE7759 class header
#include "Config.h"   // Include configuration file for global variables/constants

// Constructor for the CSE7759 class
CSE7759::CSE7759() {
}

// Function to initialize the PowerData structure with default values
void CSE7759::initialize_power_data(PowerData* data) {
  memset(data, 0, sizeof(PowerData));  // Set all fields of PowerData to 0
  data->test_flag = 0;                 // Reset test_flag
  data->checksum = 255;                // Initialize checksum with default 255
}

// Function to convert a hex string into decimal value
int CSE7759::hex_to_dec(const char* hex) {
  long long decimal = 0;      // Variable to hold decimal conversion
  int len = strlen(hex) - 1;  // Get string length - 1 for power calculation

  // Loop through each character of the hex string
  for (int i = 0; hex[i] != '\0'; i++) {
    int val;  // Temporary variable for digit
    if (hex[i] >= '0' && hex[i] <= '9') {
      val = hex[i] - '0';  // Convert char '0'-'9' to 0-9
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {
      val = hex[i] - 'a' + 10;  // Convert char 'a'-'f' to 10-15
    } else if (hex[i] >= 'A' && hex[i] <= 'F') {
      val = hex[i] - 'A' + 10;  // Convert char 'A'-'F' to 10-15
    } else {
      return -1;  // Invalid hex char → return -1
    }
    decimal += val * pow(16, len);  // Multiply digit by base-16 power
    len--;                          // Decrease power counter
  }
  return decimal;  // Return final decimal value
}

// Function to prepare a hex string from 3 bytes
void CSE7759::prepare_hex(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex) {
  // Different cases depending on whether the byte < 16 (needs leading 0)
  if (byte1 < 16 && byte2 < 16 && byte3 < 16) {
    sprintf(hex, "0%x0%x0%x", byte1, byte2, byte3);  // All three need 0-padding
  } else if (byte1 < 16) {
    sprintf(hex, "0%x%x%x", byte1, byte2, byte3);  // Only byte1 < 16
  } else if (byte2 < 16) {
    sprintf(hex, "%x0%x%x", byte1, byte2, byte3);  // Only byte2 < 16
  } else if (byte3 < 16) {
    sprintf(hex, "%x%x0%x", byte1, byte2, byte3);  // Only byte3 < 16
  } else {
    sprintf(hex, "%x%x%x", byte1, byte2, byte3);  // No padding needed
  }
}

// Function to calculate voltage, current, power from readings
void CSE7759::calculate_power(PowerData* data) {
  char temp_hex[10];          // Temporary buffer for hex string
  float temp1, temp2, temp3;  // Temporary variables for division

  // Voltage calculation
  prepare_hex(data->readings[2], data->readings[3], data->readings[4], temp_hex);  // Prepare numerator
  temp1 = hex_to_dec(temp_hex);                                                    // Convert numerator
  prepare_hex(data->readings[5], data->readings[6], data->readings[7], temp_hex);  // Prepare denominator
  temp2 = hex_to_dec(temp_hex);                                                    // Convert denominator
  data->voltage = data->test_flag ? 0 : temp1 / temp2;                             // Assign voltage

  // Current calculation
  prepare_hex(data->readings[8], data->readings[9], data->readings[10], temp_hex);    // Prepare numerator
  temp1 = hex_to_dec(temp_hex);                                                       // Convert numerator
  prepare_hex(data->readings[11], data->readings[12], data->readings[13], temp_hex);  // Prepare denominator
  temp2 = hex_to_dec(temp_hex);                                                       // Convert denominator
  data->current = data->test_flag ? 0 : temp1 / temp2;                                // Assign current

  // Power calculation
  prepare_hex(data->readings[14], data->readings[15], data->readings[16], temp_hex);  // Prepare numerator
  temp1 = hex_to_dec(temp_hex);                                                       // Convert numerator
  prepare_hex(data->readings[17], data->readings[18], data->readings[19], temp_hex);  // Prepare denominator
  temp2 = hex_to_dec(temp_hex);                                                       // Convert denominator
  data->power = data->test_flag ? 0 : temp1 / temp2;                                  // Assign power

  data->test_flag = 0;  // Reset flag
}

// Function to read raw power data packet from UART
PowerData CSE7759::read_power_data() {
  PowerData data;                // Create PowerData struct
  initialize_power_data(&data);  // Reset values
  unsigned char byte;            // Variable to hold received byte
  unsigned int Checksum = 0;     // Checksum variable (unused)

  // Loop up to 100 times to read data
  for (int i = 0; i <= 100; i++) {
    byte = Serial1.read();        // Read one byte from UART
    if (byte == 85) {             // Check for sync byte 0x55
      data.readings[0] = byte;    // Store it
      byte = Serial1.read();      // Read next byte
      if (byte == 90) {           // Check for sync byte 0x5A
        data.readings[1] = byte;  // Store it
        for (int j = 2; j <= 23; j++) {
          data.readings[j] = Serial1.read();  // Read remaining packet bytes
        }
        // for (int x = 2; x <= 22; x++) {
        //   Checksum += data.readings[x];
        // }
        // if ((Checksum & data.checksum) == data.readings[23]) {
        calculate_power(&data);  // Compute voltage/current/power
        return data;             // Return processed data
        // }
      }
    }
  }
  return data;  // If not found, return initialized data
}

// AMit

// Add a new sample into the buffer
void CSE7759::addSamplesForPower(float value) {
  samplesForPowerCSE1[indexForPowerCSE1] = value;
  indexForPowerCSE1 = (indexForPowerCSE1 + 1) % 10;  // wrap around (circular buffer)
  if (indexForPowerCSE1 == 0) bufferFilledForPowerCSE1 = true;
}

// Calculate moving average
float CSE7759::getAverageForPower() {
  float sum = 0;
  int count = bufferFilledForPowerCSE1 ? 10 : indexForPowerCSE1;  // use only filled values
  for (int i = 0; i < count; i++) {
    sum += samplesForPowerCSE1[i];
  }
  return (count > 0) ? (sum / count) : 0;
}

// // Add a new sample into the buffer
// void CSE7759::addSamplesForCurrent(float value1) {
//   samplesForCurrentCSE1[indexForCurrentCSE1] = value1;
//   Serial.print("Sample : ");
//   Serial.println(samplesForCurrentCSE1[indexForCurrentCSE1]);
//   indexForCurrentCSE1 = (indexForCurrentCSE1 + 1) % 10;  // wrap around (circular buffer)
//   if (indexForCurrentCSE1 == 0) bufferFilledForCurrentCSE1 = true;
// }

// // Calculate moving average
// float CSE7759::getAverageForCurrent() {
//   float sum = 0;
//   int count = bufferFilledForCurrentCSE1 ? 10 : indexForCurrentCSE1;  // use only filled values
//   Serial.print("count ");
//   Serial.println(count);
//   for (int i = 0; i < count; i++) {
//     Serial.print("Sample ");
//     Serial.print(i);
//     Serial.print(" : ");
//     Serial.println(samplesForCurrentCSE1[i]);
//     sum += samplesForCurrentCSE1[i];
//   }
//   Serial.print("Avg : ");
//   Serial.println(sum / count);
//   return (count > 0) ? (sum / count) : 0;
// }

// // Add a new sample into the buffer
// void CSE7759::addSamplesForVoltage(float value2) {
//   samplesForVoltageCSE1[indexForVoltageCSE1] = value2;
//   indexForVoltageCSE1 = (indexForVoltageCSE1 + 1) % 10;  // wrap around (circular buffer)
//   if (indexForVoltageCSE1 == 0) bufferFilledForVoltageCSE1 = true;
// }

// // Calculate moving average
// float CSE7759::getAverageForVoltage() {
//   float sum = 0;
//   int count = bufferFilledForVoltageCSE1 ? 10 : indexForVoltageCSE1;  // use only filled values
//   for (int i = 0; i < count; i++) {
//     sum += samplesForVoltageCSE1[i];
//   }
//   return (count > 0) ? (sum / count) : 0;
// }

/*
  INPUT: VOLTAGE, CURRENT, POWER STRUCTURE AND RELAY STATUS FLAG
  OUTPUT: VERIFIED VOLTAGE, CURRENT, POWER STRUCTURE
  OPERATION: CHECKING IF RECEIEVED DATA IS GARBAGE. IF YES, THEN RE-REQUEST FOR DATA. TRYING IT FOR 5 TIMES, IF STILL GETTING GARBAGE DATA THEN RETURNING THE PREVIOUS CORRECT DATA.
*/
PowerData CSE7759::validateTheCSEChipData(PowerData powerData) {
  Serial.println("*****************************CSE1***********************");  // Print debug header

  float current_power = powerData.power;      // Local copy of current power
  float current_current = powerData.current;  // Local copy of current current
  float current_voltage = powerData.voltage;  // Local copy of current voltage

  // Replace invalid (infinite) readings with last good value
  if (isinf(powerData.power) || isinf(powerData.current) || isinf(powerData.voltage)) {
    if (isinf(powerData.power)) {
      powerData.power = powerPre;  // Use last valid power
    }
    if (isinf(powerData.current)) {
      powerData.current = currentPre;  // Use last valid current
    }
    if (isinf(powerData.voltage)) {
      powerData.voltage = voltagePre;  // Use last valid voltage
    }
  }

  addSamplesForPower(powerData.power);
  float avgForPower = getAverageForPower();
  // Validate power data (reject if >5x or <1/5 of last 3 readings)
  if ((powerData.power > (avgForPower + 100)) || (powerData.power < (avgForPower - 100))) {
    current_power = powerData.power;  // Save rejected value
    powerData.power = powerPre;       // Replace with last valid
    current_current = powerData.current;
    powerData.current = currentPre;
    current_voltage = powerData.voltage;  // Save rejected value
    powerData.voltage = voltagePre;       // Replace with last valid
    // Serial.print("current_power : ");
    // Serial.println(current_power);
    // Serial.print("current_current : ");
    // Serial.println(current_current);
    // Serial.print("current_voltage : ");
    // Serial.println(current_voltage);
  } else {
    powerPre = powerData.power;      // Update last valid power
    currentPre = powerData.current;  // Update last valid current
    voltagePre = powerData.voltage;  // Update last valid voltage
    // Serial.print("powerPre : ");
    // Serial.println(powerPre);
    // Serial.print("currentPre : ");
    // Serial.println(currentPre);
    // Serial.print("voltagePre : ");
    // Serial.println(voltagePre);
  }

  // addSamplesForCurrent(powerData.current);
  // float avgForCurrent = getAverageForCurrent();
  // // Validate current data
  // if ((powerData.current > (avgForCurrent + 0.5)) || (powerData.current < (avgForCurrent - 0.5))) {
  //   current_current = powerData.current;  // Save rejected value
  //   Serial.print("current_current : ");
  //   Serial.println(current_current);
  //   powerData.current = currentPre;  // Replace with last valid
  //   // Serial.print("powerData.current : ");
  //   // Serial.println(powerData.current);
  // } else {
  //   currentPre = powerData.current;  // Update last valid current
  //   Serial.print("currentPre : ");
  //   Serial.println(currentPre);
  // }

  // addSamplesForVoltage(powerData.voltage);
  // float avgForVoltage = getAverageForVoltage();
  // // Validate voltage data
  // if ((powerData.voltage > (avgForVoltage + 50)) || (powerData.voltage < (avgForVoltage - 50))) {
  //   current_voltage = powerData.voltage;  // Save rejected value
  //   Serial.print("current_voltage : ");
  //   Serial.println(current_voltage);
  //   powerData.voltage = voltagePre;  // Replace with last valid
  //   // Serial.print("powerData.voltage : ");
  //   // Serial.println(powerData.voltage);
  // } else {
  //   voltagePre = powerData.voltage;  // Update last valid voltage
  //   Serial.print("voltagePre : ");
  //   Serial.println(voltagePre);
  // }

  // Serial.print("Moving Power Avg : ");
  // Serial.println(avgForPower);
  // Serial.print("Moving Current Avg : ");
  // Serial.println(avgForCurrent);
  // Serial.print("Moving Voltage Avg : ");
  // Serial.println(avgForVoltage);

  // Return validated data structure
  return powerData;
}
