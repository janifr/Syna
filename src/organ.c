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

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "organ.h"
#include "io.h"

extern uint8_t Barchartcoarse[];

#define DELAYEDGAIN 16

//y=16384

#if SAMPLERATE == 48000
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
#endif

#if SAMPLERATE == 44100
static int16_t Multiplier[TONEWHEELS]={
    153,162,171,182,192,204,216,229,242,257,
    272,288,305,323,343,363,385,407,432,458,
    485,514,544,576,610,647,686,726,769,815,
    864,915,969,1027,1088,1152,1221,1293,1371,1452,
    1539,1630,1727,1830,1938,2054,2176,2305,2441,2586,
    2742,2904,3076,3258,3453,3658,3874,4106,4349,4606,
    4880,5169,5478,5802,6146,6508,6897,7305,7734,8195,
    8678,9190,9732,10306,10918,11559,12238,12952,13717,14519,
    15361,16262,17204,18197,19268,20385,21553,22772,24073,25427,
    26846};
#endif

#if SAMPLERATE == 42000
static int16_t Multiplier[TONEWHEELS]={
    160,170,180,191,202,214,227,240,254,270,
    286,303,321,340,360,381,404,428,453,480,
    509,539,571,605,641,679,720,763,808,856,
    907,961,1017,1078,1142,1210,1282,1358,1440,1525,
    1616,1711,1814,1921,2035,2157,2284,2420,2564,2716,
    2879,3049,3230,3421,3626,3841,4067,4311,4566,4836,
    5123,5427,5752,6092,6452,6833,7240,7669,8119,8603,
    9110,9646,10215,10816,11459,12130,12842,13591,14392,15232,
    16113,17057,18042,19081,20201,21367,22587,23859,25215,26625,
    28101};
#endif

#if SAMPLERATE == 32000
static int16_t Multiplier[TONEWHEELS]={
    210,223,236,250,265,281,298,315,334,354,
    375,397,421,446,472,500,530,562,595,631,
    668,708,750,794,841,891,945,1001,1060,1123,
    1190,1261,1335,1415,1499,1588,1683,1782,1889,2001,
    2120,2246,2380,2522,2670,2830,2998,3175,3364,3564,
    3777,4001,4238,4489,4757,5039,5336,5655,5989,6343,
    6719,7117,7542,7987,8459,8956,9489,10049,10637,11268,
    11928,12627,13367,14149,14984,15855,16776,17744,18778,19860,
    20991,22200,23458,24780,26199,27671,29201,30789,32470,0,
    0};
#endif

#if SAMPLERATE == 24000
static int16_t Multiplier[TONEWHEELS]={
    280,297,315,334,353,374,397,420,445,472,
    500,529,561,594,630,667,707,749,794,841,
    890,944,1000,1059,1122,1188,1260,1334,1414,1497,
    1587,1681,1780,1887,1999,2117,2243,2376,2519,2668,
    2827,2994,3173,3361,3560,3773,3996,4232,4484,4750,
    5034,5332,5648,5981,6338,6714,7109,7533,7977,8447,
    8947,9474,10038,10628,11254,11912,12617,13357,14133,14965,
    15835,16754,17726,18750,19840,20976,22174,23428,24762,26153,
    27601,29140,30732,32394,0,0,0,0,0,0,
    0};
#endif

//Gains found from net for RT-2 8:0dB 7:-3 6:-6 5:-10 4:-15 3:-20 2:-26 1:-32
//Use >>10
static int32_t Drawbar_gain[9]={
    0,26,51,102,182,324,512,725,1024};

//Gains for smmla multiplication, use <<10 for oscillator value
//static int32_t Drawbar_gain[9]={
//    0,0x1a000,0x33000,0x66000,0xb6000,0x144000,0x200000,0x2d5000,0x400000};

static int16_t Drawbar_harmonic[9]={
    -12,7,0,12,19,24,28,31,36};

static int32_t Keyclick_gain[2]; //one for each manual
static int32_t Tonewheel_gain[TONEWHEELS];
static int32_t Tonewheel_target_gain[TONEWHEELS];
static int8_t Tonewheel_active[TONEWHEELS];
static int8_t Percussion_harmonic;
static int32_t Percussion_gain[TONEWHEELS];
static int32_t Percussion_target_gain[TONEWHEELS];
static int32_t Percussion_global_target_gain;
static int16_t Drawbar_setting[9];

