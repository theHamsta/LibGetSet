#include <stdio.h>

int main(int argc, char ** argv)
{
	if (argc!=8)
	{
		printf(
			"Usage:\n"
			"   renderFractal file.bmp width_px height_px\n"
			"                 scale num_iterations\n"
			"                 center_read center_imag\n"
			"Computes an approximation to the mandelbrot set and saves an image as bmp.\n");
		return 1;
	}

	// Image properties
    int width_px=atoi(argv[2]), height_px = atoi(argv[3]);
	double scale=atof(argv[4]);
	int num_its=atoi(argv[5]);
	int center_x=atoi(argv[6]), center_y=atoi(argv[7]);

	// Print configuration
	printf("Parameters:\n"
		"file=%s\n"
		"width:%i\n"
		"height:%i\n"
		"scale:%.3f\n"
		"num_iterations:%i\n"
		"center x:%i\n"
		"center y:%i\n"
		,argv[1],width_px, height_px,scale,num_its,center_x,center_y);

	// Check a douple of parameters
	if (width_px%4!=0)
	{
		printf("Image width must be devisible by four. BMP-padding not supported.\n");
		return 1;
	}

	if (width_px<0 || height_px<0)
	{
		printf("Image width and height must be positive\n");
		return 1;
	}

	if (num_its<1 || num_its>100000)
	{
		printf("Please choose a reasonable number of iterations!\n");
		return 1;		
	}

	//
	// INSERT ALGORITHM HERE
	//
	
    // Exit normally
    return 0;
}
