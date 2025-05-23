#ifndef __V4L_MODULE_H__
#define __V4L_MODULE_H__

#include <inttypes.h>


#ifdef __cplusplus 
extern "C"  {
#endif
	
        typedef void (*new_frame_callback)(uint8_t *RGB);

        void setup_camera_callback(new_frame_callback callback_fn);

        void start_camera_thread();


#ifdef __cplusplus
}
#endif



#endif
