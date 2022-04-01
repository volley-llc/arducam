#ifndef CONTROLS_CAM_LIB
#define CONTROLS_CAM_LIB

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>

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
/**
 * @brief List of camera controls, including format.
 * Used in arguments of get/set functions.
 * 
 */
typedef const enum
{
    ACAM_BRIGHTNESS = 0,
    ACAM_CONTRAST,
    ACAM_SATURATION,
    ACAM_HUE,
    ACAM_AUTO_WHITE_BALANCE,
    ACAM_GAMMA,
    ACAM_GAIN,
    ACAM_POWER_LINE_FREQUENCY,
    ACAM_WHITE_BALANCE_TEMPERATURE,
    ACAM_SHARPNESS,
    ACAM_BACKLIGHT_COMPENSATION,
    ACAM_EXPOSURE_AUTO,
    ACAM_EXPOSURE_ABSOLUTE,
    ACAM_EXPOSURE_AUTO_PRIORITY,

    ACAM_FORMAT,

    __ACAM_CTRL_COUNT

} ctrl_tag_t;
/**
 * @brief List of supported pixel formats for the
 * ARDUCAM. Used as arguments when setting camera format;
 * corresponding int returned when getting camera format.
 * 
 */
typedef enum
{
    ACAM_MJPEG_1920_1080 = 0,
    ACAM_MJPEG_1280_1024,
    ACAM_MJPEG_1280_720,
    ACAM_MJPEG_800_600,
    ACAM_MJPEG_640_480,
    ACAM_MJPEG_320_240,

    ACAM_YUYV_1920_1080,
    ACAM_YUYV_1280_1024,
    ACAM_YUYV_1280_720,
    ACAM_YUYV_800_600,
    ACAM_YUYV_640_480,
    ACAM_YUYV_320_240,

    __ACAM_FMT_COUNT

} fmt_t;

/**
 * @brief Structure that maintains static information
 * about each of the camera's control registers.
 * 
 */
typedef struct
{
    char name[32];
    unsigned int v4l2_id;
    int min_value;
    int max_value;
    int default_val;
} ctrl_t;

/**
 * @brief The structure which maintains static info
 * about the ARDUCAM.
 * 
 */
typedef struct camera
{
    int fd;
    ctrl_t ctrls[__ACAM_CTRL_COUNT];
    uint8_t *buffer;

} camera_t;

/**
 * @brief A struct to help the user get and set
 * batches of camera controls.
 * 
 */
typedef struct ctrls_struct
{
    int value[__ACAM_CTRL_COUNT];

} ctrls_struct;


//For details on functions, refer to the comments at the top of
//each function definition in controls.c

camera_t *open_cam(const char *cam_file, int *error); //start the camera
int close_cam(camera_t *cam); //close the camera
int capture_image(camera_t *cam, const char *file_name); //captures a single image

int get_ctrl(const camera_t *cam, ctrl_tag_t ctrl, int *value); //get the current value of a control
int set_ctrl(const camera_t *cam, ctrl_tag_t ctrl, int value); //set the value of a control

int save_file(const camera_t *cam, const char *fname); //save current control values and format to external file
int load_file(const camera_t *cam, const char *fname); //load control values and format from external file into camera

int save_struct(const camera_t *cam, ctrls_struct *controls); //save a ctrls_struct with current camera control values and format
void save_default_struct(const camera_t *cam, ctrls_struct *controls); //save a ctrls_struct with default camera values and format
int load_struct(const camera_t *cam, const ctrls_struct *controls); //load control values and format from ctrls_struct into camera

int reset_ctrl(const camera_t *cam, ctrl_tag_t ctrl); //resets the value of a single control to its default
int reset_cam(const camera_t *cam); //resets all controls in camera to their defaults

int print_ctrl(const camera_t *cam, ctrl_tag_t ctrl); //print the value of a single control
int print_ctrl_all(const camera_t *cam); //print values of all controls
void print_defaults(const camera_t *cam); //print default values of all controls
void print_bounds(const camera_t *cam); //print upper and lower bounds of all controls
int print_caps(const camera_t *cam); //print camera capabilities

#endif