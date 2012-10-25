/* Q-Light USB driver v0.0.4
   Written by Kenneth Wilke */

#include "qlight.h"

int main(int argc, char *argv[])
{
	// show usage and exit if no args given
	if(argc < 2)
	{
		usage(argv[0]);
		exit(1);
	}
	
	handle_arguments(argc, argv);
	
	// initialize libusb and detect qlight
	init();
	
	// do the needful
	set_state(red, yellow, green, blue, white, sound);
	
	// cleanup
	usb_release();
	quit(0);
	return 0;
}

void usage()
{
	fprintf(stderr, "Note: Sounds and lights vary by model\n - Light states: off, on, blink\n\
   -r <state>  |  red light\n\
   -y <state>  |  yellow light\n\
   -b <state>  |  blue light\n\
   -g <state>  |  green light\n\
   -w <state>  |  white light\n\
\n\
 - Sound states: off, use 1-5 to play sound\n\
   -s <state>  |  speaker\n\n");
}

void parse_option(int *target, char *option, int lightopt)
{
	int soundopt;
	
	if(lightopt)
	{
		// handle light state options
		if(strcmp(option, "off") == 0)
		{
			*target = light_off;
		}
		else if(strcmp(option, "on") == 0)
		{
			*target = light_on;
		}
		else if(strcmp(option, "blink") == 0)
		{
			*target = light_blink;
		}
		else
		{
			fprintf(stderr, "Invalid state selection\n");
			usage();
			exit(1);
		}
	}
	else
	{
		// handle sound state options
		if(strcmp(option, "off") == 0)
		{
			*target = sound_off;
		}
		else
		{
			soundopt = atoi(option);
			if(soundopt >= 1 && soundopt <= 5)
			{
				*target = soundopt;
			}
			else
			{
				fprintf(stderr, "Sound selection invalid or out of range\n");
				usage();
				exit(1);
			}
		}
	}
}

void handle_arguments(int argc, char *argv[])
{
	int opt;
	red = light_nochange;
	yellow = light_nochange;
	blue = light_nochange;
	green = light_nochange;
	white = light_nochange;
	sound = sound_nochange;
	opterr = 0;
	
	while((opt = getopt(argc, argv, "r:y:b:g:w:s:")) != -1)
	{
		switch(opt)
		{
			case '?':
				// unhandled switches and unfilled arguments
				if(optopt == 'r' ||
					optopt == 'y' ||
					optopt == 'b' ||
					optopt == 'g' ||
					optopt == 'w' ||
					optopt == 's')
				{
					fprintf(stderr, "Option '%c' requires an argument\n", optopt);
				}
				else
				{
					fprintf(stderr, "Unhandled option '%c' given\n", optopt);
				}
				usage(argv[0]);
				exit(1);
			case 'r':
				parse_option(&red, optarg, 1);
				break;
			case 'y':
				parse_option(&yellow, optarg, 1);
				break;
			case 'b':
				parse_option(&blue, optarg, 1);
				break;
			case 'g':
				parse_option(&green, optarg, 1);
				break;
			case 'w':
				parse_option(&white, optarg, 1);
				break;
			case 's':
				parse_option(&sound, optarg, 0);
				break;
		}
	}
}

void init()
{
	libusb_device **devices;
	ssize_t deviceCount;
	struct libusb_device_descriptor descriptor;
	libusb_device *qlight = NULL;
	int i, errNo;
	
	// initialize libusb
	if(libusb_init(&context) < 0)
	{
		fprintf(stderr, "Failed to initialize libusb context\n");
		quit(1);
	}
	
	// full libusb verbosity
	libusb_set_debug(context, 3);
	
	// obtain list of devices
	deviceCount = libusb_get_device_list(context, &devices);
	if(deviceCount < 0)
	{
		fprintf(stderr, "Failed to obtain device list\n");
		libusb_free_device_list(devices, 1);
		quit(1);
	}
	
	// search for qlight in list of devices
	for(i = 0; !qlight && i < deviceCount; i++)
	{
		if(libusb_get_device_descriptor(devices[i], &descriptor) < 0)
		{
			fprintf(stderr, "Failed to obtain a device descriptor\n");
			libusb_free_device_list(devices, 1);
			quit(1);
		}
		if(descriptor.idVendor == 0x04d8 && descriptor.idProduct == 0xe73c)
		{
			printf("Q-light detected\n");
			qlight = devices[i];
		}
	}
	if(!qlight)
	{
		fprintf(stderr, "Q-light not detected\n");
		libusb_free_device_list(devices, 1);
		quit(1);
	}
	
	// open device handle
	if(libusb_open(qlight, &qlight_handle) < 0)
	{
		fprintf(stderr, "Failed to open device\n");
		libusb_free_device_list(devices, 1);
		quit(1);
	}
	
	// cleanup device list and poke kernel if necessary
	libusb_free_device_list(devices, 1);
	
	if(libusb_kernel_driver_active(qlight_handle, 0))
	{
		printf("Kernel driver is active\n");
		if(!libusb_detach_kernel_driver(qlight_handle, 0))
		{
			printf("Kernel driver detached\n");
		}
	}
	
	// mark your territory, this device is mine!
	if((errNo = libusb_claim_interface(qlight_handle, 0)) < 0)
	{
		fprintf(stderr, "Could not claim interface\n");
		
		switch(errNo)
		{
			case LIBUSB_ERROR_NOT_FOUND:
				fprintf(stderr, " Could not find interface\n");
				break;
			case LIBUSB_ERROR_BUSY:
				fprintf(stderr, " Device has been claimed by another program\n");
				break;
			case LIBUSB_ERROR_NO_DEVICE:
				fprintf(stderr, " Device not found\n");
				break;
		}
		
		libusb_close(qlight_handle);
		quit(1);
	}
	printf("Claimed interface\n");
}

void usb_release(void)
{
	if(libusb_release_interface(qlight_handle, 0) != 0)
	{
		fprintf(stderr, "Failed to release interface\n");
		libusb_close(qlight_handle);
		quit(1);
	}
	printf("Released interface\n");
	libusb_close(qlight_handle);
}

void quit(int exitCode)
{
	libusb_exit(context);
	exit(exitCode);
}

int set_state(int red, int yellow, int green, int blue, int white, int sound)
{
	int bytesWritten;
	
	unsigned char data[16];
	
	// copy template packet to local variable
	memcpy(data, data_template, 16);
	
	// fill desired state changes
	data[qlight_red] = red;
	data[qlight_yellow] = yellow;
	data[qlight_green] = green;
	data[qlight_blue] = blue;
	data[qlight_white] = white;
	data[qlight_sound] = sound;
	
	// send the packet
	if(libusb_bulk_transfer(qlight_handle, 0x1, data, 16, &bytesWritten, 0) == 0 && bytesWritten == 16)
	{
		return 1;
	}
	
	return 0;
}
