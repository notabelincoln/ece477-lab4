#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "wiringPi.h"

#define BUFFER_SIZE 32

int main(int argc, char **argv)
{
	unsigned char previous_state;
	unsigned int led_delay;
	// Set up appropriate GPIOs as outputs
	wiringPiSetup();
	for (i = 0; i < 8; i++)
		pinMode(i, OUTPUT);

	// Use GPIO 8 as input for "button"
	pinMode(8, INPUT);
	pullUpDnControl(8, PUD_UP);

	while(1) {
		sleep(1);
	}
	return 0;
}
