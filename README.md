# arducam
Repo for the arducam mini-cam that's integrated into the trainer head

API:

********CAPTURE.C******************
______________________________________________________________________
int print_caps(int fd)
@param fd: The camera's file descriptor (usually in /dev mount)

Run this fucntion to print all of the camera's specifications.
This includes controls, recording modes, and pixel ratio.

Returns exit status, along with perror if exit status is not 0.
______________________________________________________________________
int init_mmap(int fd)
@param fd: The camera's file descriptor (usually in /dev mount)

Initializes the buffer that will be used to store image data.
Must be run before capture_image.

Returns exit status, along with perror if exit status is not 0.
______________________________________________________________________
int capture_image(int fd)
@param fd: The camera's file descriptor (usually in /dev mount)

Writes image capture data to the buffer, then saves it to a new
file. The file is saved to /images folder in the root program
folder.

Returns exit status, along with perror if exit status is not 0.
______________________________________________________________________
int quick_cap_frame(int fd);
@param fd: The camera's file descriptor (usually in /dev mount)

Calls init_mmap and capture_image in succession.

Returns exit status; 1 if error on either of the above function calls
and 0 on success.

***********************************

*********CONTROLS.C****************
______________________________________________________________________
int set_ctrl(int ctrl, int value, struct camera *cam);
@param int ctrl: The control to have its value modified (use the control's ENUM code)
@param int value: The value to which the control will be set
@param struct camera *cam: The structure which holds the current control values.

This function changes the existing value in a control's register to a new one.
The structure which maintains the values (ctrl_vals) will be automatically
updated, and the camera's control value will change accordingly. Changes to
the camera's registers can be observed on the camera's video output in real-time.

Returns: exit status.
______________________________________________________________________
int get_ctrl(struct camera *cam, int ctrl, int* value);
@param struct camera *cam: The structure which holds the current control values.
@param int ctrl: The control to have its value read (use the control's ENUM code)
@param int* value: The int pointer in which the control's value will be stored
after exiting the function.

This function reads in a value from the camera and stores it in int* value.

Returns: exit status.
______________________________________________________________________
int write_ctrls_to_file(struct camera *cam)
@param struct camera *cam: The structure which holds the current control values.

Writes the control values stored in @param *cam to a file named
ctrl_values.txt. This file is located in the build folder.

returns: exit status.
______________________________________________________________________
int load_ctrls_from_file(struct camera *cam)
@param struct camera *cam: The structure which holds the current control values.

Loads control values from ctrl_values.txt. Values are places into both the *cam 
struct's "values" field and the camera's registers.

returns: exit status.
______________________________________________________________________
int restore_defaults(struct camera *cam)
@param struct camera *cam: The structure which holds the current control values.

Restores all values in camera's registers to their defaults. Updates @param
*cam to reflect these new, default values.

returns: exit status.
______________________________________________________________________
struct camera* boot_camera();

This function initializes the structure that holds the camera's file
descriptor, control values, pixel ratio, and pixel format. All of the 
camera's control values in the struct will reflect the camera's current
values.

Returns struct cam: the structure which holds the camera's info.
______________________________________________________________________
void camera_close(struct camera *cam)
@param struct camera *cam: The structure which holds the current control values.

Closes the camera's file descriptor and frees the *cam struct from the heap.
