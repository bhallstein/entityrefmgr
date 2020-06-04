#include <cstdio>
#include <tuple>
#include <vector>

// - Declare a template struct SameType<T1, T2> with bool static member value = false
// - Create a specialization for SameType<T,T> with value = true
// - A class can derive from this like so to know whether its template params are the same:

template<class T1, class T2>
struct SameType {
	static const bool value = false;
};

template<class T>
struct SameType<T, T> {
	static const bool value = true;
};


// We might create a helper to check if the passed type is the same as 
// the value type as a vector
//    
//    typedef std::vector<float> fl_vec;
//    fl_vec vec;
// 
//    template<class U>
//    struct Det_Vector : SameType<U, fl_vec> { };

// Or if using a tuple of vectors of different types:
//    
//    typedef std::tuple<A, B> vecs;
//    vecs v;
//
//    template<int N, class U>
//    struct Det_Tuple : SameType<U, std::tuple_element<N, vecs>::type::value_type> { };
//
// This we can use to determine if a given tuple member has the given element value type
// at index N:
//    Det_Tuple<0, float>::value; 

typedef std::tuple<int, float> tup;
tup v;
template<int N, class U>
struct Det_Tuple : SameType<U, typename std::tuple_element<N, tup>::type> { };

typedef std::tuple<std::vector<int>, std::vector<float>> vecs;
vecs w;
template<int N, class U>
struct Det_Tuple_Of_Vecs : SameType<U, typename std::tuple_element<N, vecs>::type::value_type> { };


// Suppose we then want to get a vector *by its type*.
// We can define a function to fetch the next item in the tuple:
template<int N, class T, bool Match>
struct Tuple_Iterate {
	static std::vector<T>& get() {
		return Tuple_Iterate<N+1, T, Det_Tuple_Of_Vecs<N+1, T>::value>::get();
	}
	// Each function passes whether or not the targeted item in the tuple matches the requested type:
	//     Tuple_Iterate<..., Det_Tuple_Of_Vecs<N+1, T>::value>
};

// We can therefore create a specialization where the value of Match is true, which simply
// returns the desired array, as appropriate when ::value is true:
template<int N, class T>
struct Tuple_Iterate<N, T, true> {
	static std::vector<T>& get() {
		return std::get<N>(w);
	}
};

// A helper function initiates the iteration:
template<class T>
std::vector<T>& get() {
	return Tuple_Iterate<0, T, Det_Tuple_Of_Vecs<0, T>::value>::get();
}





// Example with Components
struct UnitPosition {
	float x, y;
};
struct UnitVelocity {
	float a;
};


template<class T>
struct VerySparseArray {

};

template<class... Ts>
struct EntityRefMgr {
	// Each component has an array
	typedef std::tuple<std::vector<Ts>...> vecs;   // std::vector<Ts>... is an expansion of the parameter pack 'class... Ts'
	vecs vvv;                                      // expanding to std::vector<E1>, std::vector<E2>, ...
	
	// ...and an entry in the ComponentIndexSet
	typedef int ComponentIndexSet[sizeof...(Ts)];
	VerySparseArray<ComponentIndexSet> entity_component_indices;
		// Will need to find the entry here using a similar method as below
	
	template<class T>
	std::vector<T>& get_array() {
		return Tuple_Iterate<0, T, Det_Tuple_Of_Vecs<0, T>::value>::get(vvv);
	}


	template<int N, class U>
	struct Det_Tuple_Of_Vecs : SameType<U, typename std::tuple_element<N, vecs>::type::value_type> { };

	template<int N, class T, bool Match>
	struct Tuple_Iterate {
		static std::vector<T>& get(vecs &w) {
			return Tuple_Iterate<N+1, T, Det_Tuple_Of_Vecs<N+1, T>::value>::get(w);
		}
	};
	template<int N, class T>
	struct Tuple_Iterate<N, T, true> {
		static std::vector<T>& get(vecs &w) {
			return std::get<N>(w);
		}
	};
};





void yn(int a) {
	printf("%s\n", a ? "yes" : "no");
}

int main() {
	EntityRefMgr<UnitPosition, UnitVelocity> e;
	auto &positions = e.get_array<UnitPosition>();
	positions.push_back({7.0, 1.0});
	
	
	struct X : SameType<int, float> { };
	struct Y : SameType<int, int> { };
	yn(X::value);
	yn(Y::value);
	
	yn(Det_Tuple<0, int>::value);
	yn(Det_Tuple<0, float>::value);
	yn(Det_Tuple<1, float>::value);
	
	yn(Det_Tuple_Of_Vecs<0, int>::value);
	yn(Det_Tuple_Of_Vecs<0, float>::value);

	{
		auto &vec = std::get<0>(w);
		vec.push_back(7);		
	}
	auto &vec = get<int>();
	printf("%lu\n", vec.size());
	printf("%d\n", vec[0]);
	
	
	
	return 0;
}


