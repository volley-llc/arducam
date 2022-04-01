#include "controls.h"

#ifndef NDEBUG
#define DEBUG_PRINT fprintf
#define DEBUG_PERROR perror
#else
#define DEBUG_PRINT
#define DEBUG_PERROR
#endif
//int init_mmap(camera_t *cam);
/**
 * @brief Struct which maintains fields associated with different pixel formats
 *
 */
typedef const struct
{
    int v4l2_pix_fmt;
    const char *name;
    int width;
    int height;

} fmt_fields_t;

/**
 * @brief Table of different pixel formats with accessible fields.
 *
 */
const fmt_fields_t fmts[__ACAM_FMT_COUNT] = {
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 1920, 1080},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 1280, 1024},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 1280, 720},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 800, 600},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 640, 480},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", 320, 240},

    {V4L2_PIX_FMT_YUYV, "YUYV", 1920, 1080},
    {V4L2_PIX_FMT_YUYV, "YUYV", 1280, 1024},
    {V4L2_PIX_FMT_YUYV, "YUYV", 1280, 720},
    {V4L2_PIX_FMT_YUYV, "YUYV", 800, 600},
    {V4L2_PIX_FMT_YUYV, "YUYV", 640, 480},
    {V4L2_PIX_FMT_YUYV, "YUYV", 320, 240},

};
/**
 * @brief Table used to access V4L2 ids of different camera controls.
 *
 */
const ctrl_tag_t v4l2_id[__ACAM_CTRL_COUNT] = {
    V4L2_CID_BRIGHTNESS,
    V4L2_CID_CONTRAST,
    V4L2_CID_SATURATION,
    V4L2_CID_HUE,
    V4L2_CID_AUTO_WHITE_BALANCE,
    V4L2_CID_GAMMA,
    V4L2_CID_GAIN,
    V4L2_CID_POWER_LINE_FREQUENCY,
    V4L2_CID_WHITE_BALANCE_TEMPERATURE,
    V4L2_CID_SHARPNESS,
    V4L2_CID_BACKLIGHT_COMPENSATION,
    V4L2_CID_EXPOSURE_AUTO,
    V4L2_CID_EXPOSURE_ABSOLUTE,
    V4L2_CID_EXPOSURE_AUTO_PRIORITY};


//function prototypes for private functions:
fmt_t get_fmt_tag(int fmt_type, int height);
int get_fmt(const camera_t *cam, int *value);
int init_mmap(camera_t *cam);
int set_fmt(const camera_t *cam, fmt_t fmt_tag);
int check_file_for_errors(const camera_t *cam, const char *fname);
int get_queryctrl(camera_t *cam, ctrl_tag_t ctrl, struct v4l2_queryctrl *query_out);
static int xioctl(int fd, int request, void *arg);

/**
 * @brief Helps to interface between V4L2 query of selected pixel
 * format and the format ENUM values.
 *
 * @param fmt_type The V4L2_PIX_FMT enum for either YUYV or MJPEG.
 * @param height The height of the aspect ratio for above pixel format.
 * @return The format ENUM associated with the V4L2 pixel format/aspect ratio
 * (a 0-11 int). -1 if the fmt_type and pixel height do not match the ARDUCAM's specs.
 */
fmt_t get_fmt_tag(int fmt_type, int height)
{
    for (int i = 0; i < __ACAM_FMT_COUNT; i++)
    {
        if (height == fmts[i].height && fmt_type == fmts[i].v4l2_pix_fmt)
        {
            return i;
        }
    }

    return -1; // Our camera does not have a pixel format that matches the ARDUCAM specs. Should this be __ACAM_FMT_COUNT?
}
/**
 * @brief Get the format ENUM for the camera's current state.
 *
 * @param cam pointer to a cam struct.
 * @param value the value into which the format ENUM will be passed on the function's exit.
 * @return exit status. 0 on success, errno on IOCTL failure, EBADFD if the pixel format
 * is not supported by ARDUCAM.
 */
