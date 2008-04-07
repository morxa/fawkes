
/***************************************************************************
 *  mamdani_fuzzy_controller.cpp - Mamdani Fuzzy Controller
 *
 *  Generated: Thu May 31 18:36:55 2007
 *  Copyright  2007  Martin Liebenberg
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include <plugins/navigator/fuzzy/mamdani_fuzzy_controller.h>
#include <plugins/navigator/fuzzy/fuzzy_set.h>
#include <plugins/navigator/fuzzy/fuzzy_partition.h>

/** @class MamdaniFuzzyController fuzzy/mamdani_fuzzy_controller.h
 *  This is an implementation of a Mandani fuzzy controller.
 *  It implements a singleton fuzzification, an aggregation with minimal operator,
 *  a max-min interference and a continuous defuzzification.
 *
 * @author Martin Liebenberg
 */
/** @struct MamdaniFuzzyController::Rule fuzzy/mamdani_fuzzy_controller.h
 *    A rule structure for the rule base of the Mamdani controller.
 * @fn MamdaniFuzzyController::Rule::Rule(std::vector<FuzzySet *> *inputSets, FuzzySet *outputSet)
 *      Constructor.
 * @param inputSets the condition fuzzy sets of this rule
 * @param outputSet the conclusion fuzzy set of this rule
 */
/** @var MamdaniFuzzyController::Rule::inputSets
 *    A vector the condition fuzzy sets of this rule.
 */
/** @var MamdaniFuzzyController::Rule::outputSet
 *    The conclusion fuzzy set of this rule.
 */
/** @var MamdaniFuzzyController::aggregationValues
 *      A vector of double values generated by the aggregation.
 */
/** @var MamdaniFuzzyController::ruleBase
 *      A vector of pointers of rules for the rule base of this fuzzy controller.
 */

/** Empty standard constructor.*/
MamdaniFuzzyController::MamdaniFuzzyController()
{}

/** Constructor.
 * @param inputPartitions a vector of pointers to objects of FuzzyPartition
 * @param outputPartition a pointer to a FuzzyPartition
 */
MamdaniFuzzyController::MamdaniFuzzyController(std::vector<FuzzyPartition *> *inputPartitions,
    FuzzyPartition* outputPartition)
{
  this->inputPartitions = inputPartitions;
  this->outputPartition = outputPartition;
}


/** Destructor. */
MamdaniFuzzyController::~MamdaniFuzzyController()
{
  for(unsigned int i = 0; i < ruleBase.size(); i++)
    delete ruleBase[i];
  ruleBase.clear();
}

/** Adds a rule to the rule base.
 * @param inputSets a vector of pointers to condition fuzzy sets
 * @param outputSet a pointer to the conclusion fuzzy set
 */
void MamdaniFuzzyController::addRule(std::vector<FuzzySet *> *inputSets, FuzzySet * outputSet)
{
  ruleBase.push_back(new Rule(inputSets, outputSet));
}

/** Implements a singleton fuzzification.
 */
void MamdaniFuzzyController::fuzzification(std::vector<double> values)
{
  for(unsigned int i = 0; i < values.size(); i++)
    {
      std::vector<FuzzySet *> *sets = inputPartitions->at(i)->getFuzzySets();
      for(unsigned int j = 0; j < sets->size(); j++)
        {
          sets->at(j)->setAlphaLevel(sets->at(j)->membershipGrade(values[i]));
        }
    }
}

/** Implements aggregation with minimal operator.
 *      It generates for each rule a aggregation value.
 */
void MamdaniFuzzyController::aggregation()
{
  double min = 1;
  for(unsigned int i = 0; i < ruleBase.size(); i++)
    {
      std::vector<FuzzySet *> *sets = ruleBase[i]->inputSets;
      for(unsigned int j = 0; j < sets->size(); j++)
        {
          double level = sets->at(j)->getAlphaLevel();
          min = std::min(level, min);
        }
      aggregationValues.push_back(min);
      min = 1;
    }
}

/** Implements a max-min interference.
 *      It needs the aggregation values from the aggregation.
 */
void MamdaniFuzzyController::interference()
{
  for(unsigned int i = 0; i < ruleBase.size(); i++)
    {
      FuzzySet *set = ruleBase[i]->outputSet;
      set->setAlphaLevel(aggregationValues[i]);
    }
  aggregationValues.clear();
}

/** The accumulation method remains empty.
 */
void MamdaniFuzzyController::accumulation()
{
  //here is no need for accumulation
}

/** Implements a continuous defuzzification.
 *      There is the FuzzyPartition::getWeightedAverageMax() method in use.
 */
double MamdaniFuzzyController::defuzzification()
{
  return outputPartition->getWeightedAverageMax();
}
