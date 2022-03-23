#include "controls.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

int get_fmt(camera *cam){
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1920;
    fmt.fmt.pix.height = 1080;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        perror("Setting Pixel Format");
        return 1;
    }

    printf("%s", fmt.fmt.pix.width);

}

int get_ctrl_id(ctrl_tag ctrl){
    int ctrl_id;
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
            //fprintf(stderr, "Invalid control entered. ");
            return 1;

    }

    return ctrl_id;
}

int get_ctrl_struct(ctrls_struct *controls, ctrl_tag ctrl){
    int *ptr = &(controls->brightness);
    ptr = ptr + ctrl;
    return *ptr;

}

void set_ctrl_struct(ctrls_struct *controls, ctrl_tag ctrl, int value){
    int *ptr = &(controls->brightness);
    ptr = ptr + ctrl;
    *ptr = value;
}

int get_ctrl(camera *cam, ctrl_tag ctrl, int* value){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    int ret = ioctl(cam->fd, VIDIOC_G_CTRL, &control);

    if (ret == -1) {
        if (control.id == 1)
            perror("IOCTL failed because of ctrl argument");
        else
            perror("IOCTL failed because of value argument");
        return errno;
    } else {

        *value = control.value;
        return 0;

    }

}

int set_ctrl(camera *cam, ctrl_tag ctrl, int value){
    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    control.value = value;

    int ret = ioctl(cam->fd, VIDIOC_S_CTRL, &control); //set camera's control value to @param value


    /////////////////// BEGIN CHECKING FOR EDGE CASES ///////////////////////////////////////
    if (ctrl == WHITE_BALANCE_TEMPERATURE){
        int value;
        get_ctrl(cam, AUTO_WHITE_BALANCE, &value);
        if (value == 1)
            fprintf(stderr, "Cannot set WHITE_BALANCE_TEMPERATURE while AUTO_WHITE_BALANCE is on. Set WHITE_BALANCE to 0 to adjust WHITE_BALANCE_TEMPERATURE\n");
            //I am not returning here because I want to keep errno. 

    }

    if (ctrl == EXPOSURE_ABSOLUTE){
        int value;
        get_ctrl(cam, EXPOSURE_AUTO, &value);
        if (value == 3)
            fprintf(stderr, "Cannot set EXPOSURE_ABSOLUTE while EXPOSURE_AUTO is set to 3. Set EXPOSURE_AUTO to 1 to adjust exposure.\n");
            //I am not returning here because I want to keep errno. 

    }
    ///////////////////// DONE CHECKING FOR EDGE CASES/////////////////////////////////////

    if (ret == -1) {
        if (control.id == 1)
            perror("IOCTL failed because of ctrl argument");
        else
            perror("IOCTL failed because of value argument");
        return errno;

        
    } else {

        if (value < cam->controls[ctrl].min_value){
            fprintf(stderr, "WARNING: Set value for %s was less than lower bound. %s was automatically set to lower bound: %d\n", 
            cam->controls[ctrl].name, cam->controls[ctrl].name, cam->controls[ctrl].min_value);
        }

        else if (value > cam->controls[ctrl].max_value){
            fprintf(stderr, "WARNING: Set value for %s exceeds upper bound. %s was automatically set to upper bound: %d\n"
            , cam->controls[ctrl].name, cam->controls[ctrl].name, cam->controls[ctrl].max_value);
        }

        return (0);

    }

}

int save_file(camera *cam, const char* fname){

    FILE *out = fopen(fname, "w+");
    if (out == NULL){
        perror("Unable to write file");
        return errno;
    }

    //error checking loop -- makes sure camera can get all values
    for (int i = 0; i< CAM_CTRL_COUNT; i++){
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0){
            return -1;
        }
    }
    
    //writing loop
    for (int i =0; i < CAM_CTRL_COUNT; i++){
        char val_char[10];
        int value;
        get_ctrl(cam, i, &value);
        sprintf(val_char, ":%d\n", value);
        fputs(cam->controls[i].name, out);
        fputs(val_char, out);
    }

    fclose(out);
    

    return 0;

}

