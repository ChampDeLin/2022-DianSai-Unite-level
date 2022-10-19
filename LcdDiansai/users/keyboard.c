/****
* 配置矩阵键盘，只需要在初始化函数中配齐GPIO引脚，然后使能即可
*****/
#include "main.h"
#include "keyboard.h"
#include "gpio.h"


#define key_delay 15     //按键消抖15ms


keyboard_T  g_keyboard;

void keyboard_init()  ///TODO,4*4的键盘
{
    keyboard_create(&g_keyboard, GPIOB,GPIO_PIN_3,
                                    GPIOB,GPIO_PIN_4,
                                    GPIOB,GPIO_PIN_5,
																		GPIOB,GPIO_PIN_8,
                                    GPIOA,GPIO_PIN_4,
                                    GPIOA,GPIO_PIN_5,
                                    GPIOA,GPIO_PIN_6,
																		GPIOA,GPIO_PIN_7);
}
/*行1-PB3、行2-PB4、行3-PB5、行4-PB8*/
/*列1-PA4、列2-PA5、列3-PA6、列4-PA7*/

void keyboard_create(keyboard_T *keyboard_t, GPIO_TypeDef * keyboardrow1_gpio,  uint32_t keyboardrow1_gpio_pin,
                                GPIO_TypeDef * keyboardrow2_gpio,  uint32_t keyboardrow2_gpio_pin,
                                GPIO_TypeDef * keyboardrow3_gpio,  uint32_t keyboardrow3_gpio_pin,
								GPIO_TypeDef * keyboardrow4_gpio,  uint32_t keyboardrow4_gpio_pin,
                                GPIO_TypeDef * keyboardcol1_gpio,  uint32_t keyboardcol1_gpio_pin,
                                GPIO_TypeDef * keyboardcol2_gpio,  uint32_t keyboardcol2_gpio_pin,
                                GPIO_TypeDef * keyboardcol3_gpio,  uint32_t keyboardcol3_gpio_pin,
								GPIO_TypeDef * keyboardcol4_gpio,  uint32_t keyboardcol4_gpio_pin )
{  
    keyboard_t->keyboardcol1_gpio = keyboardcol1_gpio;
    keyboard_t->keyboardcol2_gpio = keyboardcol2_gpio;
    keyboard_t->keyboardcol3_gpio = keyboardcol3_gpio;
	keyboard_t->keyboardcol4_gpio = keyboardcol4_gpio;

    keyboard_t->keyboardrow1_gpio = keyboardrow1_gpio;
    keyboard_t->keyboardrow2_gpio = keyboardrow2_gpio;
    keyboard_t->keyboardrow3_gpio = keyboardrow3_gpio;
	keyboard_t->keyboardrow4_gpio = keyboardrow4_gpio;

    keyboard_t->keyboardrow1_gpio_pin = keyboardrow1_gpio_pin;
    keyboard_t->keyboardrow2_gpio_pin = keyboardrow2_gpio_pin;
    keyboard_t->keyboardrow3_gpio_pin = keyboardrow3_gpio_pin;
    keyboard_t->keyboardrow4_gpio_pin = keyboardrow4_gpio_pin;

    keyboard_t->keyboardcol1_gpio_pin = keyboardcol1_gpio_pin;
    keyboard_t->keyboardcol2_gpio_pin = keyboardcol2_gpio_pin;
    keyboard_t->keyboardcol3_gpio_pin = keyboardcol3_gpio_pin;
    keyboard_t->keyboardcol4_gpio_pin = keyboardcol4_gpio_pin;
    
}

