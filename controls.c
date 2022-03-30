#include "controls.h"
#include "capture.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct recording_format
{
    char *name;
    int height;
    int width;

} recording_format;

/**
 * @brief Fills in a recording_format struct from an int value associated with the format ENUM.
 *
 * @param fmt_tag: the ENUM for the desired format struct.
 * @return recording_format*: the recording format struct with char *name, int height, and
 * int width all matching that of @param fmt_tag.
 */
recording_format *get_fields_from_tag(format fmt_tag)
{

    assert(fmt_tag <= YUYV_320_240 && fmt_tag >= 0);

    recording_format *fmt = malloc(sizeof(fmt));
    fmt->name = "MJPEG";

    if (fmt_tag == MJPEG_1920_1080 || fmt_tag == YUYV_1920_1080)
    {
        fmt->width = 1920;
        fmt->height = 1080;
    }
    else if (fmt_tag == MJPEG_1280_1024 || fmt_tag == YUYV_1280_1024)
    {
        fmt->width = 1280;
        fmt->height = 1024;
    }
    else if (fmt_tag == MJPEG_1280_720 || fmt_tag == YUYV_1280_720)
    {
        fmt->width = 1280;
        fmt->height = 720;
    }
    else if (fmt_tag == MJPEG_800_600 || fmt_tag == YUYV_800_600)
    {
        fmt->width = 800;
        fmt->height = 600;
    }
    else if (fmt_tag == MJPEG_640_480 || fmt_tag == YUYV_640_480)
    {
        fmt->width = 640;
        fmt->height = 480;
    }
    else if (fmt_tag == MJPEG_320_240 || fmt_tag == YUYV_320_240)
    {
        fmt->width = 320;
        fmt->height = 240;
    }

    else
    {
        fprintf(stderr, "Invalid tag entered\n"); // cannot enter this code with assert done
        return NULL;
    }

    int diff = fmt_tag - 5;
    if (diff > 0)
    {
        fmt->name = "YUYV"; // We are in the second half of the enums, and so it is a YUYV format
    }

    return fmt;
}
/**
 * @brief Helps to interface between V4L2 query of selected pixel
 * format and the format ENUM values.
 *
 * @param fmt_type The V4L2_PIX_FMT enum for either YUYV or MJPEG.
 * @param height The height of the aspect ratio for above pixel format.
 * @return The format ENUM associated with the V4L2 pixel format/aspect ratio
 * (a 0-11 int).
 */
int get_fmt_tag(int fmt_type, int height)
{
    assert(fmt_type == V4L2_PIX_FMT_YUYV || fmt_type == V4L2_PIX_FMT_MJPEG);
    assert(height == 1080 || height == 1024 || height == 720 || height == 600 || height == 480 || height == 240);

    format ret;
    switch (height)
    {
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
    }

    if (fmt_type == V4L2_PIX_FMT_YUYV)
    {
        ret = ret + 6;
    }
    else
    {
        return ret;
    }
}
/**
 * @brief Getting the format ENUM for the camera's current state.
 *
 * @param cam pointer to a cam struct.
 * @param value the value into which the format ENUM will be passed on the function's exit.
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int get_fmt(camera *cam, int *value)
{
    assert(cam != NULL);
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == ioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        perror("Getting Pixel Format"); // IOCTL failed
        return errno;
    }

    int ret = get_fmt_tag(fmt.fmt.pix.pixelformat, fmt.fmt.pix.height);

    *value = ret;
    return 0;
}
/**
 * @brief Set the fmt object
 *
 * @param cam pointer to a cam struct
 * @param fmt_tag ENUM for camera format to which the camera will be set
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int set_fmt(camera *cam, format fmt_tag)
{
    assert(cam != NULL);

    recording_format *fmt_struct = get_fields_from_tag(fmt_tag); // cannot be null, assert is handled in the get_fields_from_tag function

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = fmt_struct->width;
    fmt.fmt.pix.height = fmt_struct->height;
    if (fmt_tag < 6)
    {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    }
    else
    {
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
/**
 * @brief Gets the V4L2 ID associated with a ctrl_tag ENUM.
 *
 * @param ctrl the ctrl_tag ENUM
 * @return the V4L2 ID for ctrl.
 */
