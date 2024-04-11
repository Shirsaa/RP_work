#include "./model.h"
Eloquent::ML::Port::RandomForestRegressor regressor;

// Define analog input pins
#define VOLTAGE_PIN A0
#define CURRENT_PIN A1

// Floats for ADC voltage & Input voltage
float voltage_adc_voltage = 0.0;
float voltage_in_voltage = 0.0;

float current_sensitivity = 2.98;   // Sensitivity of the LA55P in mV/A (check datasheet)
float voltage_sensitivity = 23.76;   

// Floats for resistor values in voltage divider (in ohms)
float voltage_R1 = 10000.0;
float voltage_R2 = 1000.0;

// Float for Reference Voltage
float voltage_ref_voltage = 5;

// Integer for ADC value
int voltage_adc_value = 0;

//current soc value (predicted)
float soc_prev = 0;
float soc_predicted = 0;
float soc_initial =0;
float i_dt = 0;
float soc_actual = soc_initial;

float a1 = 5.211;
float b1 = -45.5;
float a2 = 43.35;
float b2 = -456.4;
float a3 = 323.7;
float b3 = -3528;
float a4 = 58.61;
float b4 = -598;
float a5 = 18.67;
float b5 = -139.4;
float a6 = 27.11;
float b6 = -243.11;

float v1 = 8.4;
float v2 = 10.641;
float v3 = 10.977;
float v4 = 11.055;
float v5 = 11.483;
float v6 = 12.3;
float v7 = 12.6;

void setup() {
  // Setup Serial Monitor
  Serial.begin(9600);
  Serial.println("Voltage and Current Test");
}

void loop() {

  // Read the Voltage Analog Input
  voltage_adc_value = analogRead(VOLTAGE_PIN);

  // Determine voltage at ADC input
  voltage_adc_voltage = (voltage_adc_value * voltage_ref_voltage) / 1024.0;

  // Calculate voltage at voltage divider input
  voltage_in_voltage = voltage_adc_voltage * voltage_sensitivity;

  // Read the Current Analog Input
  int current_sensorValue = analogRead(CURRENT_PIN);

  // Convert to current
  float current = current_sensorValue * (5 / 1024.0) * current_sensitivity;

  //count time in milliseconds
  long int time = millis();


  // Print voltage, current time results to Serial Monitor to 2 decimal places
  Serial.print(F("Current_(in amperes):")); 
  Serial.print(current, 4); 
  Serial.print(" ");
  Serial.print(F("Voltage_Input_(in volts):")); 
  Serial.print(voltage_in_voltage, 4); 
  Serial.print(" ");
  Serial.print(F("Time_(in milliseconds):")); 
  Serial.print(time);
  Serial.print(" ");


  if(soc_prev==0){

    if(v1< voltage_in_voltage && voltage_in_voltage <=v2){
      soc_initial = a1*voltage_in_voltage + b1;
    }
    if(v2<voltage_in_voltage && voltage_in_voltage<=v3){
      soc_initial = a2*voltage_in_voltage + b2;
    }
    if(v3<voltage_in_voltage && voltage_in_voltage<=v4){
      soc_initial = a3*voltage_in_voltage + b3;
    }
    if(v4<voltage_in_voltage && voltage_in_voltage<=v5){
      soc_initial = a4*voltage_in_voltage + b4;
    }
    if(v5<voltage_in_voltage && voltage_in_voltage<=v6){
      soc_initial = a5*voltage_in_voltage + b5;
    }
    if(v6<voltage_in_voltage && voltage_in_voltage<=v7){
      soc_initial = a6*voltage_in_voltage + b6;
    }

    float params[] = {(voltage_in_voltage-8.3981)/3.2775, (current+8.098)/0.00164, (soc_prev-0.002)/73.4166};
    soc_predicted = (regressor.predict(params));

    Serial.print(F("Predicted_soc:"));
    Serial.print((soc_predicted*73.4166)+0.002, 4);
    Serial.print(" ");

    soc_initial = soc_initial;
    soc_prev = soc_predicted;
  }

  else{
    float params[] = {(voltage_in_voltage-8.3981)/3.2775, (current+8.098)/0.00164, (soc_prev-0.002)/73.4166};
    soc_predicted = (regressor.predict(params));

    Serial.print(F("Predicted_soc:"));
    Serial.print((soc_predicted*73.4166)+0.002, 4);
    Serial.print(" ");

    soc_prev = soc_predicted;

  }


  //calculate (i*dt/Q)*100
  i_dt = ((current*time)/(8.1*3600*1000))*100;
  soc_actual = soc_initial - i_dt;
  Serial.print(F("soc_actual:"));
  Serial.println(soc_actual, 4);

  // Short delay
  delay(1000);
}
