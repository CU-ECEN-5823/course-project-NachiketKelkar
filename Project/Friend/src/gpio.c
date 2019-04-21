/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>
#include "log.h"


#define	LED0_port gpioPortF
#define LED0_pin	4
#define LED1_port gpioPortF
#define LED1_pin 5

void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);
	SET_BUTTON0_INPUT;
	SET_BUTTON1_INPUT;
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}


/*
 * Function name: gpioEnableDisplay();
 * Description: This function enables the display. The temperature sensor and LCD
 * has same enable pin hence the temperature_sensor_enable macro is reused.
 * @param: void
 * @return: void
 */
void gpioEnableDisplay()
{
	LCD_ENABLE;
}

/*
 * Function name: gpioSetDisplayExtcomin();
 * Description: The Extcomin pin of the LCD needs to be toggled every one sec.
 * @param: void
 * @return: void
 */
void gpioSetDisplayExtcomin(bool high)
{
	if(high == true)
	{
		DISPLAY_EXTCOMIN_LOW;
//		LOG_INFO("In if");
	}
	else
	{
		DISPLAY_EXTCOMIN_HIGH;
//		LOG_INFO("In else");
	}
}
