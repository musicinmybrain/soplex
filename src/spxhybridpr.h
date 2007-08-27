/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1997-1999 Roland Wunderling                              */
/*                  1997-2007 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: spxhybridpr.h,v 1.17 2007/08/27 15:35:11 bzfberth Exp $"

/**@file  spxhybridpr.h
 * @brief Hybrid pricer.
 */
#ifndef _SPXHYBRIDPR_H_
#define _SPXHYBRIDPR_H_

#include <assert.h>

#include "spxdefines.h"
#include "spxpricer.h"
#include "spxdevexpr.h"
#include "spxparmultpr.h"
#include "spxsteeppr.h"

namespace soplex
{

/**@brief   Hybrid pricer.
   @ingroup Algo

   The hybrid pricer for SoPlex tries to guess the best pricing strategy to
   use for pricing the loaded LP with the loaded algorithm type and basis
   representation. Currently it does so by switching between SPxSteepPR,
   SPxDevexPR and SPxParMultPR.

   See SPxPricer for a class documentation.
*/
class SPxHybridPR : public SPxPricer
{
   //-------------------------------------
   /**@name Data */
   //@{
   /// steepest edge pricer
   SPxSteepPR   steep;
   /// partial multiple pricer
   SPxParMultPR parmult;
   /// devex pricer
   SPxDevexPR   devex;
   /// the currently used pricer
   SPxPricer*   thepricer;
   /// factor between dim and coDim of the problem to decide about the pricer
   Real hybridFactor; 
   //@}

public:

   //-------------------------------------
   /**@name Access / modification */
   //@{
   /// sets the epsilon
   virtual void setEpsilon(Real eps);
   /// sets the solver
   virtual void load(SPxSolver* solver);
   /// clears all pricers and unselects the current pricer
   virtual void clear();
   /// sets entering or leaving algorithm
   virtual void setType(SPxSolver::Type tp);
   /// sets row or column representation
   virtual void setRep(SPxSolver::Representation rep);
   /// selects the leaving algorithm
   virtual int selectLeave();
   /// selects the entering algorithm
   virtual SPxId selectEnter();
   /// calls left4 on the current pricer
   virtual void left4(int n, SPxId id);
   /// calls entered4 on the current pricer
   virtual void entered4(SPxId id, int n);
   /// calls addedVecs(n) on all pricers
   virtual void addedVecs (int n);
   /// calls addedCoVecs(n) on all pricers
   virtual void addedCoVecs (int n);
   //@}

   //-------------------------------------
   /**@name Consistency check */
   //@{
#ifndef NO_CONSISTENCY_CHECKS
   /// consistency check
   virtual bool isConsistent() const;
#endif
   //@}

   //-------------------------------------
   /**@name Construction / destruction */
   //@{
   /// default constructor
   SPxHybridPR() 
      : SPxPricer("Hybrid")
      , thepricer(0)
      , hybridFactor(3.0) // we want the ParMult pricer
   {}
   /// destructor
   virtual ~SPxHybridPR()
   {}
   //@}
};

} // namespace soplex
#endif // _SPXHYBRIDPR_H_

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------
