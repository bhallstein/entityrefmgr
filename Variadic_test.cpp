#include "EntityRefMgr_Variadic.h"
#include <cstdio>

struct UnitPosition {
	float x, y;
	int entity;
};
struct UnitVelocity {
	float a;
};


int main() {
	EntityRefMgr<UnitPosition, UnitVelocity> mgr;
	// auto &positions = e.get_array<UnitPosition>();
	// int i = positions.addEntry();
	// positions[i] = {7.0, 1.0};
	
	int e1 = mgr.addEntity();
	int e2 = mgr.addEntity();
	
	printf("e1: %d\ne2: %d\n", e1, e2);
	int poscompind_for_entity_1 = mgr.componentIndexForEntity<UnitPosition>(e1);
	int poscompind_for_entity_2 = mgr.componentIndexForEntity<UnitPosition>(e2);
	printf("poscompind_for_entity_1: %d\n", poscompind_for_entity_1);
	printf("poscompind_for_entity_2: %d\n", poscompind_for_entity_2);
	
	
	poscompind_for_entity_1 = mgr.addComponent<UnitPosition>({ 2.0, 4.0, e1 }, e1);
	poscompind_for_entity_2 = mgr.addComponent<UnitPosition>({ 6.0, 8.0, e2 }, e2);
	printf("poscompind_for_entity_1: %d / %d\n", poscompind_for_entity_1, mgr.componentIndexForEntity<UnitPosition>(e1));
	printf("poscompind_for_entity_2: %d / %d\n", poscompind_for_entity_2, mgr.componentIndexForEntity<UnitPosition>(e2));
	
	mgr.removeComponent<UnitPosition>(poscompind_for_entity_1, e1);
	printf("poscompind_for_entity_1: %d / %d\n", poscompind_for_entity_1, mgr.componentIndexForEntity<UnitPosition>(e1));
	printf("poscompind_for_entity_2: %d / %d\n", poscompind_for_entity_2, mgr.componentIndexForEntity<UnitPosition>(e2));
	
	
	return 0;
}
