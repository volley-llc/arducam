
#include "controls.h"
#include "capture.h"

int main()
{

    camera *x = malloc(sizeof(camera));
    boot_camera(x, "/dev/video0");

    int value;
    set_ctrl(x, FORMAT, 0);
    get_ctrl(x, FORMAT, &value);
    printf("%d", value);

    // new little problem -- need to know how to "free" mmap such as to allow for changes in the pixel format.
    // There are tons of ways to do this -- get Steve's advice?
    // Ask about mmap leaks.

    close_cam(x);

    return 0;
}