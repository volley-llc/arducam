
#include "controls.h"
#include "capture.h"

int main() {
    int ret;
    int fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
    set_ctrl(fd, BRIGHTNESS, 0);
    write_params(fd);
    //quick_cap_frame(fd);
    


    close(fd);
    return 0;
}