#include <cstring>
#include <sys/mman.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <linux/videodev2.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

//odkomentujte nasledovny riadok ak kamera nepodporuje BGR format
//pozri v4l2-ctl -d /dev/videoX --list-formats

#include "v4l_module.h"

#define TIMEOUT_SEC 3

extern int sirka, vyska;
extern int pocet_beziacich_vlakien;
static int system_runs = 1;

new_frame_callback frame_callback;

uint8_t *buffer;


extern "C" void setup_camera_callback(new_frame_callback callback_fn)
{
    frame_callback = callback_fn;
}

static int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

int init_mmap(int fd)
{
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
 
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        perror("nepodarilo sa inicializovat mmap buffer");
        return 1;
    }
 
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
    {
        perror("nepodarilo sa ziskat mmap buffer");
        return 1;
    }
 
    buffer = (uint8_t *)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
 
    return 0;
}
 

int setup_format(int fd)
{
        struct v4l2_format fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = sirka;
        fmt.fmt.pix.height = vyska;
        
        // ak vasa kamera nepodporuje BGR24, m ozno podporuje YUV420,
        // ale v tom pripade bude treba obrazok spracovavat v tom
        // formate, alebo si ho skonvertovat...

#ifdef POUZI_YUV
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
#else
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
#endif

        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        
        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        {
            perror("nepodarilo sa nastavit format");
            return 1;
        }

        return 0;
}
 

long long usec()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (1000000L * (long long)tv.tv_sec) + tv.tv_usec;
}

int process_frames(int fd)
{
    int pocitadlo = 0;
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    
    if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
    {
        perror("nepodarilo sa zapnut snimanie obrazu");
        return 1;
    }
 
    long long tm = usec();

    while (system_runs)
    {
      if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
      {
        perror("nepodarilo sa poziadat o buffer");
        return 1;
      }
      
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(fd, &fds);
      struct timeval tv = {0};
      tv.tv_sec = 2;
      int rv = select(fd+1, &fds, NULL, NULL, &tv);
      if(-1 == rv)
      {
          perror("pocas cakania na obrazok doslo k chybe");
          return 1;
      }
  
      if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
      {
          perror("nepodarilo sa ziskat obrazok");
          return 1;
      }

      uint8_t *p = (uint8_t *)buffer;
      frame_callback(p);
      //printf("poc=%d\n", pocitadlo++);
      
    } 
    return 0;
}
 
extern "C" void *camera_main(void *args)
{
	int width = sirka;
	int height = vyska;
        int fd;
	const char *device = "/dev/video0";
	//if (argc > 1) device = argv[1];
 
        fd = open(device, O_RDWR);
        if (fd == -1)
        {
                perror("nepodarilo sa otvorit zariadenie /dev/videoN ...");
                return 0;
        }
        if(setup_format(fd))
            return 0;
        
        if(init_mmap(fd))
            return 0;
            
	process_frames(fd);

        close(fd);
        return 0;
}

extern "C" void start_camera_thread()
{
    pthread_t t;
    if (pthread_create(&t, 0, camera_main, 0) != 0)
    {
      perror("nepodarilo sa vytvorit thread");
      exit(-1);
    }
    else pocet_beziacich_vlakien++;
}

