// EntityRefMgr
//
// - each entity consists of a set of components
// - the indices of these components within their respective arrays can change
// - for an entity, there is a list of these indices -- a ComponentIndexSet
// - EntityRefMgr manages an array of ComponentIndexSets
//   - since deletes are sparse, indices in it do not change
//   - so, an entity ref is just the index of a ComponentIndexSet in EntityRefMgr
//   - a component is looked up by providing:
//      - the entity ref
//      - the index of the desired component in ComponentIndexSet

#ifndef __EntityRefMgr_h
#define __EntityRefMgr_h

#include "SparseArray.h"

typedef int ComponentIndexSet[Components::Count];

class EntityRefMgr {
public:
	EntityRefMgr() : v(8) {  }

	static const size_t NotFound -1;
	
	EntityRef addEntity() {
		EntityRef e = v.addEntry();
		for (int i=0; i < Components::Count; ++i) {
			v[e][i] = NotFound;
		}
		printf("Added entity, now about %d of them\n", e);
		return e;
	}
	void removeEntity(EntityRef e) {
		v.removeEntry(e);
		// v.printFreeSpace();
	}
	
	int componentIndexForEntity(EntityRef e, Components::Type component) {
		return v[e][component];
	}
	void setComponentIndexForEntity(EntityRef e, Components::Type component, int i) {
		v[e][component] = i;
	}

private:
	SparseArray<ComponentIndexSet> v;

};

#endif
