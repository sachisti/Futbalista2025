#ifndef __CAMERA_MODULE_H__
#define __CAMERA_MODULE_H__

#include <inttypes.h>


#ifdef __cplusplus 
extern "C"  {
#endif
	
        typedef void (*new_frame_callback)(uint8_t *RGB);

        void setup_camera_callback(new_frame_callback callback_fn);
	int camera_main(int width, int height);

#ifdef __cplusplus
}
#endif



#endif
