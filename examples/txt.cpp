
#include <fstream>
#include <deque>
#include <vector>
#include <kmeans.hpp>

using namespace std;

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
	size_t vector_size, vectors_count;
	double value;
	deque<double> data;
	ifstream src;
	ofstream dst;

	src.open(src_fname);
	src >> vectors_count >> vector_size;

	const unsigned vectors_count_vector_size = vectors_count * vector_size;

	for(size_t ind = 0; ind < vectors_count_vector_size; ind++)
	{
		src >> value;

		data.push_back(value);
	}

	kmeans::kmeans_result && result = kmeans::kmeans(data, vectors_count, vector_size, class_count, thread_num);

	dst.open(dst_fname);
	dst << vectors_count << " " << class_count << " " << vector_size << "\n";

	dst << "Centers of classes\n";

	for(size_t ind = 0, raw_ind = 0; ind < class_count; ind++, raw_ind += vector_size)
	{
		for(size_t shift = 0; shift < vector_size; shift++)
			dst << result.centers[raw_ind + shift] << " ";

		dst << "\n";
	}

	dst << "Vectors with class indexes\n";

	for(size_t ind = 0, raw_ind = 0; ind < vectors_count; ind++, raw_ind += vector_size)
	{
		for(size_t shift = 0; shift < vector_size; shift++)
			dst << data[raw_ind + shift] << " ";

		dst << result.indexes[ind] << "\n";
	}

	return 0;
}

