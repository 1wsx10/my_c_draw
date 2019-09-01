#include "drawlib.h"



FBINFO* init() {
		FBINFO *data = malloc(sizeof(*data));
		int fbfd = 0;
		struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
		char *fbp;
        long int screensize = 0;
		int error;

        /* Open the file for reading and writing */
        fbfd = open("/dev/fb0", O_RDWR);
        if (fbfd == -1) {
                fprintf(stderr, "%s:%d: Error: cannot open "
                        "framebuffer device: %s\n",
                        __FILE__,
                        __LINE__-3,
                        strerror(errno));
                exit(1);
        }
        printf("The framebuffer device was opened successfully.\n");

		/* Get fixed screen information */
        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
                fprintf(stderr, "Error reading fixed information: %s\n",
                        strerror(errno));
                exit(2);
        }

        /* Get variable screen information */
        if ((error = ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))) {
                printf("Error reading variable information.\nerror code: %d\n", error);
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

				// make it volatile so that it doesn't get optimised out
        ((char volatile *)fbp)[location + 0] = pixel->colour->b; /* blue */
        ((char volatile *)fbp)[location + 1] = pixel->colour->g; /* green */
        ((char volatile *)fbp)[location + 2] = pixel->colour->r; /* red */
        //((char volatile *)fbp)[location + 3] = pixel->colour->t; /* transparency */
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

int oval(const void *x_distance, const void *y_distance, const void *data) {
	OVAL_DATA od = *(OVAL_DATA*)data;
	int x = od.centre_offset - *(int*)x_distance;
	int y = od.centre_offset - *(int*)y_distance;
	int r = od.radius;

	return x*x * od.x_mul + y*y * od.y_mul < r*r;
}

int circle(const void *x_distance, const void *y_distance, const void *circle_data) {
	CIRCLE_DATA cd = *(CIRCLE_DATA*)circle_data;
	int x = cd.centre_offset - *(int*)x_distance;
	int y = cd.centre_offset - *(int*)y_distance;
	int r = cd.radius;

	return x*x + y*y < r*r;
}

int parabola(const void *x_distance, const void *y_distance, const void *parabola_data) {
	PARABOLA_DATA pd = *(PARABOLA_DATA*)parabola_data;
	float x = (float)(*(int*)x_distance);
	int y = pd.side_length - *(int*)y_distance;

	if(pd.lge_flag < 0) {
		return y < (int)(pd.a * x*x + pd.b * x + pd.c);
	} else if(pd.lge_flag > 0) {
		return y > (int)(pd.a * x*x + pd.b * x + pd.c);
	} else {
		return y == (int)(pd.a * x*x + pd.b * x + pd.c);
	}
}

int square(const void *x, const void *y, const void *nothing) {
	/*x = x;
	y = y;
	nothing = nothing;*/
	return 1;
}

void drawSpecialCircle(FBINFO *data, PIXEL *pixel, int side_len, const void *comp_data, int (*comparison)(const void *x, const void *y, const void *data)) {
	int i, j;
	PIXEL tempPix;
	int t_x;
	int t_y;

	tempPix.colour = pixel->colour;
	tempPix.x = &t_x;
	tempPix.y = &t_y;

	/* a*a + b*b = c*c */

	/* pixels in the square contained by the circle */
	for(j = 0; j < side_len; j++) {
		for(i = 0; i < side_len; i++) {
			if(comparison(&i, &j, comp_data)) {
				*tempPix.x = *pixel->x + i;
				*tempPix.y = *pixel->y + j;
				draw(data, &tempPix);
			}
		}
	}
}

void drawCircle(FBINFO *data, int r, PIXEL *pixel) {
	int i, j;
	int rSqr = r*r;
	PIXEL tempPix;
	int t_x;
	int t_y;

	tempPix.colour = pixel->colour;
	tempPix.x = &t_x;
	tempPix.y = &t_y;

	/* a*a + b*b = c*c */

	/* pixels in the square contained by the circle */
	for(j = -r; j < r; j++) {
		for(i = -r; i < r; i++) {
			if(i*i + j*j/* (rand()%200+1)*/ < rSqr) {
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
	int temp_x;
	int temp_y;

	*pixel->x -= px;
	*pixel->y -= py;

	temp_x = *pixel->x;
	temp_y = *pixel->y;
	
	*pixel->x = (int)(temp_x * cos(angle) - temp_y * sin(angle));
	*pixel->y = (int)(temp_x * sin(angle) + temp_y * cos(angle));

	*pixel->x += px;
	*pixel->y += py;
}
