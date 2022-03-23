
#include "controls.h"
#include "capture.h"

int main() {
    

    camera *x = boot_camera("/dev/video0");

    

    //print_caps(x->fd);
    //quick_cap_frame(x->fd);
    /*
    set_ctrl(x, BRIGHTNESS, 50);
    set_ctrl(x, CONTRAST, 50);
    reset(x);
    int val;
    get_ctrl(x, BRIGHTNESS, &val);
    printf("%d", val);
    */
   
    ctrls_struct *ctrls = malloc(sizeof(ctrls_struct));
    save_default_struct(x, ctrls);
    load_struct(x, ctrls);


    //printf("%d", ctrls->contrast);
    //print_ctrls(x);


    print_caps(x->fd);

    close_cam(x);

    


    
    return 0;
}