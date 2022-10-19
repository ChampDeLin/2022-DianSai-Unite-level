/*
 * @Author: your name
 * @Date: 2021-07-22 18:09:40
 * @LastEditTime: 2022-05-13 14:38:56
 * @LastEditors: lkx 1658228948@qq.com
 * @Description: In User Settings Edit
 * @FilePath: \2021_diansai_prepare\keyboard\keyboard.h
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "main.h"

typedef struct  keyboard_T
{
    GPIO_TypeDef *      keyboardrow1_gpio;
    uint32_t            keyboardrow1_gpio_pin;

    GPIO_TypeDef *      keyboardrow2_gpio;
    uint32_t            keyboardrow2_gpio_pin;

    GPIO_TypeDef *      keyboardrow3_gpio;
    uint32_t            keyboardrow3_gpio_pin;

	GPIO_TypeDef *      keyboardrow4_gpio;
    uint32_t            keyboardrow4_gpio_pin;

    GPIO_TypeDef *      keyboardcol1_gpio;
    uint32_t            keyboardcol1_gpio_pin;

    GPIO_TypeDef *      keyboardcol2_gpio;
    uint32_t            keyboardcol2_gpio_pin;

    GPIO_TypeDef *      keyboardcol3_gpio;
    uint32_t            keyboardcol3_gpio_pin;

	GPIO_TypeDef *      keyboardcol4_gpio;
    uint32_t            keyboardcol4_gpio_pin;

}keyboard_T;





void keyboard_create(keyboard_T *keyboard_t, GPIO_TypeDef * keyboardrow1_gpio,  uint32_t keyboardrow1_gpio_pin,
                                GPIO_TypeDef * keyboardrow2_gpio,  uint32_t keyboardrow2_gpio_pin,
                                GPIO_TypeDef * keyboardrow3_gpio,  uint32_t keyboardrow3_gpio_pin,
								GPIO_TypeDef * keyboardrow4_gpio,  uint32_t keyboardrow4_gpio_pin,

                                GPIO_TypeDef * keyboardcol1_gpio,  uint32_t keyboardcol1_gpio_pin,
                                GPIO_TypeDef * keyboardcol2_gpio,  uint32_t keyboardcol2_gpio_pin,
                                GPIO_TypeDef * keyboardcol3_gpio,  uint32_t keyboardcol3_gpio_pin,
								GPIO_TypeDef * keyboardcol4_gpio,  uint32_t keyboardcol4_gpio_pin );


char matrix_keyboard_scan(keyboard_T *keyboard_t);
void keyboard_init(void);
void config_gpio_mode(GPIO_TypeDef * gpio,  uint32_t gpio_pin,uint32_t mode,uint32_t pull);
extern keyboard_T  g_keyboard;


#define KEY_OK '&'
#define KEY_CANCEL  ';'

#endif

