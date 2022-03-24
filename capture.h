#pragma once
#ifndef CAPTURE
#define CAPTURE

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

int print_caps(int fd);
int init_mmap(int fd);
int capture_image(int fd, char* file_name);


#endif