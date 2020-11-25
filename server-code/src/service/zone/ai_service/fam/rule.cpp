//
//  rule.cpp
//  fam
//
//  Created by Chris Powell on 8/19/13.
//  Copyright (c) 2013 Prylis Inc. All rights reserved.
//
//  This file is part of The C++ Fuzzy Associative Memory
//  http://github.com/cpowell/fuzzy-associative-memory-cplusplus
//  You can redistribute and/or modify this software only in accordance with
//  the terms found in the "LICENSE" file included with the library.
//

#include "rule.h"

#include <algorithm>

//#include <iostream>

fuzzy::Rule::Rule(const std::vector<Trapezoid>& antecedents,
                  const Conjunction&            conjunction,
                  const Trapezoid&              consequent,
                  const std::string&            naturalLanguage)
    : _antecedents(antecedents)
    , _conjunction(conjunction)
    , _consequent(consequent)
    , _naturalLanguage(naturalLanguage)
{
    // nop
}

double fuzzy::Rule::fire(const std::vector<double>& values) const
{
    // Holds the calculated mu values for each antecedent upon firing
    std::vector<double> _mus;

    if(_antecedents.size() != values.size())
    {
        throw std::out_of_range("The size of the input vector to Rule::fire() must equal the size of its antecedent vector.");
    }

    for(size_t i = 0; i < _antecedents.size(); ++i)
    {
        _mus.push_back(_antecedents[i].calculateMu(values[i]));
    }

    if(_conjunction == Conjunction::AND) // AND == intersection == minimum
    {
        return *std::min_element(begin(_mus), end(_mus));
    }
    else // OR == union == maximum
    {
        return *std::max_element(begin(_mus), end(_mus));
    }

    //    std::cout << "Fired rule '" << _naturalLanguage << "' with inputs [";
    //    for (const auto &v: values) {
    //        std::cout << v << ", ";
    //    }
    //    std::cout << "], µ choices are [";
    //    for (const auto &m: _mus) {
    //        std::cout << m << ", ";
    //    }
    //    std::cout << "], final µ for this rule is " << mu << std::endl;

    //    return mu;
}
