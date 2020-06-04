//
// swap_remove.h
//
// - Swap the final element with a removed element, keeping the vector tightly
//   packed.
//
// - Returns the previous index of the swapped element, so the caller can
//   update existing references to that index.
//
// - i.e. given a = [ 1, 2, 3, 4, 5 ]
//        swapRemove(a, 1) will change the array to: [ 1, 5, 3, 4 ], and return 4.
//

#ifndef __swap_remove_h
#define __swap_remove_h

#include <vector>

template<class T>
size_t swap_remove(std::vector<T> &v, size_t i) {
	T last = v.back();
	v.pop_back();
	size_t n = v.size();

	if (n > 0) {
		v[i] = last;
	}

	return n;
}

#endif

