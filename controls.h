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
typedef enum controls
{
    BRIGHTNESS = 0,
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
    EXPOSURE_AUTO_PRIORITY,

    FORMAT,

    CAM_CTRL_COUNT

} ctrl_tag;

typedef enum recording_formats
{
    MJPEG_1920_1080 = 0,
    MJPEG_1280_1024,
    MJPEG_1280_720,
    MJPEG_800_600,
    MJPEG_640_480,
    MJPEG_320_240,

    YUYV_1920_1080,
    YUYV_1280_1024,
    YUYV_1280_720,
    YUYV_800_600,
    YUYV_640_480,
    YUYV_320_240,
} format;

typedef struct Control
{
    unsigned char name[32];
    unsigned int v4l2_id;
    int min_value;
    int max_value;
    int default_val;
} Control;

typedef struct camera
{
    int fd;
    Control controls[CAM_CTRL_COUNT - 1];
    uint8_t *buffer;

} camera;

typedef struct ctrls_struct
{
    int value[CAM_CTRL_COUNT];

} ctrls_struct;

int print_caps(camera *cam);
int init_mmap(camera *cam);
int capture_image(camera *cam, char *file_name);
int set_fmt(camera *cam, format fmt_tag);
int get_fmt(camera *cam, int *value);
int print_ctrls(camera *cam);
int save_struct(camera *cam, ctrls_struct *controls);
int load_struct(camera *cam, ctrls_struct *controls);
int set_ctrl(camera *cam, ctrl_tag ctrl, int value);
int get_ctrl(camera *cam, ctrl_tag ctrl, int *value);
int save_file(camera *cam, const char *fname);
int load_file(camera *cam, const char *fname);
int reset(camera *cam);
int boot_camera(camera *cam, char *cam_file);
int get_queryctrl(camera *cam, ctrl_tag ctrl, struct v4l2_queryctrl *query_out);
int close_cam(camera *cam);
int reset_ctrl(camera *cam, ctrl_tag ctrl);

#endif