int get_fmt(const camera_t *cam, int *value)
{
    assert(cam && value);

    // set up v4l2 struct into which data will be read
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == ioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        DEBUG_PERROR("Getting Pixel Format"); // IOCTL failed
        return errno;
    }

    int ret = get_fmt_tag(fmt.fmt.pix.pixelformat, fmt.fmt.pix.height);
    if (ret == -1)
    {
        DEBUG_PRINT(stderr, "Invalid pixel format detected for ARDUCAM.\n");
        return EBADFD; // The pixel format received by the IOCTL was not supported by Arducam
    }

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
int set_fmt(const camera_t *cam, fmt_t fmt_tag)
{
    assert(cam);

    // set up struct that will be fed into camera's format register
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    fmt.fmt.pix.width = fmts[fmt_tag].width;
    fmt.fmt.pix.height = fmts[fmt_tag].height;
    fmt.fmt.pix.pixelformat = fmts[fmt_tag].v4l2_pix_fmt;

    if (-1 == ioctl(cam->fd, VIDIOC_S_FMT, &fmt))
    {
        DEBUG_PERROR("Setting Pixel Format");
        return errno;
    }
    return 0;
}

/**
 * @brief Get's the value of a control.
 *
 * @param cam a pointer to the cam struct
 * @param ctrl the ctrl_tag ENUM
 * @param value the int into which @param ctrl's value will be passed. If @param ctrl is FORMAT, this will be
 * the number associated with the camera's current format ENUM.
 * @return exit status. 0 on success, errno on IOCTL failure, EBADF if the function retrieved a pixel format
 * not supported by ARDUCAM.
 */
