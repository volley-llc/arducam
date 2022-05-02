
# Arducam Library
______________________________________________________________________________________
# Brief Usage guide.
This library is used to control the arducam UBO212. It contains functions that are able to change the camera's settings and to take a single image in the camera and store it in the user's memory space.

In order to use to use the camera and take a picture with it, the following actions must be performed:
1. The camera must be opened using the arducam UBO212's file descriptor.
2.  A buffer must be created in which to store an image.
3.  The image must be written to the buffer.

When finished, the memory for the camera and the buffer must be freed using their respective freeing functions. Here is a typical example of what code using this library looks like:

`int error = 0`;

`acam_camera_t *x = acam_open("/dev/video0", &error);` Open the arducam

`acam_set_ctrl(x, ACAM_FORMAT, ACAM_MJPEG_1920_1080);` Set control values that you would like to modify from their defaults. This is not necessary

`acam_buffer_t *buffer = acam_create_buffer(x, &error);`create a buffer

`acam_capture_image(x, buffer);` capture an image

`acam_write_to_file("image.jpg", buffer);` perform your desired action with the buffer

`acam_destroy_buffer(buffer);` deallocate the buffer

`acam_close(x);`deallocate the camera

Further notes/warnings about usage:
* It is best practice to set pixel format before creating buffers. This will ensure that all buffers are of the correct length to store images. If pixel format must be changed, it is encouraged to close and reopen the camera using acam_open() and acam_close() before changing the pixel format.
* If multithreading, changing the camera's pixel format at the same time as a buffer is being created/a picture is being taken will result in undefined behavior. 
___________________________________________________________________
# API

#### acam_camera_t *acam_open(const char *cam_file, int *error)
Boots the camera.
* Initializes the following in the camera struct:
 `cam->fd`: the camera's file descriptor.
`cam->ctrls`: The list of controls belonging to the minicam. The control struct includes name, as well as default/min/max values.
`cam->buffer` The memory map which is used to store bits before they are written to an image file.
`cam->stream_on`: Turns on once a buffer has been requested. Enables format to be changed after creating a buffer,
although this is highly discouraged.
* `@param cam_file` the string for the file name of the camera. Usually one of the video files in the /dev mount.
* `@param error` Pointer to an integer which will store the error number on failure.
* On function exit, @param error will be 0 on success and errno on file open/ioctl failure.
* `@return acam_camera_t *cam` Pointer to cam struct on success, NULL on failure.
____________________________________________________________________
#### int acam_close(acam_camera_t *cam)
Deallocates the memory used for the camera and closes the camera's file descriptor.
* `@param cam` the pointer to the camera structure.
* `@return` exit status. 0 on success, errno on failure
____________________________________________________________________
#### int acam_capture_image(acam_camera_t *cam, const char *file_name)
Captures a single image and writes it to @param buffer
* `@param cam` the pointer to the camera file
* @param `buffer` The buffer which will store the captured image.
* `@return` exit status. 0 on success, errno on ioctl failure, ENOMEM on failure
to map/unmap memory to/from user space, EINVAL if the buffer is of incorrect
size.
______________________________________________________________________
#### acam_buffer_t *acam_create_buffer(acam_camera_t *cam, int *error)
Initializes the buffer that stores bytes captured by the camera. Multiple
buffers exist at any given time.
 * `@param cam` the pointer to the camera object.
 * `@param error` keeps track of error code on failure.
 * `@return acam_buffer_t`, which stores a buffer calibrated for the camera's
 * current pixel format/aspect ratio.
_____________________________________________________________________
#### int acam_destroy_buffer(acam_buffer_t *buffer)
Deallocates memory for a buffer created with 
acam_create_buffer.

 * `@param buffer` The buffer struct to be destroyed
 * `@return` errno on munmap failure, 0 on success.
