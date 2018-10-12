/* ATMega328P potentiometer controlled fan speed and tachometer
 * 
 * This program controls the fan speed using a potentiometer input. It also
 * reads the RPM using the tach signal from the fan, and outputs the data
 * to an LCD screen over i2c.
 * 
 * It works with the Arduino Uno and other ATMega328P based Arduinos.
 * 
 * By default, the lowest PWM goes is 45%. Some fans will run at high speed
 * when given a PWM duty cycle that's too low; I want to avoid that in the
 * demo. 45% seems like a safe minimum in my testing but other fans may go
 * lower. Test fan was a FoxConn PVB120G12H-PO1 pulled from a Dell Optiplex.
 * 
 * It's an adaption of ATmega328P_PWM_25kHz.ino from the original
 * project this is forked from by Marcelo Aquino: https://github.com/marceloaqno
 * The primary differences are that I set a lower bound for the PWM signal and
 * I run the fan for 2 seconds on startup at full speed.
 * 
 * Copyright (C) 2018  Scott W. Vincent
 * Copyright (C) 2014  Marcelo Aquino
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


const int TACH_PIN = 2;       // Tachometer input
const int PWM_PIN = 3;        // Only works with Pin 1(PB1)
const int POT_PIN = 0;        // Potentiometer input
const int LOWER_BOUND = 35;   // 45% mimimum (45 * (79 / 100))
const unsigned long SERIAL_UPDATE_INTERVAL = 1000;

unsigned long serialLastUpdateMillis = 0;
unsigned long calcRPMLastMillis = 0;
volatile unsigned long tachPulseCount = 0;


void CountTachPulse()
{
  // Count each pulse from the tach pin of the fan
  tachPulseCount++;
}


void setup()
{
  // Setup tach
  Serial.begin(9600);
  Serial.println("Starting fan...");
  pinMode(TACH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), CountTachPulse, FALLING);
  
  // Setup PWM control
  pinMode(PWM_PIN, OUTPUT);
  // Fast PWM Mode, Prescaler = /8
  // PWM on Pin 3, Pin 11 disabled
  // 16Mhz / 8 / (79 + 1) = 25Khz
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS21);
  
  // Set TOP
  OCR2A = 79;   // TOP - DO NOT CHANGE, SETS PWM PULSE RATE   

  // Run fan at 100% speed for 2 seconds to ensure it starts.
  // Intel spec says run at 30% speed max on startup but this
  // is easier. You can change the initial value of 0CR2B if
  // you want it to be "correct".
  // Note: duty cycle for Pin 3 (0-79) generates 1 500nS
  // pulse even when 0.
  OCR2B =79;    // duty cycle for Pin 1(PB1)
  delay(2000);
}


unsigned long CalculateRPM()
{
  // Calculate RPM based on time passed and pulses counted.
  
  noInterrupts();

  // Calculate RPM
  float elapsedMS = (millis() - calcRPMLastMillis);
  unsigned long rpm = (tachPulseCount / (elapsedMS / 500.0)) * 60;

  // Reset for next round
  calcRPMLastMillis = millis();
  tachPulseCount = 0;
  
  interrupts();
  
  return rpm;
}


void loop()
{
  int in = analogRead(POT_PIN);
  int out = map(in, 0, 1023, LOWER_BOUND, 79);
  OCR2B = out;

  // Update serial monitor with RPM every second
  if (millis() - serialLastUpdateMillis > SERIAL_UPDATE_INTERVAL)
  {
    Serial.print("Fan RPM = ");
    Serial.println(CalculateRPM());
    serialLastUpdateMillis = millis();  
  }
}
