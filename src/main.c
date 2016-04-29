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


    Init_misc();
    Init_midi();
    Init_organ();

    EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, VOLUME, SAMPLERATE);
    EVAL_AUDIO_Play((uint16_t*)audiobuffer, AUDIO_BUFFER_LENGTH);
    
    while(1)
    {
    	n=New_input();

    	if (n)
    	    Decode_midi(n);
    }
}