///引脚模式设置
void config_gpio_mode(GPIO_TypeDef * keyboard_gpio,  uint32_t keyboard_gpio_pin,uint32_t mode,uint32_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = keyboard_gpio_pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  ///< 高速响应快
    GPIO_InitStruct.Pull = pull;         ///< (键盘下拉)。
    HAL_GPIO_Init(keyboard_gpio, &GPIO_InitStruct);
}
/**
*   @brief 函数简要说明-引脚设置,,附带电平设置
*   @name: lkx
*   @param index    参数1 对象句柄
*   @param t        参数2 行引脚模式
*   @param t        参数3 列  模式
*   @return 返回说明
*   @LastEditTime: Do not Edit
*/
void config_gpios_mode(keyboard_T  *keyboard_t,uint32_t row_mode,uint32_t col_mode)
{
    config_gpio_mode(keyboard_t->keyboardrow1_gpio,keyboard_t->keyboardrow1_gpio_pin,row_mode,GPIO_PULLDOWN);
    config_gpio_mode(keyboard_t->keyboardrow2_gpio,keyboard_t->keyboardrow2_gpio_pin,row_mode,GPIO_PULLDOWN);
    config_gpio_mode(keyboard_t->keyboardrow3_gpio,keyboard_t->keyboardrow3_gpio_pin,row_mode,GPIO_PULLDOWN);
	config_gpio_mode(keyboard_t->keyboardrow4_gpio,keyboard_t->keyboardrow4_gpio_pin,row_mode,GPIO_PULLDOWN);

    config_gpio_mode(keyboard_t->keyboardcol1_gpio,keyboard_t->keyboardcol1_gpio_pin,col_mode,GPIO_PULLDOWN);
    config_gpio_mode(keyboard_t->keyboardcol2_gpio,keyboard_t->keyboardcol2_gpio_pin,col_mode,GPIO_PULLDOWN);
    config_gpio_mode(keyboard_t->keyboardcol3_gpio,keyboard_t->keyboardcol3_gpio_pin,col_mode,GPIO_PULLDOWN);
	config_gpio_mode(keyboard_t->keyboardcol4_gpio,keyboard_t->keyboardcol4_gpio_pin,col_mode,GPIO_PULLDOWN);

     if(row_mode == GPIO_MODE_OUTPUT_PP)
    {
        HAL_GPIO_WritePin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(keyboard_t->keyboardcol1_gpio, keyboard_t->keyboardcol1_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol2_gpio, keyboard_t->keyboardcol2_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol3_gpio, keyboard_t->keyboardcol3_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol4_gpio, keyboard_t->keyboardcol4_gpio_pin, GPIO_PIN_RESET);
    }
    if(col_mode == GPIO_MODE_OUTPUT_PP)
    {
        HAL_GPIO_WritePin(keyboard_t->keyboardcol1_gpio, keyboard_t->keyboardcol1_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol2_gpio, keyboard_t->keyboardcol2_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol3_gpio, keyboard_t->keyboardcol3_gpio_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(keyboard_t->keyboardcol4_gpio, keyboard_t->keyboardcol4_gpio_pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin, GPIO_PIN_RESET);
    }
}

/**
*   @brief 函数简要说明-按键函数
*   @name: lkx
*   @param index    参数1
*   @param t        参数2 @see CTest
*   @return 返回说明
*   @LastEditTime: Do not Edit
*/
char matrix_keyboard_scan(keyboard_T *keyboard_t)
{
    //while(1)     // 使用return,while(1)不可以去掉
    {
        ///将行设置为推挽输出 高电平；列设置为输入 低电平
        config_gpios_mode(keyboard_t,GPIO_MODE_OUTPUT_PP,GPIO_MODE_INPUT);

        if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol1_gpio, keyboard_t->keyboardcol1_gpio_pin) == GPIO_PIN_SET)
        {
            HAL_Delay(key_delay);
            if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol1_gpio, keyboard_t->keyboardcol1_gpio_pin) == GPIO_PIN_SET)
            {
                //将列设置为推挽输出 高电平；行设置为输入 低电平
                config_gpios_mode(keyboard_t,GPIO_MODE_INPUT,GPIO_MODE_OUTPUT_PP);

                if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
					    return '1';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
					    return '4';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
					    return '7';
                }
				else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
					    return '@';
                }
            }
        }
        else if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol2_gpio, keyboard_t->keyboardcol2_gpio_pin) == GPIO_PIN_SET)
        {
            HAL_Delay(key_delay);
            if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol2_gpio, keyboard_t->keyboardcol2_gpio_pin) == GPIO_PIN_SET)
            {
                //将列设置为推挽输出，行设置为输入
                config_gpios_mode(keyboard_t,GPIO_MODE_INPUT,GPIO_MODE_OUTPUT_PP);

                 if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
					    return '2';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
					    return '5';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
					    return '8';
                }
				else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
					    return '#';
                }
            }
        }
        else if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol3_gpio, keyboard_t->keyboardcol3_gpio_pin) == GPIO_PIN_SET)
        {

            HAL_Delay(key_delay);
            if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol3_gpio, keyboard_t->keyboardcol3_gpio_pin) == GPIO_PIN_SET)
            {
                //将列设置为推挽输出，行设置为输入
                config_gpios_mode(keyboard_t,GPIO_MODE_INPUT,GPIO_MODE_OUTPUT_PP);

                if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
					    return '3';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
					    return '6';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
					    return '9';
                }
				else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
					    return ';';
                }
            }
        }
		 else if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol4_gpio, keyboard_t->keyboardcol4_gpio_pin) == GPIO_PIN_SET)
        {

            HAL_Delay(key_delay);
            if(HAL_GPIO_ReadPin(keyboard_t->keyboardcol4_gpio, keyboard_t->keyboardcol4_gpio_pin) == GPIO_PIN_SET)
            {
                //将列设置为推挽输出，行设置为输入
                config_gpios_mode(keyboard_t,GPIO_MODE_INPUT,GPIO_MODE_OUTPUT_PP);

                 if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow1_gpio, keyboard_t->keyboardrow1_gpio_pin) == GPIO_PIN_SET)
					    return '%';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow2_gpio, keyboard_t->keyboardrow2_gpio_pin) == GPIO_PIN_SET)
					    return '-';
                }else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow3_gpio, keyboard_t->keyboardrow3_gpio_pin) == GPIO_PIN_SET)
					    return '0';
                }
				else if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
                {
					HAL_Delay(key_delay);
					if(HAL_GPIO_ReadPin(keyboard_t->keyboardrow4_gpio, keyboard_t->keyboardrow4_gpio_pin) == GPIO_PIN_SET)
					    return '&';
                }
            }
        }
        return 0;
    }
}















