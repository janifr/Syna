// Copyright 2016 Jani Frilander

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "organ.h"

static int16_t Multiplier[TONEWHEELS]={
    137,149,160,165,175,191,197,209,224,234,
    254,263,283,295,318,332,354,375,395,420,
    447,472,499,529,560,594,630,669,706,748,
    794,840,892,943,1000,1059,1123,1188,1260,1335,
    1413,1497,1587,1681,1780,1887,1999,2117,2243,2376,
    2520,2668,2827,2994,3173,3361,3559,3773,3996,4232,
    4484,4750,5034,5332,5648,5981,6338,6714,7109,7532,
    7977,8447,8947,9475,10039,10628,11254,11912,12617,13357,
    14133,14965,15835,16754,17744,18778,19860,20992,22199,23458,
    24780};

//Gains found from net for RT-2 8:0dB 7:-3 6:-6 5:-10 4:-15 3:-20 2:-26 1:-32
//Use >>10
static int16_t Drawbar_gain[9]={
    0,26,51,102,182,324,512,725,1024};

static int16_t Drawbar_harmonic[9]={
    -12,7,0,12,19,24,28,31,36};

static int16_t Tonewheel_gain[TONEWHEELS];
static int16_t Drawbar_setting[9];
static uint16_t drawbarstate,drawbarindex;

extern int16_t audiobuffer[AUDIO_BUFFER_LENGTH];

static int16_t oscillator_x[TONEWHEELS];
static int16_t oscillator_y[TONEWHEELS];

void Init_organ()
{
    int i;

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
        
    for(i=0;i<TONEWHEELS;i++)
    {
        oscillator_x[i]=0; // Tonewheels are synced. Not real situation.
        oscillator_y[i]=1024;
        Tonewheel_gain[i]=0;
    }
}

void Organ_noteon(uint8_t note)
{
    int16_t tmp,i;
    
    GPIO_SetBits(GPIOD, GPIO_Pin_15); 
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
    }
        //hack ends here

    if ((note>23) && (note<97))
    {
        for (i=0;i<9;i++)
        {
            tmp=Drawbar_harmonic[i]+note-24;
            while (tmp<0)
                tmp += 12;
            while (tmp>(TONEWHEELS-1))
                tmp -= 12;
            Tonewheel_gain[tmp] += Drawbar_gain[Drawbar_setting[i]];
        }
    }
}

void Organ_noteoff(uint8_t note)
{
    int16_t tmp,i;

    GPIO_ResetBits(GPIOD, GPIO_Pin_15);
    
    if ((note>23) && (note<97))
    {
        for (i=0;i<9;i++)
        {
            tmp=Drawbar_harmonic[i]+note-24;
            while (tmp<0)
                tmp += 12;
            while (tmp>(TONEWHEELS-1))
                tmp -= 12;
            Tonewheel_gain[tmp] -= Drawbar_gain[Drawbar_setting[i]];
            if (Tonewheel_gain[tmp]<0)
                Tonewheel_gain[tmp]=0;
        }
    }
}


void Generate_buffer(uint16_t start)
{
    uint16_t i,j;
    
    static int32_t temp;
    static int16_t out;

    GPIO_ResetBits(GPIOD, GPIO_Pin_13);

    i = 0;

    while(i<AUDIO_BUFFER_LENGTH_HALF)
    {
        out=0;
        
        for(j=0;j<TONEWHEELS;j++)
        {
            temp = (uint32_t) ((oscillator_y[j] * Multiplier[j])>>15);
            oscillator_x[j] -= (uint16_t) temp;
            temp = (uint32_t) (oscillator_x[j] * Multiplier[j])>>15;
            oscillator_y[j] += (uint16_t) temp;
            temp = (uint32_t) (oscillator_x[j]*Tonewheel_gain[j])>>10;
            out += (uint16_t) temp;
        }

        audiobuffer[start+i] = out;
        audiobuffer[start+i+1] = out;


	i = i+2;
    }

    GPIO_SetBits(GPIOD, GPIO_Pin_13);

}

