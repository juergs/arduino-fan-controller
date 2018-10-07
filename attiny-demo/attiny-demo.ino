/* ATTiny85 fan speed control demo
 * 
 * This demo ramps the speed of the fan up and down in a loop. I just wrote
 * it so I could quickly test this code without a potentiometer, etc. to
 * control it.
 * 
 * By default, the lowest it goes is 45%. Some fans will run at high speed
 * when given a PWM duty cycle that's too low; I want to avoid that in the
 * demo. 45% seems like a safe minimum in my testing but other fans may go
 * lower. Test fan was a FoxConn PVB120G12H-PO1 pulled from a Dell Optiplex.
 * 
 * It's an adaption of ATtiny_PWM_Phase_Corret_25kHz.ino from the original
 * project this is forked from by Marcelo Aquino: https://github.com/marceloaqno
 * That program controls the speed using a potentionmeter.
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
 * 
 * Hookup guide:
 * 
 *                         ATtiny85
 *                      -------u-------
 *  RST - A0 - (D 5) --| 1 PB5   VCC 8 |-- +5V
 *                     |               |
 *        A3 - (D 3) --| 2 PB3   PB2 7 |-- (D 2)
 *                     |               | 
 *        A2 - (D 4) --| 3 PB4   PB1 6 |-- (D 1) - PWM --> Fan Blue wire
 *                     |               |      
 *              Gnd ---| 4 GND   PB0 5 |-- (D 0) - PWM --> Disabled
 *                     -----------------
 */
 
// normal delay() won't work anymore because we are changing Timer1 behavior
// Adds delay_ms and delay_us functions
#include <util/delay.h>    // Adds delay_ms and delay_us functions

// Clock at 8mHz
#define F_CPU 8000000         // This is used by delay.h library

const int PWM_PIN = 1;        // Only works with Pin 1(PB1)
const int LOWER_BOUND = 72;   // 45% mimimum (45 * (160 / 100))


void setup()
{
  pinMode(PWM_PIN, OUTPUT);
  // Phase Correct PWM Mode, no Prescaler
  // PWM on Pin 1(PB1), Pin 0(PB0) disabled
  // 8Mhz / 160 / 2 = 25Khz
  TCCR0A = _BV(COM0B1) | _BV(WGM00);
  TCCR0B = _BV(WGM02) | _BV(CS00); 
  
  // Set TOP
  OCR0A = 160;  // TOP - DO NOT CHANGE, SETS PWM PULSE RATE

  // Run fan at 100% speed for 2 seconds to ensure it starts.
  // Intel spec says run at 30% speed max on startup but this
  // is easier. You can change the initial value of 0CR0B if
  // you want it to be "correct".
  OCR0B =160;    // duty cycle for Pin 1(PB1)
  _delay_ms(2000);
}


void loop()
{
  //Step from 100% to 0%
  for (int pwmValue = 160 ; pwmValue >= LOWER_BOUND; pwmValue -= 2)
  {
    // sets the value (range from 0 to 255):
    OCR0B = pwmValue;
    _delay_ms(1000);
  }
  
  //Step from 0% to 100%
  for (int pwmValue = LOWER_BOUND ; pwmValue <= 160; pwmValue += 2)
  {
    // sets the value (range from 0 to 255):
    OCR0B = pwmValue;
    _delay_ms(1000);
  }
}
