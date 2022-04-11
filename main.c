
#include "controls.h"


int main()
{

    int error;

    acam_camera_t *x = acam_open("/dev/video0", &error);
    acam_close(x);





    return 0;
}
