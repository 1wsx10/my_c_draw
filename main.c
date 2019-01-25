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
	float radians;
	int temp_x;
	int temp_y;
	PIXEL randPixel;
	struct timeval time_of_day;
	
	CIRCLE_DATA circle_data;
	OVAL_DATA oval_data;
	PARABOLA_DATA parabola_data;

	randPixel.colour = &randColour;
	randPixel.x = &temp_x;
	randPixel.y = &temp_y;

	srand(time(NULL));

	/*green.g = 255;
	blue.b = 255;*/
	randColour.r = rand()%255;
	randColour.g = rand()%255;
	randColour.b = rand()%255;
	randColour.t = rand()%255;

	test();

	data = init();


#define DRAWTESTa
#ifdef DRAWTEST

	/* circle */
	*randPixel.x = 500;
	*randPixel.y = 500;
	circle_data.radius = 25;
	circle_data.centre_offset = 25;
	drawSpecialCircle(data, &randPixel, 50, &circle_data, &circle);

	/* oval */
	*randPixel.x = 550;
	*randPixel.y = 500;
	oval_data.radius = 25;
	oval_data.centre_offset = 25;
	oval_data.x_mul = 1;
	oval_data.y_mul = 1.5;
	drawSpecialCircle(data, &randPixel, 50, &oval_data, &oval);

	/* square */
	*randPixel.x = 600;
	*randPixel.y = 500;
	drawSpecialCircle(data, &randPixel, 50, NULL, &square);

	/* parabola */
	parabola_data.lge_flag = -1;
	parabola_data.a = 0.04f;
	parabola_data.b = -0.6f;
	parabola_data.c = 15;
	parabola_data.side_length = 50;
	*randPixel.x = 650;
	*randPixel.y = 500;
	drawSpecialCircle(data, &randPixel, 50, &parabola_data, &parabola);

#endif

	while(screensaver && 0) {
		sleep(0.1);

		gettimeofday(&time_of_day, NULL);

#define USETIMEFORRANDOMSQUARE
#ifdef USETIMEFORRANDOMSQUARE
		randColour.r = (int)(sin(PI * (((double)time_of_day.tv_usec)/1000000) + time_of_day.tv_sec%10) * 255);
		randColour.g = (int)(sin(PI * (((double)time_of_day.tv_usec)/1000000) + time_of_day.tv_sec%20) * 255);
		randColour.b = (int)(sin(PI * (((double)time_of_day.tv_usec)/1000000) + time_of_day.tv_sec%5) * 255);
		
#endif
#ifndef USETIMEFORRANDOMSQUARE
		
		randColour.r = rand()%255;
		randColour.g = rand()%255;
		randColour.b = rand()%255;
		randColour.t = rand()%255;
#endif

		/*degrees = 60;*/
		radians = sin(PI * ((double)time_of_day.tv_usec)/(1000000)) * 2 * PI;
		for(height = 0+400; height < 100+400/*rand()%data->vinfo.yres*/; height++) {
			for(width = 0+400; width < 100+400/*rand()%data->vinfo.xres*/; width++) {
				temp_x = width;
				temp_y = height;

				rotate_about_point(&randPixel, 450, 450, radians);
				draw(data, &randPixel);
			}
		}
	}

	while(screensaver && 0) {
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
