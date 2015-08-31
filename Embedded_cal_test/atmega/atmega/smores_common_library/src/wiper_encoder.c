/*
 * drive_coil.c
 *
 * Created: 2/10/2015
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "wiper_encoder.h"

//#define TILT_WIPER

static int adc0_0_eeprom = 0;
static int adc0_180_eeprom = 0;
static int adc1_0_eeprom = 0;  // zero degree for tilt wiper
static int adc1_180_eeprom = 0;
static float resolu_0_eeprom = 0;
static float resolu_1_eeprom = 0;

void get_eeprom() {
    adc0_0_eeprom = eeprom_read_word((uint16_t*) 4);
    adc1_0_eeprom = eeprom_read_word((uint16_t*) 8);
    adc0_180_eeprom = eeprom_read_word((uint16_t*) 12);
    adc1_180_eeprom = eeprom_read_word((uint16_t*) 16);
    resolu_0_eeprom = eeprom_read_float((float*) 24);
    resolu_1_eeprom = eeprom_read_float((float*) 32);
}

#define LOW 0

#ifndef TILT_WIPER
float return_position(int adc0, int adc1);
#endif
#ifdef TILT_WIPER
float return_position(int adc0);
#endif

int adc_value[2];
int adc_value_return[2];
/* volatile float global_time; */
/* volatile float previous_time; */
float current_position_deg;
volatile int current_position;
volatile float previous_position_deg;
float current_velocity_deg;
long current_velocity;

void init_wiper_encoder(void)
{
    ADC_Init ADC_test;
    ADC_test.ADC_CHANNEL = ADC0;
    ADC_test.ADC_REF = VCC;
    ADC_test.ADC_PRESCALAR = 64;
    ADC_test.ADC_MODE = FREE_RUN;

    adc_init(ADC_test);

    adcInterruptEnable;
    adcEnable;
    adcAutoTriggerEnable;
    adcStart;

    set(TCCR2B, CS22);
    set(TCCR2B, CS21);
    set(TCCR2B, CS20);

    set(TCCR2A, WGM21);
    clear(TCCR2A, WGM20);
    clear(TCCR2B, WGM22);

    OCR2A = 75;

    set(TIMSK2, OCIE2A);

    /* previous_time = 0; */
    previous_position_deg = 0;
    /* global_time = 0; */

}

int return_current_position(void)
{
    return current_position;
}

long return_current_velocity(void)
{
    return current_velocity;
}

/* int return_adc0(void)
{
    return adc_value_return[0];
}
#ifndef TILT_WIPER
int return_adc1(void)
{
    return adc_value_return[1];
}
#endif */

int return_adc0(void)
{
    return adc_value[0];
}
#ifndef TILT_WIPER
int return_adc1(void)
{
    return adc_value[1];
}
#endif

ISR(ADC_vect)
{
#ifndef TILT_WIPER
//    static ADC_Channel ADC_channel = ADC0;
    if ((ADMUX & 0x0F) == 0) {
        adc_value[0] = ADC;
        ADC = 0;
        adc_set_channel(ADC1);
    } else
    {
        adc_value[1] = ADC;
        ADC = 0;
        adc_set_channel(ADC0);
    }
    int i = 0;
    for (i = 0; i < 375; i++) {
        m_yellow(TOGGLE);
    }
#endif
#ifdef TILT_WIPER
    adc_value[0] = ADC;
#endif
//    ADC_channel = !ADC_channel;
//    adc_set_channel(ADC_channel);
    set(ADCSRA, ADIF);
}

