/*
 * sensor.h
 *
 *  Created on: Apr 19, 2019
 *      Author: nachi
 */

#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#include <stdbool.h>

#define MOTION_DETECTED 1
#define SOUND_THRESHOLD 25

#define START_CONVERSION ADC_Start(ADC0, adcStartSingle);

/*
 * Function    : motion_sensor_init
 * Description : Initializing the gpio pins for using motion sensor.
 * @param      : void
 * @return     : void
 */
void motion_sensor_init();


/*
 * Function    : motion_sensor_data
 * Description : Get the data from the motion sensor if it has detected motion.
 * @param      : void
 * @return     : int(the pin value)
 */
//int motion_sensor_data();


/*
 * Function    : sound_sensor_init
 * Description : Initializing the ADC0 port1x channel 16 to get the audio data
 *               from the sound sensor.
 * @param      : void
 * @return     : void
 */
void sound_sensor_init();


/*
 * Function    : get_sound_data
 * Description : Get the data from ADC0
 * @param      : void
 * @return     : uint32_t
 */
uint32_t get_sound_data();


/*
 * Function    : init_human_presence_sensors
 * Description : Initialize both the sensors
 * @param      : void
 * @return     : void
 */
void init_human_presence_sensors();


/*
 * Function    : is_human_present
 * Description : This function gets the data from motion and sound sensor and
 *               decides if it has detected the human and return true if the
 *               motion and sound is present indicating that the human is present.
 * @param      : void
 * @return     : bool (true if human is present and false if human is not present)
 */
//bool is_human_present();


/*
 * Function    : display_occupancy
 * Description : This function gets the occupied time and total time and returns the
 *               percentage of time the place was occupied.
 * @param      : uint64_t(occupied time), uint64_t(total_time)
 * @return     : float(percentage of time the room was occupied)
 */
float get_occupancy(uint64_t occupied_time, uint64_t total_time);
#endif /* SRC_SENSOR_H_ */
