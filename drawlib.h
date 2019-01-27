#ifndef DRAWLIB_H
#define DRAWLIB_H

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <sys/time.h>



typedef struct fbinfo {

		int fbfd;
		struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
		char *fbp;
        long int screensize;

} FBINFO;

typedef struct rgbt {
	int r;
	int g;
	int b;
	int t;
} RGBT;

typedef struct pixel {
	int *x;
	int *y;
	RGBT *colour;
} PIXEL;


/** sets up the framebuffer, allocates FBINFO to use when drawing
 * you should use end(FBINFO *data); to free
 **/
FBINFO* init();

/** safely frees the FBINFO* given by init */
void end(FBINFO *data);

/** checks and draws a single pixel to the screen */
void draw(FBINFO *data, PIXEL *pixel);

/** creates its own FBINFO and tries to draw some red pixels in the upper left corner */
void test();



/* ===============================================
 * special circle methods
 * you can draw any shape within a square defined by the relevant data and function ptr
 * ==============================================
 */

typedef struct oval_data {
	int radius;
	int centre_offset;
	float x_mul;
	float y_mul;
} OVAL_DATA;

/** function ptr for use with 'drawSpecialCircle'
 *	takes a 'OVAL_DATA*' which defines the parameters of the oval
 **/
int oval(const void *x_distance, const void *y_distance, const void *data);



typedef struct circle_data {
	int radius;
	int centre_offset;
} CIRCLE_DATA;

/** function ptr for use with 'drawSpecialCircle'
 *	takes a 'CIRCLE_DATA*' which defines the parameters of the circle
 **/
int circle(const void *x_distance, const void *y_distance, const void *circle_data);



typedef struct parabola_data {
	int lge_flag;/* -1 for less, 0 for equal, 1 for greater */
	/* y = ax^2 + bx + c */
	float a;
	float b;
	float c;
	int side_length;
} PARABOLA_DATA;

/** function ptr for use with 'drawSpecialCircle'
 *	takes a 'PARABOLA_DATA*' which defines the parameters of the parabola
 **/
int parabola(const void *x_distance, const void *y_distance, const void *parabola_data);

/** function ptr for use with 'drawSpecialCircle'
 *	doesn't need anything for 'nothing', thats just there to fit the function ptr definition
 **/
int square(const void *x, const void *y, const void *nothing);

/** function to draw any arbitrary shape within a square of a solid colour
 * @param data the framebuffer data to draw to, created by init()
 * @see init()
 * @param pixel defines the top left position and colour with which to draw the special circle
 * @param side_len side length of the square within which to draw the shape
 * @param comp_data the data struct created specifically for the comparison function pointer
 * @see circle()
 * @param comparison function that chooses weather to draw a pixel here or not
 **/
void drawSpecialCircle(FBINFO *data, PIXEL *pixel, int side_len, const void *comp_data, int (*comparison)(const void *x, const void *y, const void *data));

/* ===============================================
 * end special circle methods
 * ==============================================
 */


/** draw a simple circle with a radius and a solid colour
 */
void drawCircle(FBINFO *data, int r, PIXEL *pixel);

/** draw over every pixel with black
 */
void clear_screen(FBINFO *data);

/** rotates the pixel around a point
 * the point is defined by px, py
 * modifies the coordinate values in pixel param
 */
void rotate_about_point(PIXEL *pixel, int px, int py, float angle);






#endif
