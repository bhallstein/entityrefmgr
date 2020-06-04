#ifndef __EntityRefMgr_Variadic_h
#define __EntityRefMgr_Variadic_h

#include <tuple>
#include <type_traits>
#include <vector>
#include "SparseArray.h"
#include "swap_remove.h"


// SameType template helper
// -----------------------------------

template<class T1, class T2>
struct SameType {
	static const bool value = false;
};

template<class T>
struct SameType<T, T> {
	static const bool value = true;
};


// EntityRefMgr
// -----------------------------------

template<class... Ts>
class EntityRefMgr {
public:
	EntityRefMgr() : entity_component_indices(1) { }

	static const size_t NotFound = -1;


	// Add & remove entities
	// -----------------------------------

	size_t addEntity() {
		size_t e = entity_component_indices.push({});
		for (int i=0, n = sizeof...(Ts); i < n; ++i) {
			entity_component_indices.v[e].indices[i] = NotFound;
		}
		return e;
	}

	void removeEntity(int e) {
		entity_component_indices.removeEntry(e);
	}
	
	
	// Add & remove components
	// -----------------------------------

	template <class C>
	size_t addComponent(const C &c, size_t e) {
		auto &arr = get_component_array<C>();
		arr.push_back(c);
		size_t i = arr.size() - 1;
		setComponentIndexForEntity<C>(e, i);
		return i;
	}

	template <class C>
	void removeComponent(size_t e, size_t i) {
		auto &v = get_component_array<C>();
		size_t i__swapped = swap_remove<C>(v, i);
		setComponentIndexForEntity<C>(e, NotFound);

		if (i__swapped != i) {
			setComponentIndexForEntity<C>(v[i__swapped].entity, i);
		}
	}

	template <class C, class F>
	void removeComponentsMulti(std::vector<size_t> indices, F f) {
		auto &v = get_component_array<C>();

		// Iteratively remove all components from a vector
		// - reverse order, due to TightlyPackedArray constraints
		// - end the behaviour tree for the entity
		for (int j = (int)indices.size()-1; j >= 0; --j) {
			size_t i = indices[j];
			size_t entity = v[i].entity;

			removeComponent<C>(entity, i);
			f(entity);
		}
	}


	// Get a component array, given component type
	// -----------------------------------

	template<class C>
	std::vector<C>& get_component_array() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Vecs<0, C>::value>::get(vvv);
	}


	// Get index of a component array, given component type
	// -----------------------------------

	template<class C>
	size_t get_index() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Vecs<0, C>::value>::get_index(vvv);
	}
	
	
	// Helpers: Getting & setting component indices
	// -----------------------------------

	template<class C>
	size_t componentIndexForEntity(size_t e) {
		auto component_index = get_index<C>();
		return entity_component_indices.v[e].indices[component_index];
	}

	template<class C>
	void setComponentIndexForEntity(size_t e, size_t i) {
		auto component_index = get_index<C>();
		auto &indices_for_entity = entity_component_indices.v[e];
		indices_for_entity.indices[component_index] = i;
	}


private:
	typedef std::tuple<std::vector<Ts>...> vecs;
	vecs vvv;

	struct ComponentIndices {
		size_t indices[sizeof...(Ts)];
	};
	SparseArray<ComponentIndices> entity_component_indices;


	// Resolve array for given type at compile time
	// -----------------------------------

	template<size_t N, class U>
	struct Det_Tuple_Of_Vecs : SameType<U, typename std::tuple_element<N, vecs>::type::value_type> { };

	template<size_t N, class C, bool Match>
	struct Tuple_Iterate {
		static std::vector<C>& get(vecs &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Vecs<N+1, C>::value>::get(w);
		}
		static size_t get_index(vecs &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Vecs<N+1, C>::value>::get_index(w);
		}
	};
	template<size_t N, class C>
	struct Tuple_Iterate<N, C, true> {
		static std::vector<C>& get(vecs &w) {
			return std::get<N>(w);
		}
		static size_t get_index(vecs &w) {
			return N;
		}
	};
};

#endif

