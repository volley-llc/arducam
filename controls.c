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

char *ctrl_strings[14] ={
    "BRIGHTNESS ",
    "CONTRAST ",
    "SATURATION ",
    "HUE ",
    "AUTO_WHITE_BALANCE ",
    "GAMMA ",
    "GAIN ",
    "POWER_LINE_FREQUENCY ",
    "WHITE_BALANCE_TEMPERATURE ",
    "SHARPNESS ",
    "BACKLIGHT_COMPENSATION ",
    "EXPOSURE_AUTO ",
    "EXPOSURE_ABSOLUTE ",
    "EXPOSURE_AUTO_PRIORITY "
};

int ctrl_bounds[14][2] = {
    {-64, 64}, //BRIGHTNESS
    {0, 64}, //CONTRAST
    {0, 128}, //SATURATION
    {-40, 40}, //HUE
    {0, 1}, //WHITE_BALANCE_TEMPERATURE_AUTO
    {72, 500}, //GAMMA
    {0,100}, //GAIN
    {0,2}, //POWER_LINE_FREQUENCY
    {2800, 6500}, //WHITE_BALANCE_TEMPERATURE
    {0,8}, //SHARPNESS
    {0,2}, //BACKLIGHT_COMPENSATION
    {0,3}, //EXPOSURE_AUTO
    {1,5000}, //EXPOSURE_ABSOLUTE
    {0,1} //EXPOSURE_AUTO_PRIORITY

};

int ctrl_default[14] = {
    0, //BRIGHTNESS
    32, //CONTRAST
    64, //SATURATION
    0, //HUE
    1, //WHITE_BALANCE_TEMPERATURE_AUTO
    100, //GAMMA
    0, //GAIN
    1, //POWER_LINE_FREQUENCY
    4600, //WHITE_BALANCE_TEMPERATURE
    3, //SHARPNESS
    1, //BACKLIGHT_COMPENSATION
    3, //EXPOSURE_AUTO
    156, //EXPOSURE_ABSOLUTE
    0 //EXPOSURE_AUTO_PRIORITY

};

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

int get_ctrl(struct camera *cam, int ctrl, int* value){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    
    int ret = ioctl(cam->fd, VIDIOC_G_CTRL, &control);

    if (ret == -1) {
        perror("IOCTL failed");
        return errno;
    } else {

        *value = control.value;
        return 0;

    }

}

int set_ctrl(int ctrl, int value, struct camera *cam){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    control.value = value;

    int ret = ioctl(cam->fd, VIDIOC_S_CTRL, &control); //set camera's control value to @param value

    if (ctrl == WHITE_BALANCE_TEMPERATURE && cam->values[AUTO_WHITE_BALANCE] == 1){
        printf("Cannot set WHITE_BALANCE_TEMPERATURE while AUTO_WHITE_BALANCE is on.");

    }

    if (ctrl == EXPOSURE_ABSOLUTE && cam->values[EXPOSURE_AUTO] == 3){
        printf("Cannot set EXPOSURE_ABSOLUTE while EXPOSURE_AUTO is set to 3. Set EXPOSURE_AUTO to 1 to adjust exposure.");

    }

    if (ret == -1) {
        perror("IOCTL failed");
        return errno;
    } else {

        if (value < ctrl_bounds[ctrl][0]){
            printf("WARNING: Set value for %s was less than lower bound. %s was automatically set to lower bound: %d\n", 
            ctrl_strings[ctrl],ctrl_strings[ctrl],ctrl_bounds[ctrl][0]);
        }

        else if (value > ctrl_bounds[ctrl][1]){
            printf("WARNING: Set value for %s exceeds upper bound. %s was automatically set to upper bound: %d\n"
            , ctrl_strings[ctrl],ctrl_strings[ctrl],ctrl_bounds[ctrl][1]);
        }

        get_ctrl(cam, ctrl, &cam->values[ctrl]); //update the ctrl_vals struct with new value
        return (0);

    }

}

int write_ctrls_to_file(struct camera *cam){

    FILE *out = fopen("ctrl_values.txt", "w+");
    

    for (int i =0; i < EXPOSURE_AUTO_PRIORITY; i++){
        char val_char[5];
        sprintf(val_char, "%d\n", cam->values[i]);
        fputs(ctrl_strings[i], out);
        fputs(val_char, out);
    }

    fclose(out);
    

    return 0;

}

int load_ctrls_from_file(struct camera *cam){

    FILE *in = fopen("ctrl_values.txt", "r");
    if (in == NULL){
        perror("Unable to find ctrl_values.txt -- cannot read file");
        return (-1);
    }
    char line[50];
    int i = 0;
    while (fgets(line, sizeof(line), in) !=NULL){
        char *ctrl_name = strtok(line, " ");
        char *ctrl_val_str = strtok(NULL, "\n");
        int ctrl_val = atoi(ctrl_val_str);
        set_ctrl(i, ctrl_val, cam);
        i = i + 1;
    }

    return 0;

}

int restore_defaults(struct camera *cam){

    for (int i = 0; i < EXPOSURE_AUTO_PRIORITY; i++){

        set_ctrl(i, ctrl_default[i], cam);

    }

    return 0;

}

//On boot, should the camera restore defaults?
struct camera* boot_camera(){

    struct camera *cam = malloc(sizeof(struct camera));

    int fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
    cam->fd = fd;
    
    for (int i = 0; i < 14; i++){
        get_ctrl(cam, i, &cam->values[i]);
    }

    return (cam);


}

void camera_close(struct camera *cam){

    close(cam->fd);
    free(cam);


}