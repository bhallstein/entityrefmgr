#include "../EntityRefMgr.h"
#include <cstdio>
#include "_test.h"


struct UnitPosition {
  float x, y;
  size_t entity;
};


struct UnitVelocity {
  float a;
};


typedef EntityRefMgr<UnitPosition, UnitVelocity> UnitMgr;


int main() {
  UnitMgr mgr;

  p_file_header("test.cpp");


  p_header("Entity IDs");
  size_t e1 = mgr.add_entity();
  size_t e2 = mgr.add_entity();
  p_assert(e1 == 0);
  p_assert(e2 == 1);


  p_header("Uninitialized components are NotFound");
  size_t i_unitpos_1 = mgr.entity__component_index<UnitPosition>(e1);
  size_t i_unitpos_2 = mgr.entity__component_index<UnitPosition>(e2);
  p_assert(i_unitpos_1 == UnitMgr::NotFound);


  p_header("Initialized component IDs");
  i_unitpos_1 = mgr.entity__add_component<UnitPosition>({ 2.0, 4.0, e1 }, e1);
  i_unitpos_2 = mgr.entity__add_component<UnitPosition>({ 6.0, 8.0, e2 }, e2);
  p_assert(i_unitpos_1 == 0);
  p_assert(i_unitpos_2 == 1);


  p_header("Removing components from entity");
  mgr.entity__remove_component<UnitPosition>(e1);
  size_t i_unitpos_1__after_remove = mgr.entity__component_index<UnitPosition>(e1);
  p_assert(i_unitpos_1__after_remove == UnitMgr::NotFound);


  p_header("Component arrays are kept tightly packed");
  size_t i_unitpos_2__after_remove = mgr.entity__component_index<UnitPosition>(e2);
  p_assert(i_unitpos_2__after_remove == 0);


  return 0;
}

