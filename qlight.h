/* Q-Light USB driver v0.0.4
   Written by Kenneth Wilke */

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function Prototypes
void usage();
void parse_option(int *target, char *option, int lightopt);
void handle_arguments(int argc, char *argv[]);
void init();
void usb_release(void);
void quit(int exitCode);
int set_state(int red, int yellow, int green, int blue, int white, int sound);

// Globals
libusb_context *context;
libusb_device_handle *qlight_handle;
unsigned char *data_template = (unsigned char*) "\x57\x00\x64\x64\x64\x64\x64\x64\x40\x00\x54\xf3\x00\x00\x00\x00";
int red, yellow, blue, green, white, sound;

enum
{ // data index
	qlight_red = 2,
	qlight_yellow,
	qlight_green,
	qlight_blue,
	qlight_white,
	qlight_sound
};

enum
{ // light states
	light_off = 0,
	light_on,
	light_blink,
	light_nochange = 0x64
};

enum
{ // sound states
	sound_off = 0,
	/* these are device speciic */
	sound_fire,  
	sound_emergency,
	sound_ambulance,
	sound_pipipi,
	sound_pi_continue,
	sound_nochange = 0x64
};
