//
// Created by lhirwashema on 2022-06-01.
//

#pragma once

#include "TBPdata.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>


class TBPProblem{
public:
    TemporalBPData _data;

    TBPProblem(TemporalBPData & ItemData){
        _data = ItemData;
    }

};