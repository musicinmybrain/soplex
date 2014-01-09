/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1996-2013 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file  statistics.h
 * @brief Class for collecting statistical information
 */
#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <iostream>

#include "soplex.h"
#include "timer.h"

namespace soplex
{
   /**@class   Statistics
    * @brief   Class for collecting statistical information
    * @ingroup Algo
    */
   class SoPlex::Statistics
   {
   public:

      //**@name Construction, resetting, printing */
      //@{

      /// default constructor
      Statistics();

      /// clears all statistics
      void clearAllData();

      /// clears statistics on solving process
      void clearSolvingData();

      /// prints statistics
      void print(std::ostream& os);

      //@}


      //**@name Data */
      //@{

      Timer readingTime; ///< reading time not included in solving time
      Timer solvingTime; ///< solving time
      Timer preprocessingTime; ///< preprocessing time
      Timer simplexTime; ///< simplex time
      Timer syncTime; ///< time for synchronization between real and rational LP (included in solving time)
      Timer transformTime; ///< time for transforming LPs (included in solving time)
      Real luFactorizationTime; ///< time for factorizing bases matrices
      Real luSolveTime; ///< time for solving linear systems
      int iterations; ///< number of iterations/pivots
      int iterationsPrimal; ///< number of iterations with Primal
      int iterationsFromBasis; ///< number of iterations from Basis
      int luFactorizations; ///< number of basis matrix factorizations
      int luSolves; ///< number of (forward and backward) solves with basis matrix
      int refinements; ///< number of refinement steps
      int stallRefinements; ///< number of refinement steps without subsequent pivots

      //@}
   };
} // namespace soplex
#endif // _STATISTICS_H_

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------