int get_ctrl_id(ctrl_tag ctrl)
{

    assert(ctrl >= 0 && ctrl < CAM_CTRL_COUNT);

    int ctrl_id;
    switch (ctrl)
    {
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
    }

    return ctrl_id;
}
/**
 * @brief Get's the value of a control.
 *
 * @param cam a pointer to the cam struct
 * @param ctrl the ctrl_tag ENUM
 * @param value the int into which @param ctrl's value will be passed. If @param ctrl is FORMAT, this will be
 * the number associated with the camera's current format ENUM.
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int get_ctrl(camera *cam, ctrl_tag ctrl, int *value)
{
    assert(cam != NULL);

    if (ctrl == FORMAT)
    {
        return get_fmt(cam, value); // format behaves differently from other controls
    }

    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    int ret = ioctl(cam->fd, VIDIOC_G_CTRL, &control);

    if (ret == -1)
    {
        fprintf(stderr, "IOCTL failed for %s: %s\n", cam->controls[ctrl].name, strerror(errno));
        return errno;
    }
    else
    {

        *value = control.value;
        return 0;
    }
}
/**
 * @brief Sets the value of a control.
 *
 * @param cam pointer to the cam struct
 * @param ctrl the ctrl_tag ENUM
 * @param value the value to which we will set @param ctrl
 * @return exit status. 0 on success, errno on IOCTL failure.
 * Setting WHITE_BALANCE_TEMPERATURE or EXPOSURE_ABSOLUTE while
 * their respective auto-set functions are on will result in success. Setting
 * a control to a value above/below its upper/lower bounds will result in success and
 * set the control's register to its max/min.
 */
int set_ctrl(camera *cam, ctrl_tag ctrl, int value)
{
    assert(cam != NULL);

    if (ctrl == FORMAT)
    {
        return set_fmt(cam, value);
    }

    struct v4l2_control control;
    CLEAR(control);
    control.id = get_ctrl_id(ctrl);
    control.value = value;

    /////////////////// BEGIN CHECKING FOR EDGE CASES ///////////////////////////////////////
    if (ctrl == WHITE_BALANCE_TEMPERATURE)
    {
        int value;
        get_ctrl(cam, AUTO_WHITE_BALANCE, &value);
        if (value == 1)
            return 0; // skip
        // fprintf(stderr, "Cannot set WHITE_BALANCE_TEMPERATURE while AUTO_WHITE_BALANCE is on. Set WHITE_BALANCE to 0 to adjust WHITE_BALANCE_TEMPERATURE\n");
    }

    if (ctrl == EXPOSURE_ABSOLUTE)
    {
        int value;
        get_ctrl(cam, EXPOSURE_AUTO, &value);
        if (value == 3)
            return 0; // skip
        // fprintf(stderr, "Cannot set EXPOSURE_ABSOLUTE while EXPOSURE_AUTO is set to 3. Set EXPOSURE_AUTO to 1 to adjust exposure.\n");
    }
    ///////////////////// DONE CHECKING FOR EDGE CASES/////////////////////////////////////

    int ret = ioctl(cam->fd, VIDIOC_S_CTRL, &control); // set camera's control value to @param value
    if (ret == -1)
    {
        fprintf(stderr, "IOCTL failed for %s: %s\n", cam->controls[ctrl].name, strerror(errno));
        return errno;
    }
    else
    {

        if (value < cam->controls[ctrl].min_value)
        {
            fprintf(stderr, "WARNING: Set value for %s was less than lower bound. %s was automatically set to lower bound: %d\n",
                    cam->controls[ctrl].name, cam->controls[ctrl].name, cam->controls[ctrl].min_value);
        }

        else if (value > cam->controls[ctrl].max_value)
        {
            fprintf(stderr, "WARNING: Set value for %s exceeds upper bound. %s was automatically set to upper bound: %d\n", cam->controls[ctrl].name, cam->controls[ctrl].name, cam->controls[ctrl].max_value);
        }

        return 0;
    }
}
/**
 * @brief Writes camera's current values to a file.
 *
 * @param cam pointer to cam struct.
 * @param fname String of filename to which the output will be written.
 * @return exit status. 0 on success, errno on IOCTL/file write failure.
 */
