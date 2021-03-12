/* Abe Jordan
 * ECE 477
 * Lab 4
 * Part A
 * Uses a button (or just grounded GPIO input) to change the delay
 * and direction of a the LEDs
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
	signed char led_direction;
	unsigned int led_delay_ms;

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

	// Use GPIO 8 as input for "button"
	pinMode(8, INPUT);
	pullUpDnControl(8, PUD_UP);

	while(1) {


		// Change LED speed and direction based upon current status 
		current_state = digitalRead(8);
		if ((current_state != previous_state) && (previous_state == 1)) {
			if ((led_delay_ms >= (DELAY_MIN*2)) && (led_direction == 1)) {
				led_delay_ms /= 2;
				if (debug_flag) 
					printf("Decreased delay\n");
			} else if (led_direction == 1) {
				led_direction = -1;
				if (debug_flag)
					printf("Swapped from LS -> MS to MS -> LS\n");
			} else if ((led_delay_ms <= (DELAY_MAX/2)) && (led_direction == -1)) {
				led_delay_ms *= 2;
				if (debug_flag)
					printf("Delay increased\n");
			} else if (led_direction == -1) {
				led_direction = 1;
				if (debug_flag)
					printf("Swapped from MS -> LS to LS -> MS\n");
			}

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