int load_file(camera *cam, const char* fname){

    FILE *in = fopen(fname, "r");
    if (in == NULL){
        perror("Unable to find fname");
        return errno;
    }
    //before setting each control to that specified in the file,
    //we check to make sure the file is formatted correctly
    char line[50];
    int i = 0;
    while (fgets(line, sizeof(line), in) !=NULL){
            char *ctrl_name = strtok(line, ":");
            char *ctrl_val_str = strtok(NULL, "\n");
            if (ctrl_val_str == NULL){
                fprintf(stderr, "File format error: format should be \"CONTROL_NAME:CONTROL_VALUE\"");
                return -1;
            }
            i = i + 1;
            if (i > CAM_CTRL_COUNT){
                fprintf(stderr, "File format error: too many controls");
                return -1;
            }
        }
    if (i < CAM_CTRL_COUNT){
            fprintf(stderr, "File format error: too few controls");
            return -1;
        }

    //Done checking for formatting errors
    
    i = 0;
    while (fgets(line, sizeof(line), in) !=NULL){
        char *ctrl_name = strtok(line, ":");
        char *ctrl_val_str = strtok(NULL, "\n");
        int ctrl_val = atoi(ctrl_val_str);
        set_ctrl(cam, i, ctrl_val);
        i = i + 1;
    }

    return 0;

}

//cannot fail
//saves camera control values to struct
void save_struct(camera *cam, ctrls_struct *controls){
    for (int i = 0; i < CAM_CTRL_COUNT; i++){
        int value;
        get_ctrl(cam, i, &value);
        
        set_ctrl_struct(controls, i, value);
    }
}

//loads struct values into camera control registers
//can technically fail, but cannot trigger return because of white_balance_temperature and exposure_absolute
void load_struct(camera *cam, ctrls_struct *controls){
    for (int i = 0; i < CAM_CTRL_COUNT; i++){
        int value = get_ctrl_struct(controls, i);
        set_ctrl(cam, i, value);
    }

}

void save_default_struct(camera *cam, ctrls_struct *controls){
    for (int i = 0; i < CAM_CTRL_COUNT; i++){
        
        set_ctrl_struct(controls, i, cam->controls[i].default_val);
    }
}

int reset_ctrl(camera *cam, ctrl_tag ctrl){

    int ret = set_ctrl(cam, ctrl, cam->controls[ctrl].default_val);

    if (ret != 0){
        return -1;
    }

    return 0;
}

void reset(camera *cam){

    for (int i = 0; i < CAM_CTRL_COUNT; i++){

        reset_ctrl(cam, i);

    }

}

camera* boot_camera(char *cam_file){

    camera *cam = malloc(sizeof(struct camera));
    int fd = open(cam_file, O_RDWR | O_NONBLOCK, 0);
    cam->fd = fd;

    for (int i = 0; i < CAM_CTRL_COUNT; i++){
        struct v4l2_queryctrl query;
        int ret = get_queryctrl(cam, i, &query);

        if (ret != 0){
            //We failed our queryctrl;
            fprintf(stderr, "Error creating cam struct. Make sure the cam_file argument points to the correct file.");
            return NULL;
        }

        strcpy(cam->controls[i].name, query.name);
        cam->controls[i].v4l2_id = query.id;
        cam->controls[i].max_value = query.maximum;
        cam->controls[i].min_value = query.minimum;
        cam->controls[i].default_val = query.default_value;
    }

    return (cam);

}

int get_queryctrl(camera *cam, ctrl_tag ctrl, struct v4l2_queryctrl *query_out){
    struct v4l2_queryctrl query;
    CLEAR(query);
    query.id = get_ctrl_id(ctrl);
    
    int ret = ioctl(cam->fd, VIDIOC_QUERYCTRL, &query);

    if (ret == -1) {
        if (query.id == 1)
            perror("IOCTL failed because of ctrl argument");
        else
            perror("IOCTL failed because of value argument");
        return errno;
    } else {

        *query_out = query;
        return 0;

    }
}

void close_cam(camera *cam){

    if (cam == NULL){
        return;
    }

    close(cam->fd);
    free(cam);


}

void print_ctrls(camera *cam){

    for (int i = 0; i < CAM_CTRL_COUNT; i++){
        int value;
        get_ctrl(cam, i, &value);
        printf("%s %d\n", cam->controls[i].name, value);

    }

}