int get_ctrl(const camera_t *cam, ctrl_tag_t ctrl, int *value)
{
    assert(cam && value);

    if (ctrl == ACAM_FORMAT)
    {
        return get_fmt(cam, value); // format behaves differently from other controls, so we return this
    }

    // Set up the struct which will receive info from the ioctl
    struct v4l2_control control = {0};
    control.id = v4l2_id[ctrl];
    int ret = ioctl(cam->fd, VIDIOC_G_CTRL, &control);

    if (ret == -1)
    {
        DEBUG_PRINT(stderr, "IOCTL failed for %s: %s\n", cam->ctrls[ctrl].name, strerror(errno));
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
int set_ctrl(const camera_t *cam, ctrl_tag_t ctrl, int value)
{
    assert(cam);

    if (ctrl == ACAM_FORMAT)
    {
        return set_fmt(cam, value); // setting pixel format behaves differently from other controls
    }

    // set up struct which will be read into the camera register by ioctl
    struct v4l2_control control = {0};
    control.id = v4l2_id[ctrl];
    control.value = value;

    /////////////////// BEGIN CHECKING FOR EDGE CASES ///////////////////////////////////////
    if (ctrl == ACAM_WHITE_BALANCE_TEMPERATURE)
    {
        int value;
        get_ctrl(cam, ACAM_AUTO_WHITE_BALANCE, &value);
        if (value == 1)
            return 0; // Still report this as a success
        // DEBUG_PRINT(stderr, "Cannot set WHITE_BALANCE_TEMPERATURE while AUTO_WHITE_BALANCE is on. Set WHITE_BALANCE to 0 to adjust WHITE_BALANCE_TEMPERATURE\n");
    }

    if (ctrl == ACAM_EXPOSURE_ABSOLUTE)
    {
        int value;
        get_ctrl(cam, ACAM_EXPOSURE_AUTO, &value);
        if (value == 3)
            return 0; // Still report this as a success
        // DEBUG_PRINT(stderr, "Cannot set EXPOSURE_ABSOLUTE while EXPOSURE_AUTO is set to 3. Set EXPOSURE_AUTO to 1 to adjust exposure.\n");
    }
    ///////////////////// DONE CHECKING FOR EDGE CASES/////////////////////////////////////

    int ret = ioctl(cam->fd, VIDIOC_S_CTRL, &control); // set camera's control value to @param value
    if (ret == -1)
    {
        DEBUG_PRINT(stderr, "IOCTL failed for %s: %s\n", cam->ctrls[ctrl].name, strerror(errno));
        return errno;
    }
    else
    {

        if (value < cam->ctrls[ctrl].min_value)
        {
            DEBUG_PRINT(stderr, "WARNING: Set value for %s was less than lower bound. %s was automatically set to lower bound: %d\n",
                    cam->ctrls[ctrl].name, cam->ctrls[ctrl].name, cam->ctrls[ctrl].min_value);
        }

        else if (value > cam->ctrls[ctrl].max_value)
        {
            DEBUG_PRINT(stderr, "WARNING: Set value for %s exceeds upper bound. %s was automatically set to upper bound: %d\n", 
            cam->ctrls[ctrl].name, cam->ctrls[ctrl].name, cam->ctrls[ctrl].max_value);
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
int save_file(const camera_t *cam, const char *fname)
{
    assert(cam && fname);

    // make sure we can open file
    FILE *out = fopen(fname, "w+");
    if (out == NULL)
    {
        DEBUG_PERROR("Unable to write file");
        return errno;
    }
    // Design choice to abort if anything fails.
    //  error checking loop -- makes sure camera can get all values
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0)
        {
            return ret; // get_ctrl failed
        }
    }

    // writing loop. Don't need to error check our get_ctrl calls as we've preprocessed these.
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    { // do this for all except FORMAT
        char val_char[64];
        int value;
        get_ctrl(cam, i, &value);

        // writing process for normal controls
        if (i != ACAM_FORMAT)
        {
            sprintf(val_char, "%s:%d\n", cam->ctrls[i].name, value);
        }
        // writing process for format
        else
        {
            fmt_t fmt_tag = value; // for clarity -- value IS the fmt_tag
            sprintf(val_char, "%s:%s_%d_%d\n", cam->ctrls[ACAM_FORMAT].name, fmts[fmt_tag].name, fmts[fmt_tag].width, fmts[fmt_tag].height);
        }

        fputs(val_char, out);
    }

    fclose(out);

    return 0;
}
/**
 * @brief Checks file for formatting errors.
 *
 * @param cam pointer to the cam struct
 * @param fname String of filename from which to load data.
 * @return exit status. 0 on success, errno for file open failure, EBADF for file formatting error.
 * Check error log for file formatting error details.
 */
int check_file_for_errors(const camera_t *cam, const char *fname)
{
    assert(cam && fname);

    // make sure we can open file
    FILE *in = fopen(fname, "r");
    if (in == NULL)
    {
        DEBUG_PERROR("Unable to read file");
        return errno;
    }

    char line[64];
    int i = 0;
    while (fgets(line, sizeof(line), in) != NULL)
    {
        char *ctrl_name = strtok(line, ":");
        if (strcmp(ctrl_name, cam->ctrls[i].name))
        {
            DEBUG_PRINT(stderr, "Control ordering/spelling error. Expected %s, got %s\n", cam->ctrls[i].name, ctrl_name);
            return EBADF;
        }
        char *ctrl_val_str = strtok(NULL, "\n");
        if (ctrl_val_str == NULL)
        {
            DEBUG_PRINT(stderr, "File format error: format should be \"CONTROL_NAME:CONTROL_VALUE\"\n");
            return EBADF;
        }

        if (i == ACAM_FORMAT)
        {
            char *format = strtok(ctrl_val_str, "_");
            char *width_str = strtok(NULL, "_");
            char *height_str = strtok(NULL, "_");
            if (format == NULL || width_str == NULL || height_str == NULL)
            {
                DEBUG_PRINT(stderr, "File format error: pixel format should be written as \"Format:PIXELFORMAT_WIDTH_HEIGHT\"\n");
                return EBADF;
            }

            int height = atoi(height_str);
            fmt_t fmt_tag; // for clarity.
            if (!strcmp("YUYV", format))
            {
                fmt_tag = get_fmt_tag(V4L2_PIX_FMT_YUYV, height);
            }
            else if (!strcmp("MJPEG", format))
            {
                fmt_tag = get_fmt_tag(V4L2_PIX_FMT_MJPEG, height);
            }
            else
            {
                DEBUG_PRINT(stderr, "Invalid pixel format detected for ARDUCAM. Format %s in file neither YUYV nor MJPEG\n", format);
                return EBADFD;
            }
            if (fmt_tag == -1)
            { // The pixel ratio detected for @param cam is not supported by the ARDUCAM
                DEBUG_PRINT(stderr, "Invalid pixel format detected for ARDUCAM. Aspect ratio %s_%s in file not supported by ARDUCAM\n", width_str, height_str);
                return EBADFD;
            }
        }

        i = i + 1;
    }
    if (i > __ACAM_CTRL_COUNT)
    {
        DEBUG_PRINT(stderr, "File format error: too many controls\n");
        return EBADF;
    }
    if (i < __ACAM_CTRL_COUNT)
    {
        DEBUG_PRINT(stderr, "File format error: too few controls\n");
        return EBADF;
    }

    fclose(in);
    return 0;
}

/**
 * @brief Loads values from file into camera.
 *
 * @param cam pointer to the cam struct
 * @param fname String of filename from which to load data.
 * @return exit status. 0 on success, errno on IOCTL failure for setting register,
 * errno for file open failure, EBADF for file formatting error.
 * Check error log for file formatting error details.
 */
int load_file(const camera_t *cam, const char *fname)
{
    assert(cam && fname);

    // check file for errors
    int ret = check_file_for_errors(cam, fname);
    if (ret != 0)
    {
        return ret;
    }
    // Done checking for formatting errors
    FILE *in = fopen(fname, "r");
    if (in == NULL)
    {
        DEBUG_PERROR("Unable to read file");
        return errno;
    }
    int i = 0;
    char line[50];
    while (fgets(line, sizeof(line), in) != NULL)
    {
        int ctrl_val;
        char *ctrl_name = strtok(line, ":");
        char *ctrl_val_str = strtok(NULL, "\n");
        if (i != ACAM_FORMAT)
        { // the value is simply the number after the colon
            ctrl_val = atoi(ctrl_val_str);
        }
        else
        { // We need to parse from format string to fmt_tag
            char *format = strtok(ctrl_val_str, "_");
            char *width_str = strtok(NULL, "_");
            char *height_str = strtok(NULL, "_");

            int height = atoi(height_str);

            if (!strcmp("YUYV", format))
            {
                ctrl_val = get_fmt_tag(V4L2_PIX_FMT_YUYV, height); // the fmt_tag *is* the ctrl_val
            }
            else if (!strcmp("MJPEG", format))
            {
                ctrl_val = get_fmt_tag(V4L2_PIX_FMT_MJPEG, height);
            }
            assert(ctrl_val >= 0 && ctrl_val < __ACAM_FMT_COUNT); // We've preprocessed the file to check for this -- this should be impossible to trigger.
        }

        int ret = set_ctrl(cam, i, ctrl_val);
        if (ret != 0) // we failed our IOCTL for set_ctrl
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
int save_struct(const camera_t *cam, ctrls_struct *ctrls)
{
    assert(cam && ctrls);
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        int value;
        int ret = get_ctrl(cam, i, &value);
        if (ret != 0)
        {
            return ret;
        }

        ctrls->value[i] = value;
    }
    return 0;
}

/**
 * @brief Saves default values of camera controls to a ctrls_struct.
 *
 * @param cam pointer to the cam struct.
 * @param controls the ctrls_struct to which the default values will be saved
 */
void save_default_struct(const camera_t *cam, ctrls_struct *ctrls)
{
    assert(cam && ctrls);

    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        ctrls->value[i] = cam->ctrls[i].default_val;
    }
}

/**
 * @brief Loads values from a ctrls_struct into the camera.
 *
 * @param cam a pointer to a camera struct
 * @param controls
 * @return exit status. 0 on success, errno on failure.
 */
int load_struct(const camera_t *cam, const ctrls_struct *ctrls)
{
    assert(cam && ctrls);
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        int value = ctrls->value[i];
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
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int reset_ctrl(const camera_t *cam, ctrl_tag_t ctrl)
{

    int ret = set_ctrl(cam, ctrl, cam->ctrls[ctrl].default_val);

    return ret;
}

/**
 * @brief Resets the camera to its default values
 *
 * @param cam a pointer to a cam struct
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int reset_cam(const camera_t *cam)
{

    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
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
 * @brief Prints a single control's value to the console.
 *
 * @param cam Pointer to a cam struct
 * @param ctrl The control to be printed
 * @return exit status. 0 on success, errno on ioctl failure.
 */
int print_ctrl(const camera_t *cam, ctrl_tag_t ctrl)
{
    assert(cam);
    int value;
    int ret = get_ctrl(cam, ctrl, &value);
    if (ret != 0)
    {
        return ret;
    }
    if (ctrl != ACAM_FORMAT)
    {
        printf("%s:%d\n", cam->ctrls[ctrl].name, value);
    }
    else
    {
        printf("%s:%s_%d_%d\n", cam->ctrls[ctrl].name, fmts[value].name, fmts[value].width, fmts[value].height);
    }

    return 0;
}

/**
 * @brief Prints the camera's controls and their current values
 *
 * @param cam
 * @return 0 on success, errno on IOCTL failure.
 */
int print_ctrl_all(const camera_t *cam)
{
    assert(cam);
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        int ret = print_ctrl(cam, i);
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}
/**
 * @brief prints all of the default values for the camera
 *
 * @param cam pointer to the cam struct
 */
void print_defaults(const camera_t *cam)
{
    assert(cam);
    printf("DEFAULTS:\n");
    for (int i = 0; i < __ACAM_CTRL_COUNT; i++)
    {
        printf("%s:%d\n", cam->ctrls[i].name, cam->ctrls[i].default_val);
    }
}

/**
 * @brief prints all of the default values for the camera
 *
 * @param cam pointer to the cam struct
 */
void print_bounds(const camera_t *cam)
{
    assert(cam);
    printf("BOUNDS:\n");
    for (int i = 0; i < __ACAM_CTRL_COUNT - 1; i++)
    {
        printf("%s:[%d,%d]\n", cam->ctrls[i].name, cam->ctrls[i].min_value, cam->ctrls[i].max_value);
    }
}
/**
 * @brief Get the queryctrl object
 *
 * @param cam a pointer to the camera struct
 * @param ctrl The control to be queried.
 * @param query_out The struct to which the queryctrl will be written.
 * @return exit status. 0 on success, errno on IOCTL failure.
 */
int get_queryctrl(camera_t *cam, ctrl_tag_t ctrl, struct v4l2_queryctrl *query_out)
{
    assert(cam && query_out);

    // set up the struct to which we will save ioctl output
    struct v4l2_queryctrl query = {0};
    query.id = v4l2_id[ctrl];

    int ret = ioctl(cam->fd, VIDIOC_QUERYCTRL, &query);

    if (ret == -1)
    {
        DEBUG_PRINT(stderr, "IOCTL failed for %s during camera_boot: %s\n", cam->ctrls[ctrl].name, strerror(errno));
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
 * cam->ctrls: The list of controls belonging to the minicam. The control struct includes name and default/min/max values.
 * cam->buffer: The memory map which is used to store bits before they are written to an image file.
 *
 * @param cam_file the string for the file name of the camera. Usually one of the video files in the /dev mount.
 * @param error Pointer to an integer which will store the error number on failure.
 * On function exit, @param error will be errno on file open/ioctl failure.
 * @return Pointer to cam struct for @param cam_file on success, NULL on failure.
 */
camera_t *open_cam(const char *cam_file, int *error)
{
    assert(cam_file);

    int fd = open(cam_file, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1)
    {
        DEBUG_PERROR("Opening camera file");
        *error = errno;
        return NULL;
    }
    // malloc our camera struct
    camera_t *cam = malloc(sizeof(camera_t));
    if (cam == NULL)
    {
        DEBUG_PERROR("Failed to malloc for camera struct");
        *error = errno;
        return NULL;
    }
    //set our camera's file descriptor
    cam->fd = fd;
    
    //set up the default values, bounds, and names of our camera's controls
    for (int i = 0; i < __ACAM_CTRL_COUNT - 1; i++)
    {
        struct v4l2_queryctrl query;
        int ret = get_queryctrl(cam, i, &query);

        if (ret != 0)
        {
            *error = ret;
        }

        strcpy(cam->ctrls[i].name, query.name);
        cam->ctrls[i].v4l2_id = query.id;
        cam->ctrls[i].max_value = query.maximum;
        cam->ctrls[i].min_value = query.minimum;
        cam->ctrls[i].default_val = query.default_value;
    }
    strcpy(cam->ctrls[ACAM_FORMAT].name, "Format");
    cam->ctrls[ACAM_FORMAT].default_val = ACAM_MJPEG_1920_1080;

    int ret = init_mmap(cam);
    if (ret != 0)
    {
        return NULL;
    }
    

    return cam;
}

/**
 * @brief Deallocates the memory used for the camera and closes the camera's file descriptor.
 *
 * @param cam the pointer to the camera structure.
 * @return exit status. 0 on success, errno on failure
 */
int close_cam(camera_t *cam)
{

    assert(cam);

    int ret = close(cam->fd);
    if (ret == -1)
    {
        free(cam);
        DEBUG_PERROR("Closing camera's file descriptor");
        return errno;
    }
    free(cam);
    return 0;
}
/**
 * @brief Wrapper function for IOCTL. Performs ioctl until definitive success or failure.
 * @return exit status. 0 on success, -1 on failure.
 */
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
int print_caps(const camera_t *cam)
{
    assert(cam != NULL);
    struct v4l2_capability caps = {0};
    if (-1 == xioctl(cam->fd, VIDIOC_QUERYCAP, &caps))
    {
        DEBUG_PERROR("Querying Capabilities");
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
        DEBUG_PERROR("Querying Cropping Capabilities");
        return errno;
    }

    printf("Camera Cropping:\n"
           "  Bounds: %dx%d+%d+%d\n"
           "  Default: %dx%d+%d+%d\n"
           "  Aspect: %d/%d\n",
           cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
           cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
           cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);

    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char fourcc[5] = {0};
    char c, e;
    printf("  FMT : CE Desc\n--------------------\n");
    while (0 == xioctl(cam->fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
        c = fmtdesc.flags & 1 ? 'C' : ' ';
        e = fmtdesc.flags & 2 ? 'E' : ' ';
        printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
        fmtdesc.index++;
    }


    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(cam->fd, VIDIOC_G_FMT, &fmt))
    {
        DEBUG_PERROR("Getting Pixel Format");
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
int init_mmap(camera_t *cam)
{
    assert(cam);

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(cam->fd, VIDIOC_REQBUFS, &req)) // this is the line, for some reason renders camera busy indefinitely
    {
        DEBUG_PERROR("Requesting Buffer");
        return errno;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (-1 == xioctl(cam->fd, VIDIOC_QUERYBUF, &buf))
    {
        DEBUG_PERROR("Querying Buffer");
        return errno;
    }

    cam->buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam->fd, buf.m.offset);

    return 0;
}

/**
 * @brief Captures a single image and writes it to @param file_name
 *
 * @param cam the pointer to the camera file
 * @param file_name The file destination for the image.
 * @return exit status. 0 on success, errno on failure.
 */
int capture_image(camera_t *cam, const char *file_name)
{
    assert(cam && file_name);

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))
    {
        DEBUG_PERROR("Query Buffer");
        return errno;
    }

    if (-1 == xioctl(cam->fd, VIDIOC_STREAMON, &buf.type))
    {
        DEBUG_PERROR("Start Capture");
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
        DEBUG_PERROR("Waiting for Frame");
        return errno;
    }

    if (-1 == xioctl(cam->fd, VIDIOC_DQBUF, &buf))
    {
        DEBUG_PERROR("Retrieving Frame");
        return errno;
    }

    int outfd = open(file_name, O_RDWR | O_CREAT, 0644);
    if (outfd == -1)
    {
        DEBUG_PRINT(stderr, "Problem opening file %s: %s\n", file_name, strerror(errno));
        return errno;
    }
    int ret = write(outfd, cam->buffer, buf.bytesused);
    if (ret == -1)
    {
        DEBUG_PRINT(stderr, "Problem writing to file %s: %s\n", file_name, strerror(errno));
        return errno;
    }

    close(outfd);

    return 0;
}
