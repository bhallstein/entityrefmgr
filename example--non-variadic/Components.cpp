//
//  ComponentStuff.cpp
//  ComponentTest
//
//  Created by Ben on 21/01/2015.
//  Copyright (c) 2015 Ben. All rights reserved.
//

#include "Components.h"

#define COMPONENT_ARRAY_INITIAL_SIZE 2

EntityRefMgr refMgr;

auto c_unitPositions = TightlyPackedArray<Comp_UnitPosition>(COMPONENT_ARRAY_INITIAL_SIZE);
auto c_unitRotating  = TightlyPackedArray<Comp_UnitRotating>(COMPONENT_ARRAY_INITIAL_SIZE);
auto c_unitTraveling = TightlyPackedArray<Comp_UnitTraveling>(COMPONENT_ARRAY_INITIAL_SIZE);
auto c_unitTrees     = TightlyPackedArray<Comp_UnitTree>(COMPONENT_ARRAY_INITIAL_SIZE);
auto c_unitJumping   = TightlyPackedArray<Comp_UnitJumping>(COMPONENT_ARRAY_INITIAL_SIZE);

