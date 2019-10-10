
#ifndef KMEANS_HPP
#define KMEANS_HPP

#include <deque>
#include <vector>

#define DEBUG printf("%s %u\n", __FILE__, __LINE__);

namespace kmeans
{

using std::vector;
using std::deque;
using std::size_t;

struct kmeans_result
{
	vector<size_t> indexes;
	vector<double> centers;
};

kmeans_result kmeans(
	const deque<double> & data,
	const size_t vectors_count,
	const size_t vector_size,
	const size_t class_count,
	const unsigned thread_num,
	const unsigned max_iterate = 0);

void normalize(deque<double> & data, const double min, const double max);

}

#endif

