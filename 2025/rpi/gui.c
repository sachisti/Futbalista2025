#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // for usleep()
#include <pthread.h>

extern int sirka, vyska;
extern int pocet_beziacich_vlakien;

static Display *dpy = 0;
static cairo_surface_t *image_surface = 0;
static cairo_surface_t *xlib_surface = 0;
static cairo_t *cr = 0;
static uint32_t *data = 0;
static int stride = 0;
static Window win;

static const int GRID_COLS = 10;
static const int GRID_ROWS = 10;
static const int HORIZONT = 200;

void horizont() {
int horizont_line_width = (stride / 4);
memset(&(data[HORIZONT * horizont_line_width]), 0x66, sirka * 4);
}

void draw_grid()
{
    int data_line_width = (stride / 4);
    int white = 0x00FFFFFF;
    
    for (int i = 1; i < GRID_COLS; i++)
    {
        int x = sirka * i / 10;
        for (int j = 0; j < vyska; j++)
            data[j * data_line_width + x] = white;       
    }
    
    for (int i = 1; i < GRID_ROWS; i++)
    {
        int y = vyska * i / 10;
        memset(&(data[y * data_line_width]), 0xFF, sirka * 4);
    }
}


void gui_putimage(uint8_t *img)
{
    if (img == 0) return;
    int line_width = sirka * 3;
    int data_line_width = (stride / 4);
    for (int y = 0; y < vyska; y++) {
        for (int x = 0; x < sirka; x++) {
            int ofs = (vyska - y - 1) * line_width + (sirka - x - 1) * 3;
            uint8_t b = img[ofs];
            uint8_t g = img[ofs + 1];
            uint8_t r = img[ofs + 2];
            data[y * data_line_width + x] = (((int)(r)) << 16) | (((int)g) << 8) | b;
        }
    }
    
    draw_grid();
    
    XClearArea(dpy, win, 0, 0, 0, 0, True);
}

void *gui_main(void *args)
{
    // Event loop
    XEvent e;
    int running = 1;
    while (running) {
        while (XPending(dpy)) {
            //printf("."); fflush(stdout);
            XNextEvent(dpy, &e);
            switch (e.type) {
                case Expose:
                    //printf("redrawing\n");
                    cairo_set_source_surface(cr, image_surface, 0, 0);
                    cairo_paint(cr);
                    cairo_surface_flush(cairo_get_target(cr));
                    break;
                case KeyPress:
                    printf("(%d)", e.xkey.keycode);
                    if (e.xkey.keycode == 24) running = 0;
                    else if (e.xkey.keycode == 45)
                        XClearArea(dpy, win, 0, 0, 0, 0, True);
                    break;
            }
        }
        usleep(250000); 
    }
    return 0;
}

void setup_gui()
{
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);

    XSetWindowAttributes attrs;
    attrs.background_pixmap = None;
    attrs.event_mask = ExposureMask | KeyPressMask;

    win = XCreateWindow(dpy, root, 0, 0, sirka, vyska, 0,
                              CopyFromParent, InputOutput, CopyFromParent, CWBackPixmap | CWEventMask, &attrs);

    XSelectInput(dpy, win, ExposureMask | KeyPressMask);
    XMapWindow(dpy, win);

    Visual *visual = DefaultVisual(dpy, screen);
    xlib_surface = cairo_xlib_surface_create(dpy, win, visual, sirka, vyska);
    cr = cairo_create(xlib_surface);

    // Create image surface with directly accessible memory
    image_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, sirka, vyska);
    data = (uint32_t *)cairo_image_surface_get_data(image_surface);
    stride = cairo_image_surface_get_stride(image_surface);
    
    pthread_t t;
    if (pthread_create(&t, 0, gui_main, 0) != 0)
    {
      perror("nepodarilo sa vytvorit thread");
      exit(-1);
    }
    else pocet_beziacich_vlakien++;
}

void shutdown_gui()
{
    cairo_destroy(cr);
    cairo_surface_destroy(image_surface);
    cairo_surface_destroy(xlib_surface);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

