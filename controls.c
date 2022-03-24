#include "controls.h"
#include "capture.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct recording_format{
    char *name;
    int height;
    int width;

}recording_format;


recording_format* get_fields_from_tag(format fmt_tag){

    recording_format *fmt = malloc(sizeof(fmt));
    fmt->name = "MJPEG";


    if (fmt_tag == MJPEG_1920_1080 || fmt_tag == YUYV_1920_1080) {
        fmt->width = 1920;
        fmt->height = 1080;
    } else if (fmt_tag == MJPEG_1280_1024 || fmt_tag == YUYV_1280_1024){
        fmt->width = 1280;
        fmt->height = 1024;
    } else if (fmt_tag == MJPEG_1280_720 || fmt_tag == YUYV_1280_720){
        fmt->width = 1280;
        fmt->height = 720;
    } else if (fmt_tag == MJPEG_800_600 || fmt_tag == YUYV_800_600){
        fmt->width = 800;
        fmt->height = 600;
    }else if (fmt_tag == MJPEG_640_480 || fmt_tag == YUYV_640_480){
        fmt->width = 640;
        fmt->height = 480;

    } else if (fmt_tag == MJPEG_320_240 || fmt_tag == YUYV_320_240){
        fmt->width = 320;
        fmt->height = 240;
    } else {
        fprintf(stderr, "Invalid tag entered");
        return NULL;
    }

    int sub = fmt_tag - 5;

    if (sub > 0){
        fmt->name = "YUYV";
    }

    return fmt;

}

format get_fmt_tag(int fmt_type, int height){
    format ret = -1;
    switch(height){
        case 1080:
            ret = MJPEG_1920_1080;
            break;
        case 1024:
            ret = MJPEG_1280_1024;
            break;
        case 720:
            ret = MJPEG_1280_720;
            break;
        case 600:
            ret = MJPEG_800_600;
            break;
        case 480:
            ret = MJPEG_640_480;
            break;
        case 240:
            ret = MJPEG_320_240;
            break;
        default:
            //Should be impossible for this to fail
            fprintf(stderr, "Valid pixel format for MINICAM not received.");
            return -2;
    }

    if (fmt_type == V4L2_PIX_FMT_YUYV){
        ret = ret + 6;
    }

    return ret;

}

int get_fmt(camera *cam){
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == ioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        perror("Getting Pixel Format");
        return -1;
    }

    return get_fmt_tag(fmt.fmt.pix.pixelformat, fmt.fmt.pix.height); //-1 on ioctl error, -2 on getting tag

}