static uint16_t drawbarstate,drawbarindex;

extern int16_t audiobuffer[AUDIO_BUFFER_LENGTH];

static int32_t oscillator_x[TONEWHEELS];
static int32_t oscillator_y[TONEWHEELS];
static uint16_t display_buffer[84];
static uint16_t display_index;

#define LOWPASS_BUFFER_LENGTH 4

static int16_t lowpass_buffer[LOWPASS_BUFFER_LENGTH];
static uint16_t lowpass_index;

static int16_t keys_pressed;
static float delay_line[32];
static uint16_t delay_pos;
static float lfo_position,lfo_increment,lfo_orig_increment;
static float lfo_treshold;

static uint16_t stability_index;
static uint32_t sample_index;

void Update_display_buffer()
{
    uint8_t i;
    for (i=0;i<9;i++)
    {
        display_buffer[i+1]=(Barchartcoarse[Drawbar_setting[i]<<2]<<7)+2;
        display_buffer[i+22]=(Barchartcoarse[(Drawbar_setting[i]<<2)+1]<<7)+2;
        display_buffer[i+43]=(Barchartcoarse[(Drawbar_setting[i]<<2)+2]<<7)+2;
        display_buffer[i+64]=(Barchartcoarse[(Drawbar_setting[i]<<2)+3]<<7)+2;
    }
}

void Init_organ()
{
    int i;
    Keyclick_gain[0]=0;
    Keyclick_gain[1]=0;
    stability_index=0;
    Percussion_harmonic=3;
    sample_index=0;
    lfo_position=0;
    lfo_treshold=24.0f;
    lfo_orig_increment=lfo_treshold*14.0f/(float) SAMPLERATE;
    lfo_increment=lfo_orig_increment;

    for (i=0;i<84;i++)
        display_buffer[i]=(32<<7)+2;

    display_buffer[0]=0x80<<7;
    display_buffer[21]=0xc0<<7;
    display_buffer[42]=0x94<<7;
    display_buffer[63]=0xd4<<7;
    display_index=0;
    keys_pressed=0;
    delay_pos=0;

    for (i=0;i<LOWPASS_BUFFER_LENGTH;i++)
        lowpass_buffer[i]=0;
    
    for (i=0;i<32;i++)
        delay_line[i]=0.0f;

    lowpass_index =0;
    Percussion_global_target_gain=0;
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
    
    for(i=0;i<8;i++)
    {
        Percussion_gain[i]=0;
        Percussion_target_gain[i]=0;
    }
    
    for(i=0;i<TONEWHEELS;i++)
    {
        oscillator_x[i]=0; // Tonewheels are synced. Not real situation.
        oscillator_y[i]=16384; //1024
        Tonewheel_gain[i]=0;
        Tonewheel_target_gain[i]=0;
        Tonewheel_active[i]=0;
    }
}

