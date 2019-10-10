
#include <cstdio>
#include <deque>
#include <opencv4/opencv2/opencv.hpp>
#include <kmeans.hpp>

using namespace std;
using cv::imread;
using cv::imwrite;
using cv::IMREAD_COLOR;
using cv::Vec3b;
using cv::Mat;

void help()
{
	printf("images SRC_FNAME DST_FNAME CLASS_COUNT THREAD_NUM\n");
}

int main(const int argc, const char *argv[])
{
	if(argc != 5)
	{
		help();

		return -1;
	}

	const string src_fname = argv[1];
	const string dst_fname = argv[2];
	const size_t class_count = stoi(argv[3]);
	const unsigned thread_num = stoi(argv[4]);
	auto src = imread(src_fname, IMREAD_COLOR);
	const size_t vector_size = 3;
	const size_t vectors_count = src.rows * src.cols;
	deque<double> data(vectors_count * vector_size);

	for(size_t v = 0, ind = 0; v < src.rows; v++)
		for(size_t u = 0; u < src.cols; u++, ind += 3)
		{
			const Vec3b pixel = src.at<Vec3b>(v, u);

			data[ind] = pixel[0];
			data[ind + 1] = pixel[1];
			data[ind + 2] = pixel[2];
		}

	kmeans::normalize(data, 0, 255);
	kmeans::kmeans_result && result = kmeans::kmeans(data, vectors_count, vector_size, class_count, thread_num);

	auto dst = Mat(src.size(), CV_8UC3);

	for(size_t v = 0, ind = 0; v < src.rows; v++)
		for(size_t u = 0; u < src.cols; u++, ind ++)
		{
			Vec3b pixel;
			const auto & class_ind = result.indexes[ind];
			const size_t center_ind = class_ind * vector_size;

			pixel[0] = (result.centers[center_ind] + 1) * 127.5;
			pixel[1] = (result.centers[center_ind + 1] + 1) * 127.5;
			pixel[2] = (result.centers[center_ind + 2] + 1) * 127.5;

			dst.at<Vec3b>(v, u) = pixel;
		}

	imwrite(dst_fname, dst);

	return 0;
}

