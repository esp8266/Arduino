#ifndef __PWM_H__
#define __PWM_H__

/*pwm.h: function and macro definition of PWM API , driver level */
/*user_light.h: user interface for light API, user level*/
/*user_light_adj: API for color changing and lighting effects, user level*/


 /*NOTE!!  : DO NOT CHANGE THIS FILE*/

 /*SUPPORT UP TO 8 PWM CHANNEL*/
#define PWM_CHANNEL_NUM_MAX 8 

struct pwm_param {
    uint32 period;
    uint32 freq;
    uint32  duty[PWM_CHANNEL_NUM_MAX];  //PWM_CHANNEL<=8
};


/* pwm_init should be called only once, for now  */
void pwm_init(uint32 period, uint32 *duty,uint32 pwm_channel_num,uint32 (*pin_info_list)[3]);
void pwm_start(void);

void pwm_set_duty(uint32 duty, uint8 channel);
uint32 pwm_get_duty(uint8 channel);
void pwm_set_period(uint32 period);
uint32 pwm_get_period(void);

uint32 get_pwm_version(void);
void set_pwm_debug_en(uint8 print_en);

#endif

