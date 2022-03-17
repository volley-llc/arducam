#pragma once
#ifndef CONTROLS
#define CONTROLS

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <linux/usb/video.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>
#include <jpeglib.h>

#include <linux/usb/ch9.h>

enum controls{
BRIGHTNESS,
CONTRAST,
SATURATION,
HUE,
AUTO_WHITE_BALANCE,
GAMMA,
GAIN,
POWER_LINE_FREQUENCY,
WHITE_BALANCE_TEMPERATURE,
SHARPNESS,
BACKLIGHT_COMPENSATION,
EXPOSURE_AUTO,
EXPOSURE_ABSOLUTE,
EXPOSURE_AUTO_PRIORITY
};


int xioctl(int fd, int request, void *arg);
int set_ctrl(int fd, int ctrl, int value);
int get_ctrl(int fd, int ctrl);
int write_params(int fd);


#endif