int save_file(camera *cam, const char *fname)
{
    assert(cam != NULL && fname != NULL);

    FILE *out = fopen(fname, "w+");
    if (out == NULL)
    {
        perror("Unable to write file");
        return errno;
    }
    // Design choice to return if anything fails. Should I assert here?
    //  error checking loop -- makes sure camera can get all values
    for (int i = 0; i < CAM_CTRL_COUNT; i++)
    {
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0)
        {
            return ret; // get_ctrl had IOCTL failure
        }
    }

    // writing loop. Don't need to error check our get_ctrl calls as we've preprocessed these.
    for (int i = 0; i < CAM_CTRL_COUNT - 1; i++)
    { // do this for all except FORMAT
        char val_char[10];
        int value;
        get_ctrl(cam, i, &value);
        sprintf(val_char, ":%d\n", value);
        fputs(cam->controls[i].name, out);
        fputs(val_char, out);
    }

    // controls are written, now write FORMAT
    int value;
    get_fmt(cam, &value);
    format fmt_tag = value;
    recording_format *fmt_struct = get_fields_from_tag(fmt_tag);
    char val_char[32];
    sprintf(val_char, "Format:%s_%d_%d\n", fmt_struct->name, fmt_struct->width, fmt_struct->height);
    fputs(val_char, out);

    free(fmt_struct);
    fclose(out);

    return 0;
}
/**
 * @brief Loads values from file into camera.
 *
 * @param cam pointer to the cam struct
 * @param fname String of filename from which to load data.
 * @return exit status. 0 on success, errno on IOCTL failure for setting register or file open failure,
 * -2 for file formatting error, -3 for pixel format incorrect entry formatting, -4 for
 * too many control entries, -5 for too few control entries.
 * aspect ratio.
 */
int load_file(camera *cam, const char *fname)
{
    assert(cam != NULL && fname != NULL);

    FILE *in = fopen(fname, "r");

    // before setting each control to that specified in the file,
    // we check to make sure the file is formatted correctly
    char line[50];
    int i = 0;
    while (fgets(line, sizeof(line), in) != NULL)
    {
        char *ctrl_name = strtok(line, ":");
        if (strcmp(ctrl_name, cam->controls[i].name))
        {
            fprintf(stderr, "Control order error. Expected %s, got %s\n", cam->controls[i].name, ctrl_name);
            return -1;
        }
        char *ctrl_val_str = strtok(NULL, "\n");
        if (ctrl_val_str == NULL)
        {
            fprintf(stderr, "File format error: format should be \"CONTROL_NAME:CONTROL_VALUE\"\n");
            return -2;
        }

        if (i == FORMAT)
        {
            char *format = strtok(ctrl_val_str, "_");
            char *width_str = strtok(NULL, "_");
            char *height_str = strtok(NULL, "_");
            if (format == NULL || width_str == NULL || height_str == NULL)
            {
                fprintf(stderr, "File format error: pixel format should be written as \"Format:PIXELFORMAT_WIDTH_HEIGHT\"\n");
                return -3;
            }
        }

        i = i + 1;
    }
    if (i > CAM_CTRL_COUNT)
    {
        fprintf(stderr, "File format error: too many controls\n");
        return -4;
    }
    if (i < CAM_CTRL_COUNT)
    {
        fprintf(stderr, "File format error: too few controls\n");
        return -5;
    }

    // Done checking for formatting errors
    fclose(in);
    in = fopen(fname, "r");
    i = 0;
    while (fgets(line, sizeof(line), in) != NULL)
    {
        int ctrl_val;
        char *ctrl_name = strtok(line, ":");
        char *ctrl_val_str = strtok(NULL, "\n");
        if (i != FORMAT)
        {
            ctrl_val = atoi(ctrl_val_str);
        }
        else
        {
            int tag;
            char *format = strtok(ctrl_val_str, "_");
            char *width_str = strtok(NULL, "_");
            char *height_str = strtok(NULL, "_"); // V4L2_PIX_FMT_YUYV

            int height = atoi(height_str);
            if (strcmp("YUYV", format))
            {
                ctrl_val = get_fmt_tag(V4L2_PIX_FMT_MJPEG, height);
            }
            else
            {
                ctrl_val = get_fmt_tag(V4L2_PIX_FMT_YUYV, height);
            }
        }
        int ret = set_ctrl(cam, i, ctrl_val);
        if (ret != 0)
        {
            return ret;
        }

        i = i + 1;
    }

    return 0;
}

