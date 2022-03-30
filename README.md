# arducam
Repo for the arducam mini-cam that's integrated into the trainer head

API:

______________________________________________________________________
int print_caps(camera \*cam)
Prints capabilites of the camera, along with selected
recording modes.

@param cam the pointer to the camera struct.
@return exit status. 0 on success, errno on failure.
______________________________________________________________________
int capture\_image(camera \*cam, char \*file\_name)

@brief Captures a single image and writes it to @param file_name

@param cam the pointer to the camera file
@param file_name The file destination for the image.
@return exit status. 0 on success, errno on failure.

______________________________________________________________________

int set\_ctrl(camera \*cam, ctrl_tag ctrl, int value)
/**
 * @brief Sets the value of a control.
 * 
 * @param cam pointer to the cam struct
 * @param ctrl the ctrl_tag ENUM
 * @param value the value to which we will set @param ctrl
 * @return exit status. 0 on success, -1 on invalid @param ctrl argument,
 * errno on IOCTL failure. Setting WHITE_BALANCE_TEMPERATURE or EXPOSURE_ABSOLUTE
 * while their respective auto-set functions are on will result in success. Setting
 * a control to a value above/below its upper/lower bounds will result in success and
 * set the control's register to its max/min.
 */
______________________________________________________________________
int get\_ctrl(camera \*cam, ctrl_tag ctrl, int\* value)
/**
 * @brief Get's the value of a control.
 * 
 * @param cam a pointer to the cam struct
 * @param ctrl the ctrl_tag ENUM
 * @param value the int into which @param ctrl's value will be passed. If @param ctrl is FORMAT, this will be
 * the number associated with the camera's current format ENUM.
 * @return exit status. 0 on success, -1 if an invalid ctrl_tag is entered, -2 if IOCTL returns
 * invalid info, errno on IOCTL failure.
 */
______________________________________________________________________
int load\_file(camera \*cam, const char\* fname)
/**
 * @brief Loads values from file into camera.
 * 
 * @param cam pointer to the cam struct
 * @param fname String of filename from which to load data.
 * @return exit status. 0 on success, errno on IOCTL failure for setting register or file open failure,
 * -2 for file formatting error, -3 for pixel format incorrect entry formatting, -4 for
 * too many control entries, -5 for too few control entries, -6 for invalid pixel format
 * aspect ratio.
 */
______________________________________________________________________
int save\_file(camera \*cam, const char\* fname)
/**
 * @brief Writes camera's current values to a file.
 * 
 * @param cam pointer to cam struct.
 * @param fname String of filename to which the output will be written.
 * @return exit status. 0 on success, errno on IOCTL/file write failure, -2 if
 * bad information retrieved from camera.
 */
______________________________________________________________________
int save\_struct(camera \*cam, ctrls_struct \*controls)
/**
 * @brief Saves a struct of the camera's current control values
 * 
 * @param cam the pointer to the cam struct
 * @param controls The struct to which the camera's current control values with be saved.
 * @return exit status. 0 on success, -2 if IOCTL returns with invalid info,
 * errno on IOCTL failure.
 */
______________________________________________________________________
int load\_struct(camera \*cam, ctrls_struct \*controls)
/**
 * @brief Loads values from a ctrls_struct into the camera.
 * 
 * @param cam a pointer to a camera struct
 * @param controls 
 * @return exit status. 0 on success, errno on failure.
 */
______________________________________________________________________
int boot\_camera(camera \*cam, char \*cam_file)

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
______________________________________________________________________
int close_cam(camera *cam)
/**
 * @brief Deallocates the memory used for the camera and closes the camera's file descriptor.
 * 
 * @param cam the pointer to the camera structure.
 * @return exit status. 0 on success, errno on failure
 */
