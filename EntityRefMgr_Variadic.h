#ifndef __EntityRefMgr_Variadic_h
#define __EntityRefMgr_Variadic_h

#include <tuple>
#include "SparseArray.h"
#include "StretchyArray.h"
#include "StretchyArray_swapRemove.h"
#include <type_traits>

#include <cstdio>


template<class T1, class T2>
struct SameType {
	static const bool value = false;
};

template<class T>
struct SameType<T, T> {
	static const bool value = true;
};

#define ENTITY_DOES_NOT_HAVE_THIS_COMPONENT -1

//template<class... Ts> class EntityRefMgr { };


template<class... Ts>
class EntityRefMgr {
public:
	EntityRefMgr() : entity_component_indices(1) { }
	
	// Adding & removing entities
	int addEntity() {
		// An entity is just an entry in the sparse array of ComponentIndexSets
		int e = entity_component_indices.add_entry({});
		
		for (int i=0, n = sizeof...(Ts); i < n; ++i) {
			entity_component_indices.unsafe_ptr()[e].indices[i] = ENTITY_DOES_NOT_HAVE_THIS_COMPONENT;
		}
		return e;
	}
	void removeEntity(int e) {
		entity_component_indices.removeEntry(e);
	}
	
	
	// Adding & removing components for entities
	template <class C>
	int addComponent(const C &c, int e) {
		auto &arr = get_array<C>();
		arr.push(c);
		int i = arr.get_n() - 1;
		setComponentIndexForEntity<C>(e, i);
		return i;
	}
	template <class C>
	void removeComponent(int comp_index, int e) {
		auto &arr = get_array<C>();
		int ind_swapped = StretchyArray_swapRemove<C, uint16_t>(arr, comp_index);
		setComponentIndexForEntity<C>(e, ENTITY_DOES_NOT_HAVE_THIS_COMPONENT);
		if (ind_swapped != comp_index) {
			setComponentIndexForEntity<C>(arr[comp_index].entity, comp_index);
		}
	}
	
	// Get the array for the given type
	template<class C>
	StretchyArray<C, uint16_t>& get_array() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Vecs<0, C>::value>::get(vvv);
	}
	// Get the index for the given type
	template<class C>
	int get_index() {
		return Tuple_Iterate<0, C, Det_Tuple_Of_Vecs<0, C>::value>::get_index(vvv);
	}
	
	
	// Helpers: Getting & setting component indices
	template<class C>
	int componentIndexForEntity(int e) {
		auto component_index = get_index<C>();
		return entity_component_indices.unsafe_ptr()[e].indices[component_index];
	}
	template<class C>
	void setComponentIndexForEntity(int e, int i) {
		auto component_index = get_index<C>();
		auto &indices_for_entity = entity_component_indices.unsafe_ptr()[e];
		indices_for_entity.indices[component_index] = i;
	}

private:
	// Each component has an array
	typedef std::tuple<StretchyArray<Ts, uint16_t>...> vecs;
	vecs vvv;
	
	// ...and a space within ComponentIndexSet
	struct ComponentIndexSet {
		int indices[sizeof...(Ts)];
	};
	SparseArray<ComponentIndexSet> entity_component_indices;


	// Use the template system to resolve the array for a given type
	// at compile time
	template<int N, class U>
	struct Det_Tuple_Of_Vecs : SameType<U, typename std::tuple_element<N, vecs>::type::value_type> { };

	template<int N, class C, bool Match>
	struct Tuple_Iterate {
		static StretchyArray<C, uint16_t>& get(vecs &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Vecs<N+1, C>::value>::get(w);
		}
		static int get_index(vecs &w) {
			return Tuple_Iterate<N+1, C, Det_Tuple_Of_Vecs<N+1, C>::value>::get_index(w);
		}
	};
	template<int N, class C>
	struct Tuple_Iterate<N, C, true> {
		static StretchyArray<C, uint16_t>& get(vecs &w) {
			return std::get<N>(w);
		}
		static int get_index(vecs &w) {
			return N;
		}
	};
};

#endif
