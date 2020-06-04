// ComponentType.h
// Central list of component types

#ifndef __Components_h
#define __Components_h

#include "CoordinateTypes.h"
#include "TightlyPackedArray.h"

// Component type enum
namespace Components {
	enum Type {
		UnitPosition,
		UnitTree,

		UnitRotating,
		UnitTraveling,
		UnitJumping,
		
		Count
	};
}
typedef	int EntityRef;
#include "EntityRefMgr.h"


// Component definitions
struct Comp_UnitPosition {
	v2 position;
	float angle_in_deg;
	float jump_height;
	EntityRef entity;
};

struct Comp_UnitRotating {
	EntityRef entity;
};

struct Comp_UnitTraveling {
	v2 destination;
	EntityRef entity;
};

struct Comp_UnitJumping {
	float time;
	EntityRef entity;
};

namespace Banyan { class TreeInstance; }
struct Comp_UnitTree {
	Banyan::TreeInstance *tree;
};


// Component arrays
extern EntityRefMgr refMgr;
extern TightlyPackedArray<Comp_UnitPosition>  c_unitPositions;
extern TightlyPackedArray<Comp_UnitRotating>  c_unitRotating;
extern TightlyPackedArray<Comp_UnitTraveling> c_unitTraveling;
extern TightlyPackedArray<Comp_UnitJumping>   c_unitJumping;
extern TightlyPackedArray<Comp_UnitTree>      c_unitTrees;


// Create a component:
// - add component to the array
// - add an entry to the ref mgr

template <class T>
int addComponent(T &c, EntityRef e, Components::Type type, TightlyPackedArray<T> &arr) {
	int i = arr.addEntry(c);
	refMgr.setComponentIndexForEntity(e, type, i);
	return i;
}

// Remove a component:
// - remove component C from array
// - update its entry in the ref mgr to ENTITY_DOES_NOT_HAVE_THIS_COMPONENT
// - the last item in the array moves to occupy C's previous position,
//   so update its entry in refMgr

template <class T>
void removeComponent(int index, EntityRef e, Components::Type type, TightlyPackedArray<T> &c_arr) {
	int ind_swapped = c_arr.removeEntry(index);
	refMgr.setComponentIndexForEntity(e, type, ENTITY_DOES_NOT_HAVE_THIS_COMPONENT);
	if (ind_swapped != index)
		refMgr.setComponentIndexForEntity(c_arr.arr[ind_swapped].entity, type, index);
}

#endif
