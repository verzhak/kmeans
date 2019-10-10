
#include <deque>

#include <kmeans.hpp>

using namespace std;

namespace kmeans
{

void normalize(deque<double> & data, const double min, const double max)
{
	const double a = 2 / (max - min); // За деление на 0 не стоит волноваться, будет выброшен exception в любом случае
	const double b = 1 - a * max;

	for(auto & elem : data)
		elem = a * elem + b;
}

}

