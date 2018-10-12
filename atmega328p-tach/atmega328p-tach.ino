/* ATMega328P fan tachometer
 * 
 * This program reads the fan speed from the "tachometer" pin of a 3 or 4-pin
 * fan commonly found in computer equipment and calculates the RPM which
 * is sent to the serial monitor every second.
 * 
 * It works with the Arduino Uno and other ATMega328P based Arduinos.
 * 
 * This article from James "The Bald Engineer" was an excellent guide:
 * https://www.baldengineer.com/pwm-3-pin-pc-fan-arduino.html
 * 
 * I simplified the formula to reduce the number of division operations
 * and other simplifications. It's less readable, but maybe a little
 * bit more efficient to run.
 * 
 * Copyright (C) 2018  Scott W. Vincent
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

unsigned long serialMonLastUpdateMillis = 0;
unsigned long calcRPMLastMillis = 0;
unsigned long interval = 1000;
volatile unsigned long tachPulseCount = 0;

const int tachPin = 2;


void CountTachPulse()
{
  // Count each pulse from the tach pin of the fan
  tachPulseCount++;
}


void setup()
{
  Serial.begin(9600);
  pinMode(tachPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachPin), CountTachPulse, FALLING);
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
  // Update serial monitor every second.
  if (millis() - serialMonLastUpdateMillis > interval)
  {
    Serial.print("Fan RPM = ");
    Serial.println(CalculateRPM());
    serialMonLastUpdateMillis = millis();  
  }
}