_____________________________________________________________________
####int acam_write_to_file(const char *file_name, const acam_buffer_t *buffer)
Writes an image from the camera to a file. Needs a buffer to have been
created with acam_create_buffer. This buffer must be passed into the function.
 * `@param cam` pointer to the cam struct
 * `@param buffer` The acam_buffer_t which will store the image. The image bytes are stored
 * in buffer->buf.
 * `@return` int errno on failure, 0 on success.
_____________________________________________________________________
#### int acam_get_ctrl(const acam_camera_t *cam, acam_ctrl_tag_t ctrl, int *value)
Gets the value of a control.
* `@param cam` a pointer to the cam struct
* `@param ctrl` the acam_ctrl_tag ENUM
* `@param value` the int into which @param ctrl's value will be passed. If @param ctrl is FORMAT, this will be the number associated with the camera's current format ENUM.
* `@return` exit status. 0 on success, errno on IOCTL failure, EBADF if the function retrieved a pixel format not supported by ARDUCAM.
_____________________________________________________________
####  int acam_set_ctrl(const acam_camera_t *cam, acam_ctrl_tag_t ctrl, int value)
Sets the value of a control.
* `@param cam` pointer to the cam struct
* `@param ctrl` the acam_ctrl_tag ENUM
* `@param value` the value to which we will set @param ctrl
* `@return` exit status. 0 on success, errno on IOCTL failure.
	
NOTE: Setting WHITE_BALANCE_TEMPERATURE or EXPOSURE_ABSOLUTE while their respective auto-set functions are on will result in success. Setting a control to a value above/below its upper/lower bounds will both result in success and set the control's register to its max/min.
_______________________________________________
####  int acam_save_struct(const acam_camera_t *cam, acam_ctrls_struct *ctrls)
Saves a struct of the camera's current control values
* `@param cam` the pointer to the cam struct
* `@param ctrls` The struct to which the camera's current control values with be saved.
* `@return` exit status. 0 on success, errno on IOCTL failure.
_________________________________________
#### void acam_save_default_struct(const acam_camera_t *cam, acam_ctrls_struct *ctrls)
Saves default values of camera controls to a acam_ctrls_struct.
* `@param cam` pointer to the cam struct.
* `@param ctrls` the acam_ctrls_struct to which the default values will be saved
________________________________________

#### int acam_load_struct(const acam_camera_t *cam, const acam_ctrls_struct *ctrls)
Loads values from a acam_ctrls_struct into the camera.
* `@param cam` a pointer to a camera struct
* `@param ctrls` the ctrls struct
* `@return` exit status. 0 on success, errno on failure.
________________________________________

#### int acam_reset_ctrl(const acam_camera_t *cam, acam_ctrl_tag_t ctrl)
Resets a control to its default value
* `@param cam` a pointer to a cam struct
* `@param ctrl` the control value which will be reset
* `@return` exit status. 0 on success, errno on IOCTL failure.
____________________________________________________

#### int acam_reset_all(const acam_camera_t *cam)
Resets the camera to all of its default values
* `@param cam` a pointer to a cam struct
* `@return` exit status. 0 on success, errno on IOCTL failure.
___________________________________________

#### int acam_print_ctrl(const acam_camera_t *cam, acam_ctrl_tag_t ctrl)
Prints a single control's value.
* `@param cam` Pointer to a cam struct
* `@param ctrl` The control to be printed
* `@return` exit status. 0 on success, errno on ioctl failure.
__________________________________________________
#### int acam_print_ctrl_all(const acam_camera_t *cam)
Prints all of the camera's controls and their current values
* `@param` cam a pointer to the cam struct
* `@return` 0 on success, errno on IOCTL failure.
______________________________
#### void acam_print_defaults(const acam_camera_t *cam)
prints all of the default values for the camera
* `@param cam` pointer to the cam struct
_____________________________________________
#### void acam_print_bounds(const acam_camera_t *cam)
prints all of the bounds values for the camera
* `@param cam` pointer to the cam struct
______________________________________
#### int acam_print_caps(const acam_camera_t *cam)
Prints capabilites of the camera, along with selected recording modes.
* `@param cam` the pointer to the camera struct.
* `@return` exit status. 0 on success, errno on failure.
