/*
 * sensor.c
 *
 *  Created on: Apr 19, 2019
 *      Author: nachi
 */

#include <stdint.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_adc.h"
#include "gpio.h"
#include "log.h"
#include "sensor.h"
#include "gpiointerrupt.h"


void motion_sensor_init()
{
	/* Configure port C pin 12 as input */
	GPIO_PinModeSet(MOTION_PORT, MOTION_PIN, gpioModeInput, 0);
	GPIO_ExtIntConfig(MOTION_PORT, MOTION_PIN, MOTION_PIN, true, true, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();
	GPIOINT_CallbackRegister(11, MotionDetected);
}


//int motion_sensor_data()
//{
//	/* Return the value on port C pin 11 */
//	return GPIO_PinInGet(MOTION_PORT, MOTION_PIN);
//}


void sound_sensor_init()
{
	// Enable ADC0 clock
//	  Configure GPIO pin as output to give power to the sensor
	  CMU_ClockEnable(cmuClock_ADC0, true);

	  ADC_Init_TypeDef initializing_config = ADC_INIT_DEFAULT;
	  ADC_InitSingle_TypeDef init_single_conv = ADC_INITSINGLE_DEFAULT;
	  initializing_config.prescale = ADC_PrescaleCalc(13000000, 0);
	  init_single_conv.diff       = false;              // single ended
	  init_single_conv.reference  = adcRef2V5;          // internal 2.5V reference
	  init_single_conv.resolution = adcRes12Bit;        // 12-bit resolution
	  init_single_conv.posSel = adcPosSelAPORT1XCH16;   // Select ADC0 port1x channel 16 as ADC input
//	  init_single_conv.posSel = adcPosSelAPORT1YCH23;

	  ADC_Init(ADC0, &initializing_config);             // Initialize the ADC
	  ADC_InitSingle(ADC0, &init_single_conv);          // Initialize the ADC for single conversion
}


uint32_t get_sound_data()
{
	START_CONVERSION;
	return ADC_DataSingleGet(ADC0);
}


void init_human_presence_sensors()
{
	motion_sensor_init();
	sound_sensor_init();
}


//bool is_human_present()
//{
//	uint32_t sound_data;
//	int motion_data;
//	bool human_present = false;
//	motion_data = motion_sensor_data();
////	LOG_INFO("Motion data is %d",motion_data);
//	sound_data  = get_sound_data();
////	LOG_INFO("Sound data is %d",sound_data);
//	if(motion_data == MOTION_DETECTED && sound_data > SOUND_THRESHOLD)
//	{
//		human_present = true;
//	}
////	LOG_INFO("Is human present %d",human_present);
//	return human_present;
//}


float get_occupancy(uint64_t occupied_time, uint64_t total_time)
{
//	LOG_INFO("Occupied time = %d",occupied_time);
//	LOG_INFO("Total time = %d",total_time);
//	LOG_INFO("Occupied time * 100 = %d",occupied_time*100);
//	LOG_INFO("Occupied time *100/total time = %d",(occupied_time*100)/total_time);
	return ((occupied_time* 100) / total_time);
}
