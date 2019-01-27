#include "drawlib.h"

#ifndef PI
#define PI 3.14159f
#endif

void testSpecialCircle() {

	/* pixel with random colour */
	RGBT randColour;
	PIXEL colouredPixel;
	colouredPixel.colour = &randColour;
	int x = 0;
	int y = 0;
	colouredPixel.x = &x;
	colouredPixel.y = &y;


	/* seed the random function with time */
	srand(time(NULL));

	/* random rgb value */
	randColour.r = rand()%255;
	randColour.g = rand()%255;
	randColour.b = rand()%255;
	randColour.t = rand()%255;


	/* setup the framebuffer */
	FBINFO *data = init();


	/* data for the function pointers that define different shapes */
	CIRCLE_DATA circle_data;
	OVAL_DATA oval_data;
	PARABOLA_DATA parabola_data;


	/* circle */
	*colouredPixel.x = 500;
	*colouredPixel.y = 500;
	circle_data.radius = 25;
	circle_data.centre_offset = 25;
	drawSpecialCircle(data, &colouredPixel, 50, &circle_data, &circle);

	/* oval */
	*colouredPixel.x = 550;
	*colouredPixel.y = 500;
	oval_data.radius = 25;
	oval_data.centre_offset = 25;
	oval_data.x_mul = 1;
	oval_data.y_mul = 1.5;
	drawSpecialCircle(data, &colouredPixel, 50, &oval_data, &oval);

	/* square */
	*colouredPixel.x = 600;
	*colouredPixel.y = 500;
	drawSpecialCircle(data, &colouredPixel, 50, NULL, &square);

	/* parabola */
	parabola_data.lge_flag = -1;
	parabola_data.a = 0.04f;
	parabola_data.b = -0.6f;
	parabola_data.c = 15;
	parabola_data.side_length = 50;
	*colouredPixel.x = 650;
	*colouredPixel.y = 500;
	drawSpecialCircle(data, &colouredPixel, 50, &parabola_data, &parabola);


	/* end the framebuffer */
	/* don't want any leaks */
	end(data);
}

void circleScreensaver(int totalseconds, int IsUsingTimeForColour) {

	/* setup the framebuffer */
	FBINFO *data = init();

	/* seed the random function with time */
	srand(time(NULL));

	RGBT randColour;
	PIXEL colouredPixel;
	colouredPixel.colour = &randColour;
	int x = 0;
	int y = 0;
	colouredPixel.x = &x;
	colouredPixel.y = &y;

	struct timeval start_time;
	struct timeval current_time;
	if(totalseconds != -1) {
		gettimeofday(&start_time, NULL);
		gettimeofday(&current_time, NULL);
	}

	while(totalseconds == -1 || (current_time.tv_sec - start_time.tv_sec < totalseconds)) {

		gettimeofday(&current_time, NULL);

		/* draw some random circles */
		for(int i = 0; i < rand()%20; i++) {

			if(IsUsingTimeForColour) {
				/* colour based on time */
				randColour.r = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%10) * 255);
				randColour.g = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%20) * 255);
				randColour.b = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%5) * 255);
				
			} else {
				/* just random colour */
				randColour.r = rand()%255;
				randColour.g = rand()%255;
				randColour.b = rand()%255;
				randColour.t = rand()%255;
			}

			*colouredPixel.x = rand()%data->vinfo.xres;
			*colouredPixel.y = rand()%data->vinfo.yres;

			drawCircle(data, rand()%(data->vinfo.xres/5), &colouredPixel);
		}
	}

	end(data);
	
}

void rotatingSquare(int totalseconds, int IsUsingTimeForColour) {
	/* vars for rotatable square */
	float radians;
	int width;
	int height;

	/* init the framebuffer */
	FBINFO *data = init();

	/* setup the coloured pixel */
	RGBT randColour;
	PIXEL colouredPixel;
	colouredPixel.colour = &randColour;
	int x = 0;
	int y = 0;
	colouredPixel.x = &x;
	colouredPixel.y = &y;


	/* seed the random function with time */
	srand(time(NULL));

	struct timeval start_time;
	struct timeval current_time;
	if(totalseconds != -1) {
		gettimeofday(&start_time, NULL);
		gettimeofday(&current_time, NULL);
	}

	while(totalseconds == -1 || (current_time.tv_sec - start_time.tv_sec < totalseconds)) {
		sleep(0.1);

		gettimeofday(&current_time, NULL);

		if(IsUsingTimeForColour) {
			/* colour based on time */
			randColour.r = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%10) * 255);
			randColour.g = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%20) * 255);
			randColour.b = (int)(sin(PI * (((double)current_time.tv_usec)/1000000) + current_time.tv_sec%5) * 255);
			
		} else {
			/* just random colour */
			randColour.r = rand()%255;
			randColour.g = rand()%255;
			randColour.b = rand()%255;
			randColour.t = rand()%255;
		}


		/*degrees = 60;*/
		radians = sin(PI * ((double)current_time.tv_usec)/(1000000)) * 2 * PI;
		for(height = 0+400; height < 100+400/*rand()%data->vinfo.yres*/; height++) {
			for(width = 0+400; width < 100+400/*rand()%data->vinfo.xres*/; width++) {

				rotate_about_point(&colouredPixel, 450, 450, radians);
				draw(data, &colouredPixel);
			}
		}
	}

	end(data);
}

int main() {
	int timelimit = 3;


	testSpecialCircle();

	sleep(timelimit);

	circleScreensaver(timelimit, 1);

	rotatingSquare(timelimit, 0);

	return 0;
}
