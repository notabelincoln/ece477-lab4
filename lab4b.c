/* Abe Jordan
 * ECE 477
 * Lab 4
 * Part B
 * Uses a button (or just grounded GPIO input) to change the delay
 * and direction of a the LEDs based on 9 additional buttons.  GPIO
 * values are based around those given in the wiringPi library.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "wiringPi.h"

#define DELAY_MIN 32
#define DELAY_MAX 1024

int main(int argc, char **argv)
{
	// Variables for the LED 
	unsigned char previous_state;
	unsigned char current_state;
	unsigned char current_led;
	unsigned char i, debug_flag;
	unsigned int led_delay_ms, led_delay_ms_tmp;
	signed char led_direction, led_direction_tmp;

	unsigned char led_outputs[8] = {0,1,2,3,4,5,6,7};
	unsigned char button_input = 8;
	unsigned char delay_inputs[10] = {9,10,11,12,13,14,15,16,21,22};
	unsigned char direction_input = 23;

	// Check if debug mode is enabled
	if (argc >= 2)
		debug_flag = !strcmp("debug",argv[1]);
	else
		debug_flag = 0;

	/* 
	 * Initialize default delay, assume button does not start pressed, and
	 * make LEDs move from MS -> LS
	 */
	previous_state = 1;
	current_led = 7;
	led_delay_ms = 1024;
	led_direction = 1;

	// Set up appropriate GPIOs as outputs
	wiringPiSetup();
	for (i = 0; i < 8; i++)
		pinMode(led_outputs[i], OUTPUT);

	// Use GPIO 8 as input for "button" to change delay/direction
	pinMode(button_input, INPUT);
	pullUpDnControl(button_input, PUD_UP);

	/* Use GPIO 9 - 16, 21, 22 to determine 10-bit delay value (pressed = 1,
	 * unpressed = 0)
	*/
	for (i = 0; i < 10; i++) {
		pinMode(delay_inputs[i], INPUT);
		pullUpDnControl(delay_inputs[i], PUD_UP);
	}

	/* Use GPIO 23 to determine LED direction
	 * (unpressed MS -> LS, pressed LS -> MS)
	 */
	pinMode(direction_input, INPUT);
	pullUpDnControl(direction_input, PUD_UP);

	while(1) {
		// Change LED speed and direction based upon current status 
		current_state = digitalRead(8);

		// Determine new LED delay value
		led_delay_ms_tmp = 0;
		for (i = 0; i < 10; i++)
			led_delay_ms_tmp |= ((~(digitalRead(8 + i)) & 1) << i);
		
		// Do not change LED delay if no buttons pressed
		if (led_delay_ms_tmp == 0)
			led_delay_ms_tmp = led_delay_ms;

		// Determine whether to change LED direction
		led_direction_tmp = digitalRead(19);

		// Update values with new ones if button is pressed
		if ((current_state != previous_state) && (previous_state == 1)) {
			led_delay_ms = led_delay_ms_tmp;
			if (debug_flag) 
				printf("Delay set to %u\n",led_delay_ms);
			led_direction *= (led_direction_tmp == 0) ? -1 : 1;
			if (debug_flag)
				printf("Direction set to %s\n",
						(led_direction == 1)?"LS -> MS":"MS -> LS");
		}

		// Print debug data every delay
		if (debug_flag)
			printf("Delay: %d\nDirection: %d\n%d\n%d\n--------------\n",
					led_delay_ms,
					led_direction,
					previous_state,
					current_state);

		previous_state = current_state;

		// Update LEDs with appropriate values
		for (i = 0; i < 8; i++)
			digitalWrite(i, (i==current_led));

		current_led = (current_led + led_direction + 8) % 8;

		// Delay with appropriate time (1 ms = 1000 us)
		usleep(led_delay_ms * 1000);
	}

	// Not needed, but if it gest to here, the program executed completely
	return 0;
}
