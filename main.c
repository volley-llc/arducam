
#include "controls.h"


int main()
{

    int error;

    acam_camera_t *x = acam_open_cam("/dev/video0", &error);


    acam_close_cam(x);





    return 0;
}