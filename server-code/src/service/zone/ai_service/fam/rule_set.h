#ifndef RULE_SET_H
#define RULE_SET_H
//
//  rule_set.h
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

// To support Google Test for private members
#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "rule.h"

namespace fuzzy
{
    enum class Implication : char
    {
        MAMDANI,
        LARSEN
    };

    /**
     A RuleSet is a collection of related rules that govern a particular decision,
     e.g. "fan speed" or "shotgun desirability".
     */
    class RuleSet
    {

    private:
        std::string       _name;
        Implication       _implication;
        std::vector<Rule> _rules;

        // A map of each consequent to a µ value (presumably the 'best' µ for that
        // consequent)
        typedef std::unordered_map<const Trapezoid, double, TrapezoidHasher> mu_map;

        /**
         Many rules can share a consequent, which means that any given consequent
         may have been activated more than once.
         But we need to get just a single µ value out -- we only care about the 'best'
         µ. A popular way of doing so is to OR the values together, i.e. keep the
         maximum µ value and discard the others.
         Put another way, keep only the highest µ for any given consequent.
         @param inputValues A vector of inputs (each a double)
         */
        void populate_mu_map(const std::vector<double>& inputValues, mu_map& _consequents_to_mus) const;

        /**
         Using the consequents-to-µ map, we:
         1) Scale each consequent by its µ value. This is called "implication," and
         this 'weights' the consequents properly. There are several common ways of
         scaling, such as Larsen (scaling) and Mamdani (clipping).

         2) "Defuzzify" into a crisp value by summing the consequents' contributions
         to the output in some sensibly weighted fashion.

         PGAIBE uses the average of the two shoulder points as the 'representative value',
         but other literature indicates that the actual, proper X centroid of the shape
         is a proper 'representative value.' So this routine does that.

         The defuzzification method scales the representative value of each consequent
         by its confidence (i.e. µ) and takes the average, like so:
         (∑representativevalue * height) / (∑ height) for all output sets
         @return the crisp value of this ruleset
         */
        double scale_and_defuzzify(const mu_map& _consequents_to_mus) const;

    public:
        RuleSet();                                                 // default constructor
        RuleSet(const std::string& name, Implication implication); // constructor

        /**
         Retrieve the ruleset's natural-language name
         @return the name in string form
         */
        const std::string& name() const { return _name; }

        /**
         Add an existing rule to the ruleset.
         @param r an rvalue-reference to a Rule
         */
        void add(Rule&& r) { _rules.push_back(std::move(r)); }

        void merge(const RuleSet& rht)
        {
            _rules.reserve(_rules.size() + rht._rules.size());
            _rules.insert(_rules.end(), rht._rules.begin(), rht._rules.end());
        }
        /**
         Given a collection of inputs, calculate the ruleset's result. This means,
         fire all the rules to calculate each rule's µ ; choosing the highest µ
         for every consequent ; weighting the consequents (implication) by their µ
         values ; finally, adding up the weighted consequents to achieve a final
         calculation.
         @param values A vector of inputs (each a double)
         @return the result in double form
         */
        double calculate(const std::vector<double>& values) const;

        /**
         Convenience function to make it easier to call single-input RuleSets.
         (A single-input RuleSet is one whose Rules each have just one antecedent.)
         @param value the single input value to use
         @return the result in double form
         */
        double calculate(const double value) const;

        TEST_FRIENDS
    };
} // namespace fuzzy

#endif /* RULE_SET_H */
