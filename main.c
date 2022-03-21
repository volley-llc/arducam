
#include "controls.h"
#include "capture.h"

int main() {
    int ret;
    

    struct camera *x = boot_camera();

    print_caps(x->fd);
    quick_cap_frame(x->fd);


    camera_close(x);

    


    
    return 0;
}