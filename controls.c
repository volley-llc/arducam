#include "controls.h"

uint8_t* buffer;

//get
//set
//defaults handling
//open
//close
//get ranges

//IMPORTANT: does saving to and from file mean the video0 file, or a new, raw text file?
//Also, if we do save to the file, do we want all at once? not at all?
//should this thing run from the command line?

#define CLEAR(x) memset(&(x), 0, sizeof(x))


int xioctl(int fd, int request, void *arg)
{
    int r;
        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);
        return r;
}

unsigned int get_ctrl_id(int ctrl){
    unsigned int ctrl_id;
    switch(ctrl){
        case BRIGHTNESS:
            ctrl_id = V4L2_CID_BRIGHTNESS;
            break;
        case CONTRAST:
            ctrl_id = V4L2_CID_CONTRAST;
            break;
        case SATURATION:
            ctrl_id = V4L2_CID_SATURATION;
            break;
        case HUE:
            ctrl_id = V4L2_CID_HUE;
            break;
        case AUTO_WHITE_BALANCE:
            ctrl_id = V4L2_CID_AUTO_WHITE_BALANCE;
            break;
        case GAMMA:
            ctrl_id = V4L2_CID_GAMMA;
            break;
        case GAIN:
            ctrl_id = V4L2_CID_GAIN;
            break;
        case POWER_LINE_FREQUENCY:
            ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY;
            break;
        case WHITE_BALANCE_TEMPERATURE:
            ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
            break;
        case SHARPNESS:
            ctrl_id = V4L2_CID_SHARPNESS;
            break;
        case BACKLIGHT_COMPENSATION:
            ctrl_id = V4L2_CID_BACKLIGHT_COMPENSATION;
            break;
        case EXPOSURE_AUTO:
            ctrl_id = V4L2_CID_EXPOSURE_AUTO;
            break;
        case EXPOSURE_ABSOLUTE:
            ctrl_id = V4L2_CID_EXPOSURE_ABSOLUTE;
            break;
        case EXPOSURE_AUTO_PRIORITY:
            ctrl_id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
            break;
        

        default:
            printf("INVALID CONTROL ENTERED");

    }

    return ctrl_id;
}

int get_ctrl(int fd, int ctrl){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    
    int ret = ioctl(fd, VIDIOC_G_CTRL, &control);


    if (ret == -1) {
        printf("IOCTL failed, Errno: %d\n", errno);
        return -100; //handle this
    } else {

        return (control.value);

    }

}

int set_ctrl(int fd, int ctrl, int value){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    control.value = value;

    int ret = ioctl(fd, VIDIOC_S_CTRL, &control);


    if (ret == -1) {
        printf("IOCTL failed, Errno: %d\n", errno);
        return -100;
    } else {

        return (control.value);

    }

}

int write_params(int fd){

    char *ctrl_strings[14] =
        {"BRIGHTNESS",
        "CONTRAST",
        "SATURATION",
        "HUE",
        "AUTO_WHITE_BALANCE",
        "GAMMA",
        "GAIN",
        "POWER_LINE_FREQUENCY",
        "WHITE_BALANCE_TEMPERATURE",
        "SHARPNESS",
        "BACKLIGHT_COMPENSATION",
        "EXPOSURE_AUTO",
        "EXPOSURE_ABSOLUTE",
        "EXPOSURE_AUTO_PRIORITY"};

    for (int i =0; i < EXPOSURE_AUTO_PRIORITY; i++){
        printf("%s ",ctrl_strings[i]);
        printf("%d\n", get_ctrl(fd, i));
        
        
    }


    

    return 0;

}