void Organ_noteon(uint8_t note)
{
    int16_t tmp,mult,shift,tmp2,i;
    
    
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
            Percussion_global_target_gain=Drawbar_gain[8]<<4;
        }

        Keyclick_gain[0]=Drawbar_gain[8]<<4;

        tmp=Drawbar_harmonic[Percussion_harmonic]+note-24;
        if (tmp<TONEWHEELS)
        {
            Percussion_target_gain[tmp]=
                Percussion_global_target_gain;
            Tonewheel_active[tmp]=1;
        }

        for (i=0;i<9;i++)
        {
            if(Drawbar_setting[i]>0)
            {
                mult=1;
                shift=0;
                tmp=Drawbar_harmonic[i]+note-24;
                while (tmp<0)//foldback
                {
                    mult *= 3;
                    shift +=2;
                    tmp += 12;
                }
                while (tmp>(TONEWHEELS-1))
                {
                    mult *= 3;
                    shift +=2;
                    tmp -= 12;
                }
                Tonewheel_active[tmp]=1;
 
                Tonewheel_target_gain[tmp] +=
                    ((mult*Drawbar_gain[Drawbar_setting[i]])>>shift)<<4;
                
                tmp2=tmp+12;//Keyclick harmonics
                if(tmp2<TONEWHEELS)
                {
                    Tonewheel_gain[tmp2]+=Tonewheel_target_gain[tmp]>>3;
                    Tonewheel_active[tmp2]=1;
                    tmp2 =tmp+19;
                        if(tmp2<TONEWHEELS)
                        {
                            Tonewheel_gain[tmp2]+=
                                Tonewheel_target_gain[tmp]>>3;
                            Tonewheel_active[tmp2]=1;
                        tmp2=tmp+24;
                            if(tmp2<TONEWHEELS)
                            {
                                Tonewheel_gain[tmp2]+=
                                    Tonewheel_target_gain[tmp]>>3;
                                Tonewheel_active[tmp2]=1;
                            }
                        }
                }
            }
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
    int16_t tmp,i,j,mult,shift;
    //GPIO_ResetBits(GPIOD, GPIO_Pin_15);
    
    if ((note>23) && (note<97))
    {
        tmp=Drawbar_harmonic[Percussion_harmonic]+note-24;
        if (tmp<TONEWHEELS)
        {
            Percussion_target_gain[tmp]=0;
        }

        for (i=0;i<9;i++)
        {
            mult=1;
            shift=0;
            tmp=Drawbar_harmonic[i]+note-24;
            while (tmp<0)
            {
                mult *= 3;
                shift+=2;
                tmp += 12;
            }
            while (tmp>(TONEWHEELS-1))
            {
                mult *= 3;
                shift+=2;
                tmp -= 12;
            }
            Tonewheel_target_gain[tmp] -= 
                ((mult*Drawbar_gain[Drawbar_setting[i]])>>shift)<<4;
            if (Tonewheel_target_gain[tmp]<0)
                Tonewheel_target_gain[tmp]=0;
        }
        keys_pressed--;
        if (keys_pressed<0)
            keys_pressed=0;
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
    if (display_index>167)
        display_index=0;
}

void Generate_buffer(uint16_t start)
{
    uint16_t i,j,p;

    //static int32_t tim;
    uint8_t buf[8];
    static float outf,outf1,outf2,outf3,dp; 
    static int32_t temp,cmp;
    static int32_t out,out2;
    static int32_t oscx,oscy;
    //static int32_t klick; 
    static int32_t upper_manual_buffer[(AUDIO_BUFFER_LENGTH_HALF>>1)];
    static int32_t lower_manual_buffer[(AUDIO_BUFFER_LENGTH_HALF>>1)];
    static int32_t percussion_buffer[(AUDIO_BUFFER_LENGTH_HALF>>1)];
    static int32_t rnd=22222;

    //tim=SysTick->VAL;

//    static int32_t test=0,test2=0;

    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
    
    Insert_display_data();
//    i = 0;
    for(i=0;i<(AUDIO_BUFFER_LENGTH_HALF>>1);i++)
    {
        upper_manual_buffer[i]=0;
        lower_manual_buffer[i]=0;
        percussion_buffer[i]=0;
    }
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    for(j=0;j<TONEWHEELS;j++)
    {
        if (Tonewheel_active[j])
        {
            GPIO_SetBits(GPIOD, GPIO_Pin_13);
            for(i=0;i<(AUDIO_BUFFER_LENGTH_HALF>>1);i++)
            {
                temp = (127*Percussion_gain[j]+Percussion_target_gain[j])>>7;
                Percussion_gain[j] =temp;
                temp = (127*Tonewheel_gain[j]+Tonewheel_target_gain[j])>>7;
                Tonewheel_gain[j]= temp;
                temp = ((oscillator_y[j] * Multiplier[j])>>15);
                oscillator_x[j] -= temp;
                temp = (oscillator_x[j] * Multiplier[j])>>15;
                oscillator_y[j] += temp;
                temp = (oscillator_x[j]*(Tonewheel_gain[j]))>>17;
                upper_manual_buffer[i] = upper_manual_buffer[i]+temp;
                temp = (oscillator_x[j]*(Percussion_gain[j]>>4))>>12;
                percussion_buffer[i]= percussion_buffer[i]+temp;
                //manual2[i] = manual2[i]+(temp>>2);
                Percussion_target_gain[j]=(16383*Percussion_target_gain[j]>>14);
            }
        }
        else
        {
            for(i=0;i<(AUDIO_BUFFER_LENGTH_HALF>>1);i++)
            {
                temp = ((oscillator_y[j] * Multiplier[j])>>15);
                oscillator_x[j] -= temp;
                temp = (oscillator_x[j] * Multiplier[j])>>15;
                oscillator_y[j] += temp;
            }
        }
        if((Tonewheel_gain[j]==0)&&(Percussion_gain[j]==0)&&
            (Tonewheel_target_gain[j]==0)&&(Percussion_target_gain[j]==0))
            Tonewheel_active[j]=0;
    }
    
    if (Keyclick_gain[0]>0)
        for(i=0;i<(AUDIO_BUFFER_LENGTH_HALF>>1);i++)
        {
            rnd = rnd*196314165+907633515;
            temp = (127*Keyclick_gain[0])>>7;
            Keyclick_gain[0]=temp;
            upper_manual_buffer[i] += (rnd&0xffff)*temp>>20;
        }

    for(i=0;i<(AUDIO_BUFFER_LENGTH_HALF>>1);i++)
    {
        Percussion_global_target_gain=
            (16383*Percussion_global_target_gain>>14);
        lfo_position+=lfo_increment;
        if(lfo_position>lfo_treshold)
            lfo_increment=lfo_orig_increment*(-1.0f);
        else
            if(lfo_position<0.0f)
                lfo_increment=lfo_orig_increment;

        outf1 = (float) upper_manual_buffer[i];
        outf1 = 0.000025*outf1;                
        outf3 = (float) percussion_buffer[i];
        outf3 = 0.000025*outf3;

        delay_line[delay_pos]=outf1;
        delay_pos++;
        delay_pos &= 0x1f;
        p = (uint16_t)lfo_position;
        dp = lfo_position - (float) p;
        
        outf2=(1-dp)*delay_line[((1+delay_pos+p)&0x1f)]+
                dp*delay_line[((2+delay_pos+p)&0x1f)];
        //outf=0.5*outf1;
        outf=0.4*(outf1+outf2+outf3);
        
        //temp=manual1[i]+manual2[i];
        //outf=0.000033*(float)temp;
        GPIO_ResetBits(GPIOD, GPIO_Pin_14);
        
        if((outf>0.9f)||(outf<-0.9f))
            GPIO_SetBits(GPIOD, GPIO_Pin_14);
 
        out = (int32_t) 30000*outf*(27+outf*outf)/(27+9*outf*outf);
        
        lowpass_buffer[lowpass_index]=out;
        lowpass_index++;
        lowpass_index &=3;
        
        //simple low-pass filter
 
        temp = (lowpass_buffer[0]+lowpass_buffer[1]+
                lowpass_buffer[2]+lowpass_buffer[3])>>2;//+
                //lowpass_buffer[4]+lowpass_buffer[5]+
                //lowpass_buffer[6]+lowpass_buffer[7])>>3;

        out = temp;

        audiobuffer[start+(i<<1)] = (int16_t) out;
        audiobuffer[start+(i<<1)+1] = (int16_t) out;


	//i = i+2;
        /*if(sample_index>1000000)
        {
            if (cmp)
            {
                oscx=oscillator_x[stability_index];
                oscy=oscillator_y[stability_index];
                cmp=0;
                display_buffer[11]=('S'<<7)+2;
            }
            else
            {
                if ((oscx==oscillator_x[stability_index])&&
                    (oscy==oscillator_y[stability_index]))
                {
                    display_buffer[12]=(('0'+((stability_index&0xf0)>>4))<<7)+2;
                    display_buffer[13]=(('0'+(stability_index&0xf))<<7)+2;
                    stability_index++;
                    cmp=1;
                    if(stability_index>TONEWHEELS)
                        stability_index=0;
                }

            }
        }
        else
        {
            sample_index++;
            cmp=1;
        }*/
    }
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

