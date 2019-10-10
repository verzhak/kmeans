
#include <deque>
#include <random>
#include <limits>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <mutex>

#include <kmeans.hpp>

using namespace std;

namespace kmeans
{

kmeans_result kmeans(const deque<double> & data, const size_t vectors_count, const size_t vector_size, const size_t class_count, const unsigned thread_num, const unsigned max_iterate)
{
	vector<double> centers(class_count * vector_size);
	vector<size_t> indexes(vectors_count);

	{
		// Инициализация центров классов
		uniform_real_distribution<double> distribution(-1, 1);
		default_random_engine engine;

		generate(centers.begin(), centers.end(), [&]() { return distribution(engine); });
	}

	{
		const unsigned __max_iterate = max_iterate ? max_iterate : numeric_limits<unsigned>::max();
		const unsigned thread_vectors_step = vectors_count / thread_num;
		const unsigned thread_class_step = class_count / thread_num;
		vector<unsigned> classes_vectors_counts(class_count);
		vector<mutex> classes_mutexes(class_count);

		// Обновление классов для каждого из исходных векторов
		auto distribute_vectors_by_classes = [ &indexes, &data, &centers, &vector_size, &class_count ] (const size_t from, const size_t to, promise<unsigned> && changes_count)
		{
			unsigned __changes_count = 0;

			for(size_t ind = from, raw_ind = from * vector_size; ind < to; ind++, raw_ind += vector_size)
			{
				double min_distance = numeric_limits<double>::max();
				size_t min_class_ind;

				for(size_t class_ind = 0, raw_class_ind = 0; class_ind < class_count; class_ind ++, raw_class_ind += vector_size)
				{
					double distance = 0;

					for(size_t elem_ind = 0; elem_ind < vector_size; elem_ind++)
					{
						const double d = data[raw_ind + elem_ind] - centers[raw_class_ind + elem_ind];

						distance += d * d;
					}

					if(distance < min_distance)
					{
						min_distance = distance;
						min_class_ind = class_ind;
					}
				}

				const size_t current_class_ind = indexes[ind];

				if(current_class_ind != min_class_ind)
				{
					indexes[ind] = min_class_ind;
					__changes_count ++;
				}
			}

			changes_count.set_value(__changes_count);
		};

		// Обновление центров классов
		auto renew_class_centers_accumulate = [ &centers, &classes_vectors_counts, &classes_mutexes, &vector_size, &indexes, &data](const size_t from, const size_t to)
		{
			for(size_t ind = from, raw_ind = from * vector_size; ind < to; ind++, raw_ind += vector_size)
			{
				const unsigned class_ind = indexes[ind];
				const unsigned raw_class_ind = class_ind * vector_size;

				lock_guard<mutex> lock(classes_mutexes[class_ind]);
			
				classes_vectors_counts[class_ind] ++;

				for(size_t elem_ind = 0; elem_ind < vector_size; elem_ind++)
					centers[raw_class_ind + elem_ind] += data[raw_ind + elem_ind];
			}
		};

		auto renew_class_centers = [ &centers, &classes_vectors_counts, &vector_size](const size_t from, const size_t to)
		{
			for(size_t class_ind = from, raw_class_ind = from * vector_size; class_ind < to; class_ind++, raw_class_ind += vector_size)
			{
				const unsigned vectors_count = classes_vectors_counts[class_ind];

				for(size_t elem_ind = 0; elem_ind < vector_size; elem_ind++)
					centers[raw_class_ind + elem_ind] /= vectors_count;
			}
		};

		// Обновление классов
		for(unsigned iterate = 0; iterate < __max_iterate; iterate++)
		{
			unsigned changes_count = 0;

			// Обновление индексов классов каждого вектора
			vector<thread> threads;
			vector<promise<unsigned>> promises(thread_num);
			vector<future<unsigned>> futures;
			size_t from, to, thread_ind;

			for(thread_ind = 0, from = 0, to = thread_vectors_step; thread_ind < thread_num; thread_ind++, from += thread_vectors_step, to += thread_vectors_step)
			{
				futures.push_back(promises[thread_ind].get_future());
				threads.push_back(thread(distribute_vectors_by_classes, from, min(to, vectors_count), move(promises[thread_ind])));
			}

			for(thread_ind = 0; thread_ind < thread_num; thread_ind ++)
			{
				changes_count += futures[thread_ind].get();
				threads[thread_ind].join(); // Нужно корректно завершать потоки
			}

			threads.clear();

			printf("Iteration: %u from %u with %u changes\n", iterate + 1, __max_iterate, changes_count);

			// Если ни одного обновления не произошло, то алгоритм сошелся
			if(! changes_count)
				break;

			// Обновление центров классов
			fill(classes_vectors_counts.begin(), classes_vectors_counts.end(), 0);
			fill(centers.begin(), centers.end(), 0);

			for(thread_ind = 0, from = 0, to = thread_vectors_step; thread_ind < thread_num; thread_ind++, from += thread_vectors_step, to += thread_vectors_step)
				threads.push_back(thread(renew_class_centers_accumulate, from, min(to, vectors_count)));

			for(auto & thread : threads)
				thread.join();

			threads.clear();

			for(thread_ind = 0, from = 0, to = thread_class_step; thread_ind < thread_num; thread_ind++, from += thread_class_step, to += thread_class_step)
				threads.push_back(thread(renew_class_centers, from, min(to, class_count)));

			for(auto & thread : threads)
				thread.join();
		}
	}

	// Ждем, что отработает RVO
	return kmeans_result{ .indexes = indexes, .centers = centers };
}

}

