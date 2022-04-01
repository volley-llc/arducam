
#include "controls.h"
#include "capture.h"

int main()
{
    int error;

    camera_t *x = open_cam("/dev/video0", &error);

    set_ctrl(x, ACAM_FORMAT, 0);
    close_cam(x);

    return 0;
}