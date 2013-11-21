# Q-light driver

## Compiling

Compiling should be simple, just clone the repo and run make within it.
Compiling does require libusb

    $ git clone https://github.com/KennethWilke/qlight_userspace
    $ cd qlight_userspace
    $ make

## Usage

To run the driver you need to be root or a user in the plugdev group.

Run the qlight binary to change the state of the lights and sound device for
the qlight tower. 

    # This will turn on the red light
    $ ./qlight -r on

Lights may be on, off or blinking. Light options depends on what model you are
using. Use r, y, b, g or w switches for red, yellow, blue, green and white
respectively. These options can be combined.

    # Turn off red light, make green blink, and white on
    $ ./qlight -r off -g blink -w off

Sound is controled with the s flag. As with lights the sounds differ with each
model, use the number 1-5 to choose.

    # Turn on the second sound
    $ ./qlight -s 2

    # Turn sound back off
    $ ./qlight -s off