/**
 * @brief Saves a struct of the camera's current control values
 *
 * @param cam the pointer to the cam struct
 * @param controls The struct to which the camera's current control values with be saved.
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int save_struct(camera *cam, ctrls_struct *controls)
{
    assert(cam != NULL && controls != NULL);
    for (int i = 0; i < CAM_CTRL_COUNT; i++)
    {
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0)
        {
            return ret;
        }

        controls->value[i] = value;
    }
    return 0;
}

/**
 * @brief Saves default values of camera controls to a ctrls_struct.
 * 
 * @param cam pointer to the cam struct.
 * @param controls the ctrls_struct to which the default values will be saved
 */
void save_default_struct(camera *cam, ctrls_struct *controls){
    assert(cam != NULL && controls != NULL);

    for (int i = 0; i < CAM_CTRL_COUNT; i++)
    {
        controls->value[i] = cam->controls[i].default_val;

    }


}

/**
 * @brief Loads values from a ctrls_struct into the camera.
 *
 * @param cam a pointer to a camera struct
 * @param controls
 * @return exit status. 0 on success, errno on failure.
 */
int load_struct(camera *cam, ctrls_struct *controls)
{
    assert(cam != NULL && controls != NULL);
    for (int i = 0; i < CAM_CTRL_COUNT; i++)
    {
        int value = controls->value[i];
        int ret = set_ctrl(cam, i, value);
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}

/**
 * @brief Resets a control to its default value
 *
 * @param cam a pointer to a cam struct
 * @param ctrl the control value which will be reset
 * @return exit status. 0 on success, -1 on invalid @param ctrl argument,
 * errno on IOCTL failure.
 */
int reset_ctrl(camera *cam, ctrl_tag ctrl)
{

    int ret = set_ctrl(cam, ctrl, cam->controls[ctrl].default_val);

    return ret;
}

/**
 * @brief Resets the camera to its default values
 *
 * @param cam a pointer to a cam struct
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int reset(camera *cam)
{

    for (int i = 0; i < CAM_CTRL_COUNT; i++)
    {

        int ret = reset_ctrl(cam, i);
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}
/**
 * @brief Get the queryctrl object
 *
 * @param cam a pointer to the camera struct
 * @param ctrl The control to be queried.
 * @param query_out The struct to which the queryctrl will be written.
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int get_queryctrl(camera *cam, ctrl_tag ctrl, struct v4l2_queryctrl *query_out)
{
    struct v4l2_queryctrl query;
    CLEAR(query);
    query.id = get_ctrl_id(ctrl); // this cannot fail, since this function is not user-facing

    int ret = ioctl(cam->fd, VIDIOC_QUERYCTRL, &query);

    if (ret == -1)
    {
        fprintf(stderr, "IOCTL failed for %s during camera_boot: %s\n", cam->controls[ctrl].name, strerror(errno));
        return errno;
    }
    else
    {

        *query_out = query;
        return 0;
    }
}

/**
 * @brief Boots the camera. Initializes the following in the camera struct:
 * cam->fd: the camera's file descriptor.
 * cam->controls: The list of controls belonging to the minicam. The control struct includes name and default/min/max values.
 * cam->buffer: The memory map which is used to store bits before they are written to an image file.
 *
 * @param cam a pointer to a camera object. Must be malloc'd prior to funciton call.
 * @param cam_file the string for the file name of the camera. Usually one of the video files in the /dev mount.
 * @return exit status. 0 on success, -1 if pointer to cam is NULL, errno on failure.
 */
int boot_camera(camera *cam, char *cam_file)
{
    assert (cam_file != NULL);

    int fd = open(cam_file, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1)
    {
        free(cam);
        perror("Opening camera file");
        return errno;
    }
    cam->fd = fd;

    for (int i = 0; i < CAM_CTRL_COUNT - 1; i++)
    {
        struct v4l2_queryctrl query;
        int ret = get_queryctrl(cam, i, &query);

        if (ret != 0)
        {
            return ret;
        }

        strcpy(cam->controls[i].name, query.name);
        cam->controls[i].v4l2_id = query.id;
        cam->controls[i].max_value = query.maximum;
        cam->controls[i].min_value = query.minimum;
        cam->controls[i].default_val = query.default_value;
    }
    strcpy(cam->controls[FORMAT].name, "Format");
    cam->controls[FORMAT].default_val = MJPEG_1920_1080;

    int ret = init_mmap(cam);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}


/**
 * @brief Deallocates the memory used for the camera and closes the camera's file descriptor.
 *
 * @param cam the pointer to the camera structure.
 * @return exit status. 0 on success, errno on failure
 */
int close_cam(camera *cam)
{

    assert (cam!= NULL);

    int ret = close(cam->fd);
    if (ret == -1)
    {
        free(cam);
        perror("Closing camera's file descriptor");
        return errno;
    }
    free(cam);
    return 0;
}

/**
 * @brief Prints the cameras controls and their current values
 *
 * @param cam
 * @return 0 on success, errno on failure.
 */
int print_ctrls(camera *cam)
{
    assert (cam != NULL);
    for (int i = 0; i < CAM_CTRL_COUNT - 1; i++)
    {
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0)
        {
            return ret;
        }
        printf("%s:%d\n", cam->controls[i].name, value);
    }

    int value;
    int ret = get_ctrl(cam, FORMAT, &value);
    if (ret != 0)
    {
        return ret;
    }
    recording_format *f = malloc(sizeof(recording_format));
    f = get_fields_from_tag(value);
    printf("Format:%s_%d_%d\n", f->name, f->width, f->height);
    free(f);
    return 0;
}
/**
 * @brief prints all of the default values for the camera
 * 
 * @param cam pointer to the cam struct
 */
