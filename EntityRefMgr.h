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
	};
	SparseArray<Entity> entities;


	size_t add_entity() {
		size_t e = entities.push({});
		for (int i=0, n = sizeof...(Ts); i < n; ++i) {
			entities.v[e].indices[i] = NotFound;
		}
		return e;
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



	// get & set entity's entries in component vectors
	// -----------------------------------

	template<class C>
	C& entity__component(size_t e) {
		const size_t i = entity__component_index<C>(e);
		return get_component_vector<C>()[i];
	}

	template<class C>
	size_t entity__component_index(size_t e) {
		auto component_index = index_of_component_vector<C>();
		return entities.v[e].indices[component_index];
	}

	template<class C>
	void entity__set_component_index(size_t e, size_t i) {
		auto component_index = index_of_component_vector<C>();
		entities.v[e].indices[component_index] = i;
	}



	// Add & remove components
	// -----------------------------------

	template <class C>
	size_t entity__add_component(const C c, size_t e) {
		auto &arr = get_component_vector<C>();
		arr.push_back(c);
		size_t i = arr.size() - 1;
		entity__set_component_index<C>(e, i);
		return i;
	}

	template <class C>
	void entity__remove_component(size_t e) {
		Entity &entity = entities.v[e];
		auto &v = get_component_vector<C>();
		size_t vi = index_of_component_vector<C>();
		size_t i = entity.indices[vi];

		size_t i__swapped = swap_remove<C>(v, i);

		entity__set_component_index<C>(e, NotFound);
		if (i__swapped != i) {
			entity__set_component_index<C>(v[i__swapped].entity, i);
		}
	}

	template <class C, class F>
	void entity__remove_components(std::vector<size_t> entities, F f) {
		for (int i = (int)entities.size() - 1; i >= 0; --i) {
			size_t e = entities[i];
			entity__remove_component<C>(e);
			f(e);
		}
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