int set_fmt(camera *cam, format fmt_tag){
    recording_format *fmt_struct = get_fields_from_tag(fmt_tag);
    if (fmt_struct == NULL){
        return -1; //Invalid tag entered!
    }
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = fmt_struct->width;
    fmt.fmt.pix.height = fmt_struct->height;
    if (fmt_tag < 6){
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    }else{
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    }
    
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == ioctl(cam->fd, VIDIOC_S_FMT, &fmt))
    {
        perror("Setting Pixel Format");
        free(fmt_struct);
        return errno;
    }
    free(fmt_struct);
    return 0;

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
            fprintf(stderr, "IOCTL failed for %s because of ctrl argument: %s", cam->controls[ctrl].name, strerror(errno));
        else
            fprintf(stderr, "IOCTL failed for %s because of value argument: %s", cam->controls[ctrl].name, strerror(errno));
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
            return 0; //skip
            //fprintf(stderr, "Cannot set WHITE_BALANCE_TEMPERATURE while AUTO_WHITE_BALANCE is on. Set WHITE_BALANCE to 0 to adjust WHITE_BALANCE_TEMPERATURE\n");
            //I am not returning here because I want to keep errno. 

    }

    if (ctrl == EXPOSURE_ABSOLUTE){
        int value;
        get_ctrl(cam, EXPOSURE_AUTO, &value);
        if (value == 3)
            return 0; //skip
            //fprintf(stderr, "Cannot set EXPOSURE_ABSOLUTE while EXPOSURE_AUTO is set to 3. Set EXPOSURE_AUTO to 1 to adjust exposure.\n");
            //I am not returning here because I want to keep errno. 

    }
    ///////////////////// DONE CHECKING FOR EDGE CASES/////////////////////////////////////

    if (ret == -1) {
        if (control.id == 1)
            fprintf(stderr, "IOCTL failed for %s because of ctrl argument: %s", cam->controls[ctrl].name, strerror(errno));
        else
            fprintf(stderr, "IOCTL failed for %s because of value argument: %s", cam->controls[ctrl].name, strerror(errno));
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
    for (int i = 0; i< CAM_CTRL_COUNT - 1; i++){ // need to do -1 now that we have 
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0){
            return -1; //we could not successfully get a value from a camera, should only happen when unplugged
        }
    }
    
    //writing loop
    for (int i =0; i < CAM_CTRL_COUNT - 1; i++){
        char val_char[10];
        int value;
        get_ctrl(cam, i, &value);
        sprintf(val_char, ":%d\n", value);
        fputs(cam->controls[i].name, out);
        fputs(val_char, out);
    }

    //controls are written, now write format
    format fmt_tag = get_fmt(cam);
    recording_format* fmt_struct = get_fields_from_tag(fmt_tag);
    char val_char[20];
    int value;
    sprintf(val_char, ":%s_%d_%d\n", fmt_struct->name, fmt_struct->width, fmt_struct->height);
    fputs("Format", out);
    fputs(val_char, out);


    free(fmt_struct);
    fclose(out);
    

    return 0;

}
//error checking here is a nightmare
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
                return -3;
            }
            i = i + 1;
            if (i > CAM_CTRL_COUNT){
                fprintf(stderr, "File format error: too many controls");
                return -4;
            }
        }
    if (i < CAM_CTRL_COUNT){
            fprintf(stderr, "File format error: too few controls");
            return -5;
        }

    //Done checking for formatting errors
    fclose(in);
    in = fopen(fname, "r");
    i = 0;
    while (fgets(line, sizeof(line), in) !=NULL){
        char *ctrl_name = strtok(line, ":");
        char *ctrl_val_str = strtok(NULL, "\n");
        if (i != FORMAT){
            int ctrl_val = atoi(ctrl_val_str);
            set_ctrl(cam, i, ctrl_val); //error handle this
        }else{
            int tag;
            char *format = strtok(ctrl_val_str, "_");
            char *width_str = strtok(NULL, "_");
            char *height_str = strtok(NULL, "_");//V4L2_PIX_FMT_YUYV

            int height = atoi(height_str);
            if (strcmp("YUYV", format)){
                tag = get_fmt_tag(V4L2_PIX_FMT_MJPEG, height); //error handle this
            }else{
                tag = get_fmt_tag(V4L2_PIX_FMT_YUYV, height); //error handle this
            }
            
            set_fmt(cam, tag);

            
        }
        
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
//change to int
camera* boot_camera(char *cam_file){

    camera *cam = malloc(sizeof(struct camera));
    int fd = open(cam_file, O_RDWR | O_NONBLOCK, 0);
    cam->fd = fd;

    for (int i = 0; i < CAM_CTRL_COUNT - 1; i++){
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
            fprintf(stderr, "IOCTL failed for %s because of ctrl argument: %s", cam->controls[ctrl].name, strerror(errno));
        else
            fprintf(stderr, "IOCTL failed for %s because of value argument: %s", cam->controls[ctrl].name, strerror(errno));
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

    for (int i = 0; i < CAM_CTRL_COUNT - 1; i++){
        int value;
        get_ctrl(cam, i, &value);
        printf("%s %d\n", cam->controls[i].name, value);

    }
    
}

int capture(camera *cam, char *file_name){

    if(init_mmap(cam->fd) != 0)
        return errno;

    if(capture_image(cam->fd, file_name) != 0)
        return errno;

    return 0;

}