ISR(TIMER2_COMPA_vect)
{
    /* m_blue(TOGGLE); */
    static float previous_velocity_deg;
    /* previous_time = global_time; */
    /* global_time = GetTime(); */
    /* float dt = global_time - previous_time; */
    /* delta_time = dt * 10000; */
    previous_position_deg = current_position_deg;
#ifndef TILT_WIPER
    current_position_deg = return_position(adc_value[0], adc_value[1]);
    /* current_position_deg = return_position(adc_value[0], adc_value[1]); */
    if ((previous_position_deg > 90) && (current_position_deg <= 0))
    {
        previous_position_deg = previous_position_deg - 360;
    }

    if ((previous_position_deg < -90) && (current_position_deg >= 0))
    {
        previous_position_deg = previous_position_deg + 360;
    }
#endif
#ifdef TILT_WIPER
    current_position_deg = return_position(adc_value[0]);
#endif
    float diff_position = current_position_deg - previous_position_deg;
    /* if (abs(diff_position) > 300) */
    /* { */
    /*     diff_position = -(current_position_deg + previous_position_deg) * diff_position / abs(diff_position); */
    /* } */
    /* if ((diff_position < (RESOLUTION)) && (diff_position > (-RESOLUTION))) */
    /* { */
    /*     diff_position = 0; */
    /*     current_position_deg = previous_position_deg; */
    /* } */
    current_position = current_position_deg / 180 * 32767;
    current_velocity_deg = 0.2*diff_position / 0.0096 + 0.8*previous_velocity_deg;
    /* current_velocity_deg = diff_position / 0.0096; */
    /* current_velocity_deg = medfilter(current_velocity_deg); */
    previous_velocity_deg = current_velocity_deg;
    current_velocity = current_velocity_deg / 180 * 32767;
}

#ifndef TILT_WIPER
float return_position(int adc0, int adc1)
{
    static bool flag_adc0 = 1;
//    bool flag_plus_flip = 0;
//    bool flag_minus_flip = 0;
    adc_value_return[0] = adc0;
    adc_value_return[1] = adc1;
//    uint8_t threshold = 30;  // threshold to filter noisy adc1
    float current_position;
//    static int previous_adc1;
    static float previous_position;
    if ((adc1 >= adc1_0_eeprom) && (adc1 <= adc1_180_eeprom) && (flag_adc0))
    {
        current_position = (adc1 - adc1_0_eeprom) * resolu_1_eeprom + LOW;
    }
    else
    {
        flag_adc0 = 0;
        current_position = (adc0 - adc0_180_eeprom) * resolu_0_eeprom + LOW;
        if((adc0 <= adc0_0_eeprom) || (adc0 >= adc0_180_eeprom))
        {
            flag_adc0 = 1;
        }
    }
    
    /* if(abs(adc1 - previous_adc1) > threshold) */
    /* { */
    /*     current_position = (adc0 - adc0_180_eeprom) * resolu_0_eeprom - 86; */
    /* } */
    
    if ((previous_position > 90) && (current_position <= 0))
    {
        previous_position = previous_position - 360;
//        current_position = current_position + 360;
//        flag_plus_flip = 1;
    }

    if ((previous_position < -90) && (current_position >= 0))
    {
        previous_position = previous_position + 360;
//        current_position = current_position - 360;
//        flag_minus_flip = 1;
    }

    /* if (abs(current_position - previous_position) > 10) */
    /* { */
    /*     current_position = (adc0 - adc0_180_eeprom) * resolu_0_eeprom - 86; */
    /* } */
    
    current_position = 0.3*current_position + 0.7*previous_position;
//    current_position = medfilter(current_position);
    /* if (flag_plus_flip) */
    /* { */
    /*     current_position = current_position - 360; */
    /* } */
    /* if (flag_minus_flip) */
    /* { */
    /*     current_position = current_position + 360; */
    /* } */
    if (current_position > 180)
    {
        current_position = 180;
    }
    if (current_position < -180)
    {
        current_position = -180;
    }
    previous_position = current_position;
//    previous_adc1 = adc1;
    /* float diff = current_position - previous_position; */
    /* if (diff > threshold) */
    /* { */
    /*     m_blue(ON); */
    /*     current_position = (adc0 - adc0_180_eeprom) * resolu_0_eeprom - 86; */
    /* } */
    return current_position;
    //return diff;
}
#endif

#ifdef TILT_WIPER
float return_position(int adc0)
{
    static float previous_position = 0;
    adc_value_return[0] = adc0;
    float current_position = 0;
    current_position = (adc0 - adc1_0_eeprom) * resolu_0_eeprom;
    current_position = 0.3*current_position + 0.7*previous_position;
    previous_position = current_position;
    return current_position;
}

#endif


int8_t sign(float x)
{
    if(x > 0)
        return 1;
    else
        return -1;
}
