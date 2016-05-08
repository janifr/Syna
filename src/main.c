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

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_audio_codec.h"

#include "midi.h"
#include "organ.h"
#include "io.h"

int16_t audiobuffer[AUDIO_BUFFER_LENGTH];
uint8_t midibuffer[MIDI_BUFFER_LENGTH];

void LED_PUSH_Start(void);

int main(void)
{

    uint8_t n;

    Init_hid();
    Init_misc();
    Init_midi();
    Init_organ();

    if(EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, VOLUME, SAMPLERATE))
        while(1){}

    EVAL_AUDIO_Play((uint16_t*)audiobuffer, AUDIO_BUFFER_LENGTH);
    
    while(1)
    {
    	n=New_input();

    	if (n)
    	    Decode_midi(n);
    }
}

