
#include "controls.h"
#include "capture.h"

int main() {
    

    camera *x = boot_camera("/dev/video0");

    
    //set_fmt(x, YUYV_1280_720);
    //save_file(x, "happy.txt");
    print_ctrls(x);
    //load_file(x, "happy.txt");
    //printf("%d", get_fmt(x));


    close_cam(x);

    


    
    return 0;
}