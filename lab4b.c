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
	char previous_state;
	char current_state;
	char current_led;
	char i, debug_flag;
	signed char led_direction, led_direction_tmp;
	unsigned int led_delay_ms, led_delay_ms_tmp;

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
		pinMode(i, OUTPUT);

	// Use GPIO 8 as input for "button" to change delay/direction
	pinMode(8, INPUT);
	pullUpDnControl(8, PUD_UP);

	/* Use GPIO 9 - 18 to determine delay value (pressed = 1,
	 * unpressed = 0)
	*/
	for (i = 0; i < 10; i++) {
		pinMode(9 + i, INPUT);
		pullUpDnControl(9 + i, PUD_UP);
	}

	/* Use GPIO 19 to determine LED direction
	 * (unpressed MS -> LS, pressed LS -> MS)
	 */
	pinMode(19, INPUT);
	pullUpDnControl(19, PUD_UP);

	while(1) {
		// Change LED speed and direction based upon current status 
		current_state = digitalRead(8);

		// Determine new LED delay value
		led_delay_ms_tmp = 0;
		for (i = 0; i < 10; i++)
			led_delay_ms_tmp |= ((~(digitalRead(8 + i)) & 1) << i);

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
			digitalWrite(((i*led_direction) + 8) % 8, (i==current_led));

		// Delay with appropriate time (1 ms = 1000 us)
		usleep(led_delay_ms * 1000);
	}

	// Not needed, but if it gest to here, the program executed completely
	return 0;
}
