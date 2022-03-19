
#include "controls.h"
#include "capture.h"

int main() {
    int ret;
    int fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
    //print_caps(fd);
    //set_ctrl(fd, BRIGHTNESS, 0);
    //write_params(fd);
    //quick_cap_frame(fd);
    print_caps(fd);
    struct ctrl_struct *x = boot_camera(fd);
    printf("%d\n", x->values[BRIGHTNESS]);
    set_ctrl(fd, BRIGHTNESS, 40, x);
    printf("%d\n", x->values[BRIGHTNESS]);
    
    free(x);

    int value;
    


    close(fd);
    return 0;
}