void print_defaults(camera *cam){
    assert (cam!= NULL);
    printf("DEFAULTS:\n");
    for (int i = 0; i < CAM_CTRL_COUNT - 1;i++){
        printf("%s:%d\n", cam->controls[i].name, cam->controls[i].default_val);
    }
}

/**
 * @brief prints all of the default values for the camera
 * 
 * @param cam pointer to the cam struct
 */
void print_bounds(camera *cam){
    assert (cam!= NULL);
    printf("BOUNDS:\n");
    for (int i = 0; i < CAM_CTRL_COUNT - 1;i++){
        printf("%s:[%d,%d]\n", cam->controls[i].name, cam->controls[i].min_value, cam->controls[i].max_value);
    }
}

static int xioctl(int fd, int request, void *arg)
{
    int r;

    do
        r = ioctl(fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}

/**
 * @brief Prints capabilites of the camera, along with selected
 * recording modes.
 *
 * @param cam the pointer to the camera struct.
 * @return exit status. 0 on success, errno on failure.
 */
int print_caps(camera *cam)
{
    assert (cam != NULL);
    struct v4l2_capability caps = {};
    if (-1 == xioctl(cam->fd, VIDIOC_QUERYCAP, &caps))
    {
        perror("Querying Capabilities");
        return errno;
    }

    printf("Driver Caps:\n"
           "  Driver: \"%s\"\n"
           "  Card: \"%s\"\n"
           "  Bus: \"%s\"\n"
           "  Version: %d.%d\n"
           "  Capabilities: %08x\n",
           caps.driver,
           caps.card,
           caps.bus_info,
           (caps.version >> 16) && 0xff,
           (caps.version >> 24) && 0xff,
           caps.capabilities);

    struct v4l2_cropcap cropcap = {0};
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(cam->fd, VIDIOC_CROPCAP, &cropcap))
    {
        perror("Querying Cropping Capabilities");
        return errno;
    }

    printf("Camera Cropping:\n"
           "  Bounds: %dx%d+%d+%d\n"
           "  Default: %dx%d+%d+%d\n"
           "  Aspect: %d/%d\n",
           cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
           cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
           cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);

    int support_jpg = 0;

    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char fourcc[5] = {0};
    char c, e;
    printf("  FMT : CE Desc\n--------------------\n");
    while (0 == xioctl(cam->fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
        if (fmtdesc.pixelformat == V4L2_PIX_FMT_MJPEG)
            support_jpg = 1;
        c = fmtdesc.flags & 1 ? 'C' : ' ';
        e = fmtdesc.flags & 2 ? 'E' : ' ';
        printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
        fmtdesc.index++;
    }

    if (!support_jpg)
    {
        printf("Doesn't support MJPEG.\n");
        return -1;
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        perror("Getting Pixel Format");
        return errno;
    }

    strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
    printf("Selected Camera Mode:\n"
           "  Width: %d\n"
           "  Height: %d\n"
           "  PixFmt: %s\n"
           "  Field: %d\n",
           fmt.fmt.pix.width,
           fmt.fmt.pix.height,
           fourcc,
           fmt.fmt.pix.field);
    return 0;
}

/**
 * @brief Initializes the memory map that stores bytes captured by the camera.
 *
 * @param cam the pointer to the camera object.
 * @return exit status. 0 on success, errno on failure.
 */
int init_mmap(camera *cam)
{
    assert (cam != NULL);

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(cam->fd, VIDIOC_REQBUFS, &req)) // this is the line, for some reason renders camera busy indefinitely
    {
        perror("Requesting Buffer");
        return errno;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (-1 == xioctl(cam->fd, VIDIOC_QUERYBUF, &buf))
    {
        perror("Querying Buffer");
        return errno;
    }

    cam->buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam->fd, buf.m.offset);
    // printf("Length: %d\nAddress: %p\n", buf.length, buffer);
    // printf("Image Length: %d\n", buf.bytesused);

    return 0;
}

