#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>

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

void draw(FBINFO *data, int x, int y, RGBT *colour) {
		struct fb_var_screeninfo vinfo = data->vinfo;
        struct fb_fix_screeninfo finfo = data->finfo;
		char *fbp = data->fbp;
        long int location = 0;

		if(x < 0 || y < 0) { return; }
		if(x >= vinfo.xres || y >= vinfo.yres) { return; }

        /* Figure out where in memory to put the pixel */
        location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;

        *(fbp + location + 0) = colour->b; /* blue */
        *(fbp + location + 1) = colour->g; /* green */
        *(fbp + location + 2) = colour->r; /* red */
        *(fbp + location + 3) = colour->t; /* transparency */
}

void end(FBINFO *data) {
        munmap(data->fbp, data->screensize);
        close(data->fbfd);
		free(data);
}

void test() {
	FBINFO *data = NULL;
	int i = 0;
	RGBT red;
	red.r = 255;

	printf("init...\n");
	data = init();

	printf("writing some pixels...\n");
	for(i = 0; i < 200; i++) {
		draw(data, 20 + i, 20, &red);
	}

	printf("end...\n");
	end(data);
}


void drawCircle(FBINFO *data, int x, int y, int r, RGBT *colour) {
	int i, j;
	int rSqr = r*r;

	/* a*a + b*b = c*c */

	/* pixels in the square contained by the circle */
	for(i = -r; i < r; i++) {
		for(j = -r; j < r; j++) {
			if(i*i + j*j < rSqr) {
				draw(data, x+i, y+j, colour);
			}
		}
	}
}

int main() {
	FBINFO *data = NULL;
	RGBT green;
	RGBT blue;
	RGBT randColour;
	int i;
	int screensaver = 1;

	srand(time(NULL));

	green.g = 255;
	blue.b = 255;

	data = init();
	/*
	drawCircle(data, 20, 30, 5, &green);
	drawCircle(data, 600, 500, 50, &green);
	drawCircle(data, 650, 420, 60, &blue);
	drawCircle(data, 500, 25, 50, &green); */

	while(screensaver) {
		sleep(1);
	/* draw some random circles */
	for(i = 0; i < rand()%20; i++) {
		randColour.r = rand()%255;
		randColour.g = rand()%255;
		randColour.b = rand()%255;
		randColour.t = rand()%255;
		drawCircle(data, rand()%data->vinfo.xres, rand()%data->vinfo.yres, rand()%(data->vinfo.xres/5), &randColour);
	}
	}

	end(data);
	
	return 0;
}
