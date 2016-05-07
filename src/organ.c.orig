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

// Copyright 2016 Jani Frilander

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "organ.h"
#include "io.h"

extern uint8_t Barchartcoarse[];

//y=1024
/*static int16_t Multiplier[TONEWHEELS]={
    137,149,160,165,175,191,197,209,224,234,
    254,263,283,295,318,332,354,375,395,420,
    447,472,499,529,560,594,630,669,706,748,
    794,840,892,943,1000,1059,1123,1188,1260,1335,
    1413,1497,1587,1681,1780,1887,1999,2117,2243,2376,
    2520,2668,2827,2994,3173,3361,3559,3773,3996,4232,
    4484,4750,5034,5332,5648,5981,6338,6714,7109,7532,
    7977,8447,8947,9475,10039,10628,11254,11912,12617,13357,
    14133,14965,15835,16754,17744,18778,19860,20992,22199,23458,
    24780};*/

//y=16384
static int16_t Multiplier[TONEWHEELS]={
    140,149,157,167,177,187,198,210,223,236,
    250,265,280,297,315,334,353,374,397,420,
    445,472,500,529,561,594,630,667,707,749,
    794,841,890,944,1000,1059,1122,1188,1260,1334,
    1414,1497,1587,1681,1780,1887,1999,2117,2243,2376,
    2519,2668,2827,2994,3173,3361,3560,3773,3996,4232,
    4484,4750,5034,5332,5648,5981,6338,6714,7109,7533,
    7977,8447,8947,9475,10038,10628,11254,11912,12617,13357,
    14133,14965,15835,16754,17744,18778,19860,20991,22200,23458,
    24780};


//Gains found from net for RT-2 8:0dB 7:-3 6:-6 5:-10 4:-15 3:-20 2:-26 1:-32
//Use >>10
static int32_t Drawbar_gain[9]={
    0,26,51,102,182,324,512,725,1024};

static int16_t Drawbar_harmonic[9]={
    -12,7,0,12,19,24,28,31,36};

static int32_t Tonewheel_gain[TONEWHEELS];
static int32_t Tonewheel_target_gain[TONEWHEELS];

static int16_t Drawbar_setting[9];

static uint16_t drawbarstate,drawbarindex;

extern int16_t audiobuffer[AUDIO_BUFFER_LENGTH];

static int32_t oscillator_x[TONEWHEELS];
static int32_t oscillator_y[TONEWHEELS];
static uint16_t display_buffer[40];
static uint16_t display_index;

#define EFFECT_BUFFER_LENGTH 4

static int16_t effect_buffer[EFFECT_BUFFER_LENGTH];
static uint16_t effect_index;

static int16_t keys_pressed;
static int32_t percussion_gain,percussion_target_gain;
static int16_t percussion_index;
static int8_t percussion_note;
static float delay_line[32];
static uint16_t delay_pos;
static float lfo_position,lfo_increment,lfo_orig_increment;
static float lfo_treshold;

void Update_display_buffer()
{
    uint8_t i;
    for (i=0;i<9;i++)
    {
        display_buffer[i+1]=(Barchartcoarse[Drawbar_setting[i]<<2]<<7)+2;
        display_buffer[i+11]=(Barchartcoarse[(Drawbar_setting[i]<<2)+1]<<7)+2;
        display_buffer[i+21]=(Barchartcoarse[(Drawbar_setting[i]<<2)+2]<<7)+2;
        display_buffer[i+31]=(Barchartcoarse[(Drawbar_setting[i]<<2)+3]<<7)+2;
    }
}

void Init_organ()
{
    int i;
    lfo_position=0;
    lfo_treshold=24.0f;
    lfo_orig_increment=lfo_treshold*14.0f/48000.0f;
    lfo_increment=lfo_orig_increment;

    display_buffer[0]=0x80<<7;
    display_buffer[10]=0xc0<<7;
    display_buffer[20]=0x94<<7;
    display_buffer[30]=0xd4<<7;
    display_index=0;
    keys_pressed=0;
    delay_pos=0;

    for (i=0;i<EFFECT_BUFFER_LENGTH;i++)
        effect_buffer[i]=0;
    
    for (i=0;i<32;i++)
        delay_line[i]=0.0f;

    effect_index =0;
    percussion_gain=0;
    percussion_target_gain=0;
    drawbarstate = 0;
    drawbarindex = 0;

    for(i=0;i<AUDIO_BUFFER_LENGTH;i++)
        audiobuffer[i]=0;

    Drawbar_setting[0]=8;
    Drawbar_setting[1]=8;
    Drawbar_setting[2]=8;
    Drawbar_setting[3]=8;
    Drawbar_setting[4]=0;
    Drawbar_setting[5]=0;
    Drawbar_setting[6]=0;
    Drawbar_setting[7]=0;
    Drawbar_setting[8]=0;
    Update_display_buffer();
        
    for(i=0;i<TONEWHEELS;i++)
    {
        oscillator_x[i]=0; // Tonewheels are synced. Not real situation.
        oscillator_y[i]=16384; //1024
        Tonewheel_gain[i]=0;
        Tonewheel_target_gain[i]=0;
    }
}

