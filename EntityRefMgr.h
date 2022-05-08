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
struct EntityRefMgr {

	static const size_t NotFound = -1;
	EntityRefMgr() : entities(1) { }


	// entity
	// -----------------------------------
	// - an entity is just a set of indexes into the component vectors

	struct Entity {
		size_t indices[sizeof...(Ts)];
		size_t& operator[](size_t i) {
			return indices[i];
		}
	};
	SparseArray<Entity> entities;


	size_t add_entity() {
		Entity entity;
		for (int i=0, n = sizeof...(Ts); i < n; ++i) {
			entity[i] = NotFound;
		}
		return entities.push(entity);
	}


	void remove_entity(size_t e) {
		entities.removeEntry(e);
	}



	// component vectors
	// -----------------------------------
	// - declare a component vector for each type

	typedef std::tuple<std::vector<Ts>...> ComponentVectors;
	ComponentVectors vvv;

	template<class C>
	std::vector<C>& get_component_vector() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Component_Vectors<0, C>::value>::get(vvv);
	}

	template<class C>
	size_t index_of_component_vector() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Component_Vectors<0, C>::value>::get_index(vvv);
	}


	// get & set components
	// -----------------------------------

	template<class C>
	C& get_component(size_t e) {
		const size_t i = get_component_index<C>(e);
		return get_component_vector<C>()[i];
	}

	template <class C>
	size_t push_component(const C c, size_t e) {
		auto &arr = get_component_vector<C>();
		arr.push_back(c);
		size_t i = arr.size() - 1;
		set_component_index<C>(e, i);
		return i;
	}

	template <class C>
	void remove_component(size_t e) {
		auto &v = get_component_vector<C>();
		size_t i = get_component_index<C>(e);
		size_t i__swapped = swap_remove<C>(v, i);

		set_component_index<C>(e, NotFound);
		if (i__swapped != i) {
			set_component_index<C>(v[i__swapped].entity, i);
		}
	}

	template <class C>
	void remove_components(std::vector<size_t> entities) {
		for (size_t e : entities) {
			remove_component<C>(e);
		}
	}

	// generally only used internally by EntityRefMgr
	template<class C>
	size_t get_component_index(size_t e) {
		auto component_index = index_of_component_vector<C>();
		return entities[e][component_index];
	}

	// generally only used internally by EntityRefMgr
	template<class C>
	void set_component_index(size_t e, size_t i) {
		auto component_index = index_of_component_vector<C>();
		entities[e][component_index] = i;
	}


	// Resolve array for given type at compile time
	// -----------------------------------

	template<size_t N, class U>
	struct Det_Tuple_Of_Component_Vectors : SameType<U, typename std::tuple_element<N, ComponentVectors>::type::value_type> { };

	template<size_t N, class C, bool Match>
	struct Tuple_Iterate {
		static std::vector<C>& get(ComponentVectors &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Component_Vectors<N+1, C>::value>::get(w);
		}
		static size_t get_index(ComponentVectors &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Component_Vectors<N+1, C>::value>::get_index(w);
		}
	};
	template<size_t N, class C>
	struct Tuple_Iterate<N, C, true> {
		static std::vector<C>& get(ComponentVectors &w) {
			return std::get<N>(w);
		}
		static size_t get_index(ComponentVectors &w) {
			return N;
		}
	};
};

#endif


