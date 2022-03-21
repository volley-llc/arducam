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

struct camera{
    int fd;
    int values[14];

};

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


int set_ctrl(int ctrl, int value, struct camera *cam);
int get_ctrl(struct camera *cam, int ctrl, int* value);
int write_ctrls_to_file(struct camera *cam);
int load_ctrls_from_file(struct camera *cam);
int restore_defaults(struct camera *cam);
struct camera* boot_camera();
void camera_close(struct camera *cam);


#endif