/**
 * @brief Captures a single image and writes it to @param file_name
 *
 * @param cam the pointer to the camera file
 * @param file_name The file destination for the image.
 * @return exit status. 0 on success, errno on failure.
 */
int capture_image(camera *cam, char *file_name)
{
    assert (cam != NULL && file_name != NULL);

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))
    {
        perror("Query Buffer");
        return errno;
    }

    if (-1 == xioctl(cam->fd, VIDIOC_STREAMON, &buf.type))
    {
        perror("Start Capture");
        return errno;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(cam->fd, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(cam->fd + 1, &fds, NULL, NULL, &tv);
    if (-1 == r)
    {
        perror("Waiting for Frame");
        return errno;
    }

    if (-1 == xioctl(cam->fd, VIDIOC_DQBUF, &buf))
    {
        perror("Retrieving Frame");
        return errno;
    }

    int outfd = open(file_name, O_RDWR | O_CREAT, 0644);
    if (outfd == -1)
    {
        printf("Problem opening file %s: %s\n", file_name, strerror(errno));
        return errno;
    }
    int ret = write(outfd, cam->buffer, buf.bytesused);
    if (ret == -1)
    {
        printf("Problem writing to file %s: %s\n", file_name, strerror(errno));
        return errno;
    }

    close(outfd);

    return 0;
}
