
#include <fstream>
#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using cv::imread;
using cv::IMREAD_COLOR;
using cv::Vec3b;
using cv::Vec3d;
using cv::Mat;

void help()
{
	printf("image_to_simple_file SRC_FNAME DST_FNAME\n");
}

int main(const int argc, const char *argv[])
{
	if(argc != 3)
	{
		help();

		return -1;
	}

	const string src_fname = argv[1];
	const string dst_fname = argv[2];
	const double a = 2 / 255.;
	const double b = 1 - a * 255;
	auto src = imread(src_fname, IMREAD_COLOR);
	ofstream dst;

	dst.open(dst_fname);

	dst << (unsigned) (src.rows * src.cols) << " " << 3 << "\n";

	for(size_t v = 0, ind = 0; v < src.rows; v++)
		for(size_t u = 0; u < src.cols; u++, ind += 3)
		{
			const Vec3b pixel = src.at<Vec3b>(v, u);
			Vec3d norm_pixel;

			for(size_t ind = 0; ind < 3; ind++)
				norm_pixel[ind] = a * pixel[ind] + b;

			dst << norm_pixel[0] << " " << norm_pixel[1] << " " << norm_pixel[2] << "\n";
		}

	dst.close();

	return 0;
}