void Organ_noteon(uint8_t note)
{
    int16_t tmp,i;
    
    
    //GPIO_SetBits(GPIOD, GPIO_Pin_15); 
   //quick hack to manipulate drawbars. 
    if ((note>11) && (note<21))
    {
        if(drawbarstate==0)
        {
            drawbarindex=note-12;
            GPIO_SetBits(GPIOD, GPIO_Pin_15);
            drawbarstate++;
        }
        else
        {
            Drawbar_setting[drawbarindex]=note-12;
            drawbarstate=0;
            GPIO_ResetBits(GPIOD, GPIO_Pin_15);
        }
    Update_display_buffer();
    }
        //hack ends here

    if ((note>23) && (note<97))
    {
        keys_pressed++;
        if (keys_pressed==1) //activate harmonic percussion
        {//use index 3 for 2nd and 4 for 3rd setting
             percussion_index=Drawbar_harmonic[3]+note-24;
             percussion_target_gain=Drawbar_gain[8]<<6;
             percussion_note=note;
        }

        for (i=0;i<9;i++)
        {
            tmp=Drawbar_harmonic[i]+note-24;
            while (tmp<0)
                tmp += 12;
            while (tmp>(TONEWHEELS-1))
                tmp -= 12;
            Tonewheel_target_gain[tmp] += Drawbar_gain[Drawbar_setting[i]];
            Tonewheel_gain[tmp] = Tonewheel_target_gain[tmp]>>2;
        }
    }
}

/*void led_if_target_gains_zero()
{
    int i;
    GPIO_SetBits(GPIOD, GPIO_Pin_13);

    for (i=0;i<TONEWHEELS;i++)
        if (Tonewheel_target_gain[i]>0)
            GPIO_ResetBits(GPIOD, GPIO_Pin_13);

}*/

void Organ_noteoff(uint8_t note)
{
    int16_t tmp,i;

    //GPIO_ResetBits(GPIOD, GPIO_Pin_15);
    
    if ((note>23) && (note<97))
    {
        for (i=0;i<9;i++)
        {
            tmp=Drawbar_harmonic[i]+note-24;
            while (tmp<0)
                tmp += 12;
            while (tmp>(TONEWHEELS-1))
                tmp -= 12;
            Tonewheel_target_gain[tmp] -= Drawbar_gain[Drawbar_setting[i]];
            if (Tonewheel_target_gain[tmp]<0)
                Tonewheel_target_gain[tmp]=0;
        }
        keys_pressed--;
        if (keys_pressed<0)
            keys_pressed=0;
        if (note==percussion_note)
            percussion_target_gain=0;
    }
   // led_if_target_gains_zero();
}

void Insert_display_data()
{
    if(display_index&1)
    {
        Display_clock();
    }
    else
    {   
        Display_data_no_clock(display_buffer[display_index>>1]);
    }
    display_index++;
    if (display_index>79)
        display_index=0;
}

void Generate_buffer(uint16_t start)
{
    uint16_t i,j,p;

    static float outf,outf1,outf2,dp;
    
    static int32_t temp;
    static int32_t out;
    
    //GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    
    Insert_display_data();
    i = 0;
    
    while(i<AUDIO_BUFFER_LENGTH_HALF)
    {
        out=0;
        
       
        for(j=0;j<TONEWHEELS;j++)
        {
            temp = (127*Tonewheel_gain[j]+Tonewheel_target_gain[j])>>7;
            Tonewheel_gain[j]= temp;
            temp = ((oscillator_y[j] * Multiplier[j])>>15);
            oscillator_x[j] -= temp;
            temp = (oscillator_x[j] * Multiplier[j])>>15;
            oscillator_y[j] += temp;
            temp = (oscillator_x[j]*Tonewheel_gain[j])>>13;
            out += temp;
        }
        
        percussion_gain = (127*percussion_gain+percussion_target_gain)>>7;
        temp = (oscillator_x[percussion_index]*percussion_gain)>>18;
        percussion_target_gain -= 3;
        if (percussion_target_gain<0)
            percussion_target_gain=0;
        
        out += temp;

        GPIO_ResetBits(GPIOD, GPIO_Pin_13);

        if((out>30000)||(out<-30000))
            GPIO_SetBits(GPIOD, GPIO_Pin_13);
        
        lfo_position+=lfo_increment;
        if(lfo_position>lfo_treshold)
            lfo_increment=lfo_orig_increment*(-1.0f);
        else
            if(lfo_position<0.0f)
                lfo_increment=lfo_orig_increment;

                
        outf1 = (float) out;
        outf1 = 0.000033*outf1;

        delay_line[delay_pos]=outf1;
        delay_pos++;
        delay_pos &= 0x1f;
        p = (uint16_t)lfo_position;
        dp = lfo_position - (float) p;
        
        outf2=(1-dp)*delay_line[((1+delay_pos+p)&0x1f)]+
                dp*delay_line[((2+delay_pos+p)&0x1f)];
        //delay line with allpass interpolation filter
        //outf2=0.15f*(delay_line[((1+delay_pos+di)&0x1f)]-prev_outf2)+
        //        delay_line[((2+delay_pos+di)&0x1f)];
        //prev_outf2=outf2;

        outf=0.5*(outf1+outf2);
        out = (int32_t) 30000*outf*(27+outf*outf)/(27+9*outf*outf);
        
        effect_buffer[effect_index]=out;
        effect_index++;
        effect_index &=2;
        
        //simple low-pass filter
 
        temp = (effect_buffer[0]+effect_buffer[1]+
                effect_buffer[2]+effect_buffer[3])>>2;//+
                //effect_buffer[4]+effect_buffer[5]+
                //effect_buffer[6]+effect_buffer[7])>>3;

        out = temp;

        audiobuffer[start+i] = (int16_t) out;
        audiobuffer[start+i+1] = (int16_t) out;


	i = i+2;
    }

    //GPIO_SetBits(GPIOD, GPIO_Pin_13);

}

