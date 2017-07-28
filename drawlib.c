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

FBINFO* init() {
		FBINFO *data = malloc(sizeof(*data));
		int fbfd = 0;
		struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
		char *fbp;
        long int screensize = 0;

        /* Open the file for reading and writing */
        fbfd = open("/dev/fb0", O_RDWR);
        if (!fbfd) {
                printf("Error: cannot open framebuffer device.\n");
                exit(1);
        }
        printf("The framebuffer device was opened successfully.\n");

		/* Get fixed screen information */
        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
               printf("Error reading fixed information.\n");
                exit(2);
        }

        /* Get variable screen information */
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
                printf("Error reading variable information.\n");
                exit(3);
        }
		printf("Screen Size - x:%d y:%d\n", vinfo.xres, vinfo.yres);

        /* Figure out the size of the screen in bytes */
        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
        printf("Screen size is %ld\n",screensize);
        printf("Vinfo.bpp = %d\n",vinfo.bits_per_pixel);

        /* Map the device to memory */
        fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);
        if (fbp == MAP_FAILED) {
        /*if ((int)fbp == -1) {*/
                printf("Error: failed to map framebuffer device to memory.\n");
                exit(4);
        }
		printf("The framebuffer device was mapped to memory successfully.\n");

		data->fbfd = fbfd;
		data->vinfo = vinfo;
		data->finfo = finfo;
		data->fbp = fbp;
		data->screensize = screensize;


		return data;
}

void draw(FBINFO *data, PIXEL *pixel) {
		struct fb_var_screeninfo vinfo = data->vinfo;
        struct fb_fix_screeninfo finfo = data->finfo;
		char *fbp = data->fbp;
        long int location = 0;

		if(*pixel->x < 0 || *pixel->y < 0) { return; }
		if(*pixel->x >= vinfo.xres || *pixel->y >= vinfo.yres) { return; }

        /* Figure out where in memory to put the pixel */
        location = (*pixel->x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (*pixel->y+vinfo.yoffset) * finfo.line_length;

        *(fbp + location + 0) = pixel->colour->b; /* blue */
        *(fbp + location + 1) = pixel->colour->g; /* green */
        *(fbp + location + 2) = pixel->colour->r; /* red */
        *(fbp + location + 3) = pixel->colour->t; /* transparency */
}

void end(FBINFO *data) {
        munmap(data->fbp, data->screensize);
        close(data->fbfd);
		free(data);
}

void test() {
	FBINFO *data = NULL;
	int i = 0;
	int x;
	int y;
	RGBT red;
	PIXEL toDraw;
	toDraw.x = &x;
	toDraw.y = &y;
	toDraw.colour = &red;

	red.r = 255;

	printf("init...\n");
	data = init();

	printf("writing some pixels...\n");
	for(i = 0; i < 200; i++) {
		x = 20+i;
		y = 20;
		draw(data, &toDraw);
	}

	printf("end...\n");
	end(data);
}


void drawCircle(FBINFO *data, int r, PIXEL *pixel) {
	int i, j;
	int rSqr = r*r;
	PIXEL tempPix;
	int t_x;
	int t_y;
	float randb;
	float randa;

	tempPix.colour = pixel->colour;
	tempPix.x = &t_x;
	tempPix.y = &t_y;

	randa = (rand()%20)+1;
	randb = (rand()%20)+1;
	/* a*a + b*b = c*c */

	/* pixels in the square contained by the circle */
	for(j = -r; j < r; j++) {
		for(i = -r; i < r; i++) {
			if(i*i * ((randa/10)+1) + j*j * ((randb/20)+1) < rSqr) {
				*tempPix.x = *pixel->x + i;
				*tempPix.y = *pixel->y + j;
				draw(data, &tempPix);
			}
		}
	}
}

void clear_screen(FBINFO *data) {
	int i;
	int j;
	PIXEL pixel;
	RGBT black;
	pixel.colour = &black;

	for(j = 0; j < data->vinfo.yres; j++) {
		for(i = 0; i < data->vinfo.xres; i++) {
			pixel.x = &i;
			pixel.y = &j;
			draw(data, &pixel);
		}
	}
}

void rotate_about_point(PIXEL *pixel, int px, int py, float angle) {
	*pixel->x -= px;
	*pixel->y -= py;
	
	*pixel->x = (int)(*pixel->x * cos(angle) - *pixel->y * sin(angle));
	*pixel->y = (int)(*pixel->x * sin(angle) + *pixel->y * cos(angle));

	*pixel->x += px;
	*pixel->y += py;
}

#define PI 3.14159f

int main() {
	FBINFO *data = NULL;
	/*RGBT green;
	RGBT blue;*/
	RGBT randColour;
	int i;
	int width;
	int height;
	int screensaver = 1;
	int degrees;
	float radians;
	int temp_x;
	int temp_y;
	PIXEL randPixel;
	struct timeval time_of_day;

	randPixel.colour = &randColour;
	randPixel.x = &temp_x;
	randPixel.y = &temp_y;

	srand(time(NULL));

	/*green.g = 255;
	blue.b = 255;*/

	data = init();
	/*
	drawCircle(data, 20, 30, 5, &green);
	drawCircle(data, 600, 500, 50, &green);
	drawCircle(data, 650, 420, 60, &blue);
	drawCircle(data, 500, 25, 50, &green); */
	while(screensaver && 0) {
		sleep(0.1);
		randColour.r = rand()%255;
		randColour.g = rand()%255;
		randColour.b = rand()%255;
		randColour.t = rand()%255;

		gettimeofday(&time_of_day, NULL);

		/*degrees = 60;*/
		radians = sin(((double)time_of_day.tv_usec)/(1000000)) * 2 * PI;
		for(height = 0+400; height < 100+400/*rand()%data->vinfo.yres*/; height++) {
			for(width = 0+400; width < 100+400/*rand()%data->vinfo.xres*/; width++) {
				temp_x = width;
				temp_y = height;

				rotate_about_point(&randPixel, 450, 450, radians);
				draw(data, &randPixel);
			}
		}
	}

	while(screensaver) {
		sleep(1);
		/* draw some random circles */
		for(i = 0; i < rand()%20; i++) {
			randColour.r = rand()%255;
			randColour.g = rand()%255;
			randColour.b = rand()%255;
			randColour.t = rand()%255;

			*randPixel.x = rand()%data->vinfo.xres;
			*randPixel.y = rand()%data->vinfo.yres;

			drawCircle(data, rand()%(data->vinfo.xres/5), &randPixel);
		}
	}

	end(data);
	
	return 0;
}
