#include "CSE7759_two.h"  // Include the header file for CSE7759_two class
#include "Config.h"       // Include configuration file for global settings/variables



CSE7759_two::CSE7759_two() {  // Constructor for the CSE7759_two class
}

void CSE7759_two::initialize_power_data_two(PowerData2* data) {
  memset(data, 0, sizeof(PowerData2));  // Clear the PowerData2 struct by filling it with zeros
  data->test_flag = 0;                  // Initialize test_flag to 0
  data->checksum = 255;                 // Set checksum default to 255
  data->ifDataIsOk = false;             // Mark data validity flag as false
}

int CSE7759_two::hex_to_dec_two(const char* hex) {
  long long decimal = 0;      // Store final decimal value
  int len = strlen(hex) - 1;  // Get last index of string

  for (int i = 0; hex[i] != '\0'; i++) {  // Loop through each hex character
    int val;
    if (hex[i] >= '0' && hex[i] <= '9') {  // If character is 0–9
      val = hex[i] - '0';
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {  // If character is a–f
      val = hex[i] - 'a' + 10;
    } else if (hex[i] >= 'A' && hex[i] <= 'F') {  // If character is A–F
      val = hex[i] - 'A' + 10;
    } else {
      return -1;  // Invalid hex character
    }
    decimal += val * pow(16, len);  // Convert hex digit to decimal
    len--;                          // Decrease exponent
  }
  return decimal;  // Return final decimal result
}

void CSE7759_two::prepare_hex_two(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex) {
  // Create hex string from 3 bytes with proper zero-padding
  if (byte1 < 16 && byte2 < 16 && byte3 < 16) {
    sprintf(hex, "0%x0%x0%x", byte1, byte2, byte3);
  } else if (byte1 < 16) {
    sprintf(hex, "0%x%x%x", byte1, byte2, byte3);
  } else if (byte2 < 16) {
    sprintf(hex, "%x0%x%x", byte1, byte2, byte3);
  } else if (byte3 < 16) {
    sprintf(hex, "%x%x0%x", byte1, byte2, byte3);
  } else {
    sprintf(hex, "%x%x%x", byte1, byte2, byte3);
  }
}

void CSE7759_two::calculate_power_two(PowerData2* data) {
  char temp_hex[10];          // Buffer for hex string
  float temp1, temp2, temp3;  // Temporary calculation variables

  // Voltage calculation
  prepare_hex_two(data->readings[2], data->readings[3], data->readings[4], temp_hex);
  temp1 = hex_to_dec_two(temp_hex);
  prepare_hex_two(data->readings[5], data->readings[6], data->readings[7], temp_hex);
  temp2 = hex_to_dec_two(temp_hex);
  data->voltage = data->test_flag ? 0 : temp1 / temp2;  // Final voltage

  // Current calculation
  prepare_hex_two(data->readings[8], data->readings[9], data->readings[10], temp_hex);
  temp1 = hex_to_dec_two(temp_hex);
  prepare_hex_two(data->readings[11], data->readings[12], data->readings[13], temp_hex);
  temp2 = hex_to_dec_two(temp_hex);
  data->current = data->test_flag ? 0 : temp1 / temp2;  // Final current

  // Power calculation
  prepare_hex_two(data->readings[14], data->readings[15], data->readings[16], temp_hex);
  temp1 = hex_to_dec_two(temp_hex);
  prepare_hex_two(data->readings[17], data->readings[18], data->readings[19], temp_hex);
  temp2 = hex_to_dec_two(temp_hex);
  data->power = data->test_flag ? 0 : temp1 / temp2;  // Final power

  data->test_flag = 0;  // Reset test_flag after calculation
}

PowerData2 CSE7759_two::read_power_data_two() {
  PowerData2 data;                   // Struct to store readings
  initialize_power_data_two(&data);  // Initialize struct
  unsigned char byte;                // Temporary variable for incoming byte
  unsigned int Checksum = 0;         // Variable for checksum calculation

  // Serial.println("Reading Chip...");

  for (int i = 0; i <= 100; i++) {  // Try up to 100 times to read data
    byte = Serial2.read();          // Read one byte from Serial2
    if (byte == 85) {               // First sync byte (0x55)
      data.readings[0] = byte;
      byte = Serial2.read();
      if (byte == 90) {  // Second sync byte (0x5A)
        data.readings[1] = byte;
        for (int j = 2; j <= 23; j++) {  // Read remaining 22 bytes
          data.readings[j] = Serial2.read();
        }
        // Checksum calculation was here but commented out
        // for (int x = 2; x <= 22; x++) {
        //   Checksum += data.readings[x];
        // }
        // if ((Checksum & data.checksum) == data.readings[23]) {
        calculate_power_two(&data);  // Compute voltage, current, power
        return data;                 // Return the data object with values
        // }
      }
    }
  }
  return data;  // Return even if no valid frame found
}

// AMit

// Add a new sample into the buffer
void CSE7759_two::addSamplesForPower(float value3) {
  samplesForPowerCSE2[indexForPowerCSE2] = value3;
  indexForPowerCSE2 = (indexForPowerCSE2 + 1) % 10;  // wrap around (circular buffer)
  if (indexForPowerCSE2 == 0) bufferFilledForPowerCSE2 = true;
}

// Calculate moving average
float CSE7759_two::getAverageForPower() {
  float sum = 0;
  int count = bufferFilledForPowerCSE2 ? 10 : indexForPowerCSE2;  // use only filled values
  for (int i = 0; i < count; i++) {
    sum += samplesForPowerCSE2[i];
  }
  return (count > 0) ? (sum / count) : 0;
}

// // Add a new sample into the buffer
// void CSE7759_two::addSamplesForCurrent(float value4) {
//   samplesForCurrentCSE2[indexForCurrentCSE2] = value4;
//   indexForCurrentCSE2 = (indexForCurrentCSE2 + 1) % 10;  // wrap around (circular buffer)
//   if (indexForCurrentCSE2 == 0) bufferFilledForCurrentCSE2 = true;
// }

// // Calculate moving average
// float CSE7759_two::getAverageForCurrent() {
//   float sum = 0;
//   int count = bufferFilledForCurrentCSE2 ? 10 : indexForCurrentCSE2;  // use only filled values
//   for (int i = 0; i < count; i++) {
//     sum += samplesForCurrentCSE2[i];
//   }
//   return (count > 0) ? (sum / count) : 0;
// }

// // Add a new sample into the buffer
// void CSE7759_two::addSamplesForVoltage(float value5) {
//   samplesForVoltageCSE2[indexForVoltageCSE2] = value5;
//   indexForVoltageCSE2 = (indexForVoltageCSE2 + 1) % 10;  // wrap around (circular buffer)
//   if (indexForVoltageCSE2 == 0) bufferFilledForVoltageCSE2 = true;
// }

// // Calculate moving average
// float CSE7759_two::getAverageForVoltage() {
//   float sum = 0;
//   int count = bufferFilledForVoltageCSE2 ? 10 : indexForVoltageCSE2;  // use only filled values
//   for (int i = 0; i < count; i++) {
//     sum += samplesForVoltageCSE2[i];
//   }
//   return (count > 0) ? (sum / count) : 0;
// }

/*
  INPUT: VOLTAGE, CURRENT, POWER STRUCTURE AND RELAY STATUS FLAG
  OUTPUT: VERIFIED VOLTAGE, CURRENT, POWER STRUCTURE
  OPERATION: CHECKING IF RECEIEVED DATA IS GARBAGE. IF YES, THEN RE-REQUEST FOR DATA. TRYING IT FOR 5 TIMES, IF STILL GETTING GARBAGE DATA THEN RETURNING THE PREVIOUS CORRECT DATA.
*/
PowerData2 CSE7759_two::validateTheCSEChipData_two(PowerData2 powerData) {
  Serial.println("*****************************CSE2***********************");
  float current_power = powerData.power;      // Store current power value
  float current_current = powerData.current;  // Store current current value
  float current_voltage = powerData.voltage;  // Store current voltage value

  // Replace invalid (inf) values with previous valid ones
  if (isinf(powerData.power) || isinf(powerData.current) || isinf(powerData.voltage)) {
    if (isinf(powerData.power)) {
      powerData.power = powerPre2;
    }
    if (isinf(powerData.current)) {
      powerData.current = currentPre2;
    }
    if (isinf(powerData.voltage)) {
      powerData.voltage = voltagePre2;
    }
  }

  addSamplesForPower(powerData.power);
  float avgForPower = getAverageForPower();
  // Power sanity check
  if ((powerData.power > (avgForPower + 100)) || (powerData.power < (avgForPower - 100))) {
    current_power = powerData.power;

    powerData.power = powerPre2;  // Revert to last valid
    current_current = powerData.current;
    powerData.current = currentPre2;  // Revert to last valid
    current_voltage = powerData.voltage;
    powerData.voltage = voltagePre2;  // Revert to last valid
    // Serial.print("current_power : ");
    // Serial.println(current_power);
    // Serial.print("current_current : ");
    // Serial.println(current_current);
    // Serial.print("current_voltage : ");
    // Serial.println(current_voltage);
  } else {
    powerPre2 = powerData.power;  // Update previous valid power
    // Serial.print("powerPre : ");
    // Serial.println(powerPre2);
    currentPre2 = powerData.current;  // Update previous valid current
    // Serial.print("currentPre : ");
    // Serial.println(currentPre2);
    voltagePre2 = powerData.voltage;  // Update previous valid voltage
    // Serial.print("voltagePre : ");
    // Serial.println(voltagePre2);
  }

  // addSamplesForCurrent(powerData.current);
  // float avgForCurrent = getAverageForCurrent();
  // // Current sanity check
  // if ((powerData.current > (avgForCurrent + 0.5)) || (powerData.current < (avgForCurrent - 0.5))) {
  //   current_current = powerData.current;
  //   Serial.print("current_current : ");
  //   Serial.println(current_current);
  //   powerData.current = currentPre2;  // Revert to last valid
  //   // Serial.print("powerData.current : ");
  //   // Serial.println(powerData.current);
  // } else {
  //   currentPre2 = powerData.current;  // Update previous valid current
  //   Serial.print("currentPre : ");
  //   Serial.println(currentPre2);
  // }

  // addSamplesForVoltage(powerData.voltage);
  // float avgForVoltage = getAverageForVoltage();
  // // Voltage sanity check
  // if ((powerData.voltage > (avgForVoltage + 50)) || (powerData.voltage < (avgForVoltage - 50))) {
  //   current_voltage = powerData.voltage;
  //   Serial.print("current_voltage : ");
  //   Serial.println(current_voltage);
  //   powerData.voltage = voltagePre2;  // Revert to last valid
  //   // Serial.print("powerData.voltage : ");
  //   // Serial.println(powerData.voltage);
  // } else {
  //   voltagePre2 = powerData.voltage;  // Update previous valid voltage
  //   Serial.print("voltagePre : ");
  //   Serial.println(voltagePre2);
  // }

  // Serial.print("Moving Power Avg : ");
  // Serial.println(avgForPower);
  // Serial.print("Moving Current Avg : ");
  // Serial.println(avgForCurrent);
  // Serial.print("Moving Voltage Avg : ");
  // Serial.println(avgForVoltage);

  // Debug print for received values (commented out in original)
  // Serial.print("***Received Parameters are from first CSE-> Voltage: ");
  // Serial.print(powerData.voltage, 2);
  // Serial.print(" V, Current: ");
  // Serial.print(powerData.current, 2);
  // Serial.print(" A, Power: ");
  // Serial.print(powerData.power, 2);
  // Serial.println(" W");

  return powerData;  // Return validated and corrected data
}
