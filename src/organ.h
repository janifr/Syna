/* Yet unnamed clonewheel organ for STM32F4 Discovery, working name "Syna"

Copyright (C) 2016 Jani Frilander

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#define SAMPLERATE                  48000
#define AUDIO_BUFFER_LENGTH_HALF    256
#define AUDIO_BUFFER_LENGTH         512

#define VOLUME                      85
//#define MANUAL_KEYS                 61
#define TONEWHEELS                  91

void Generate_buffer(uint16_t);
void Organ_noteon(uint8_t);
void Organ_noteoff(uint8_t);
void Init_organ();
