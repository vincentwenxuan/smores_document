/*
 * drive_coil.c
 *
 * Created: 2/10/2015
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "wiper_encoder.h"

//#define TILT_WIPER

//#define ADC1_MIN  112  // -86
//#define ADC1_MIN 101  // -91
//#define ADC1_MAX 750  // 180
//#define ADC1_MAX 884
//#define ADC1_ZERO 341  // 0
//#define ADC1_ZERO 359
//#define ADC0_MIN  123  // -180
//#define ADC0_MIN 129
//#define ADC0_MAX  374  // -86
//#define ADC0_MAX 393  // -91

/* #define ADC1_MIN 99 // -87 */
/* #define ADC1_MAX 868  // 180 */
/* #define ADC1_ZERO 352  // 0 */
/* #define ADC0_MIN 116  // -180 */
/* #define ADC0_MAX 385  // -87 */

/* #define ADC1_MIN 124 // -88 */
/* //#define ADC1_MAX 897  // 180 */
/* #define ADC1_MAX 870 // -170 */
/* #define ADC1_ZERO 380  // 0 */
/* #define ADC0_MIN 146  // -180 */
/* #define ADC0_MAX 412  // -88 */

/* #define RESOLUTION_1 0.3467 */
/* #define RESOLUTION_0 0.3459 */

#ifndef TILT_WIPER
// left wiper
/* #define ADC1_MIN 486 // 0 */
/* #define ADC1_MAX 916  // 180 */
/* #define ADC0_MIN 145  // -180 */
/* #define ADC0_MAX 683  // -90 */
/* #define LOW 0 */

/* #define RESOLUTION_1 0.4186 */
/* #define RESOLUTION_0 0.3346 */

/* #define ADC1_MIN 100 // -90 */
/* #define ADC1_MAX 729  // 180 */
/* //#define ADC1_ZERO   // 0 */
/* #define ADC0_MIN 87  // -180 */
/* #define ADC0_MAX 298  // -90 */
/* #define LOW -90 */

/* #define RESOLUTION_1 0.4293 */
/* #define RESOLUTION_0 0.4265 */


// right wiper
/* #define ADC1_MIN 134 // -90 */
/* #define ADC1_MAX 852  // 180 */
/* #define ADC0_MIN 117  // -180 */
/* #define ADC0_MAX 359  // -90 */
/* #define LOW -90 */

/* #define RESOLUTION_1 0.3760 */
/* #define RESOLUTION_0 0.3719 */

#define ADC1_MIN 365 // 0
#define ADC1_MAX 890  // 180
//#define ADC1_ZERO 404  // 0
#define ADC0_MIN 107  // -180
#define ADC0_MAX 616  // 0
#define LOW 0

#define RESOLUTION_1 0.3429
#define RESOLUTION_0 0.3536


// top wiper
/* #define ADC1_MIN 352 // 0 */
/* #define ADC1_MAX 901  // 180 */
/* #define ADC0_MIN  85 // -180 */
/* #define ADC0_MAX 561  // 0 */
/* #define LOW 0 */

/* #define RESOLUTION_1 0.3279 */
/* #define RESOLUTION_0 0.3782 */

/* #define ADC1_MIN 153 // -90 */
/* #define ADC1_MAX 904  // 180 */
/* #define ADC1_ZERO 380  // 0 */
/* #define ADC0_MIN  142 // -180 */
/* #define ADC0_MAX 377  // -90 */
/* #define LOW -90 */

/* #define RESOLUTION_1 0.3595 */
/* #define RESOLUTION_0 0.3830 */
#endif

#ifdef TILT_WIPER
#define ADC0_MIN  24
#define ADC0_MAX  924
#define ADC0_ZERO 474
#define RESOLUTION_0  0.2

/* #define ADC0_MIN  9 */
/* #define ADC0_MAX  985 */
/* #define ADC0_ZERO 497 */
/* #define RESOLUTION_0  0.1844 */
#endif

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
    if ((adc1 >= ADC1_MIN) && (adc1 <= ADC1_MAX) && (flag_adc0))
    {
        current_position = (adc1 - ADC1_MIN) * RESOLUTION_1 + LOW;
    }
    else
    {
        flag_adc0 = 0;
        current_position = (adc0 - ADC0_MAX) * RESOLUTION_0 + LOW;
        if((adc0 <= ADC0_MIN) || (adc0 >= ADC0_MAX))
        {
            flag_adc0 = 1;
        }
    }
    
    /* if(abs(adc1 - previous_adc1) > threshold) */
    /* { */
    /*     current_position = (adc0 - ADC0_MAX) * RESOLUTION_0 - 86; */
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
    /*     current_position = (adc0 - ADC0_MAX) * RESOLUTION_0 - 86; */
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
    /*     current_position = (adc0 - ADC0_MAX) * RESOLUTION_0 - 86; */
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
    current_position = (adc0 - ADC0_ZERO) * RESOLUTION_0;
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
