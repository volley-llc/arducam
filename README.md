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
int set_ctrl(int fd, int ctrl, int value, struct ctrl_struct *ctrl_vals);
@param int fd: The camera's file descriptor (usually in /dev mount)
@param int ctrl: The control to have its value modified (use the control's ENUM code)
@param int value: The value to which the control will be set
ctrl_struct *ctrl vals: The structure which holds the current control values.

This function changes the existing value in a control's register to a new one.
The structure which maintains the values (ctrl_vals) will be automatically
updated, and the camera's control value will change accordingly. Changes to
the camera's registers can be observed on the camera's video output in real-time.

Returns: exit status.
______________________________________________________________________
int get_ctrl(int fd, int ctrl, int* value);
@param int fd: The camera's file descriptor (usually in /dev mount)
@param int ctrl: The control to have its value read (use the control's ENUM code)
@param int* value: The int pointer in which the control's value will be stored
after exiting the function.

This function reads in a value from the camera and stores it in int* value.

Returns: exit status.
______________________________________________________________________
struct ctrl_struct* boot_camera(int fd);
@param int fd: The camera's file descriptor (usually in /dev mount)

This function initializes the structure that holds the camera's control
values. All of the camera's control values in the struct will reflect the
camera's current values.

Returns struct ctrl_struct: the structure which holds the control values.
______________________________________________________________________
int write_params(int fd);
@param int fd: The camera's file descriptor (usually in /dev mount)

Not implemented
