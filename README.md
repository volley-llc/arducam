
# Arducam Library API

#### acam_camera_t *acam_open_cam(const char *cam_file, int *error)
Boots the camera.
* Initializes the following in the camera struct:
 `cam->fd`: the camera's file descriptor.
`cam->ctrls`: The list of controls belonging to the minicam. The control struct includes name, as well as default/min/max values.
`cam->buffer` The memory map which is used to store bits before they are written to an image file.
* `@param cam_file` the string for the file name of the camera. Usually one of the video files in the /dev mount.
* `@param error` Pointer to an integer which will store the error number on failure.
* On function exit, @param error will be 0 on success and errno on file open/ioctl failure.
* `@return acam_camera_t *cam` Pointer to cam struct on success, NULL on failure.
____________________________________________________________________
#### int acam_close_cam(acam_camera_t *cam)
Deallocates the memory used for the camera and closes the camera's file descriptor.
* `@param cam` the pointer to the camera structure.
* `@return` exit status. 0 on success, errno on failure
_____________________
#### int acam_capture_image(acam_camera_t *cam, const char *file_name)
Captures a single image and writes it to @param file_name
* `@param cam` the pointer to the camera file
* `@param file_name` The file destination for the image.
* `@return` exit status. 0 on success, errno on ioctl failure, ENOMEM on failure
	 to map/unmap memory to/from user space.
_________________________
#### int acam_get_ctrl(const acam_camera_t *cam, acam_ctrl_tag_t ctrl, int *value)
Get's the value of a control.
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
______________________________
####  int acam_save_json(const acam_camera_t *cam, const char *fname)
Writes camera's current values to a file.
* `@param cam` pointer to cam struct.
* `@param fname` String of filename to which the output will be written.
* `@return` exit status. 0 on success, errno on IOCTL/file write failure.
______________________________________________________
####  int acam_load_json(const acam_camera_t *cam, const char *fname)
Loads values from file into camera.
* `@param cam` pointer to the cam struct
* `@param fname` String of filename from which to load data.
* `@return` exit status. 0 on success, errno on IOCTL failure for setting register, errno for file open failure, EBADF for file formatting error.

NOTE: Check error log for file formatting error details.
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
