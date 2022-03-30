
#include "controls.h"

int main()
{

    camera *x = malloc(sizeof(camera));
    boot_camera(x, "/dev/video0");



    close_cam(x);

    return 0;
}
