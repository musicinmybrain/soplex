/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1997-1999 Roland Wunderling                              */
/*                  1997-2001 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: spxhybridpr.h,v 1.4 2001/12/26 12:49:42 bzfkocht Exp $"

/**@file  spxhybridpr.h
 * @brief Hybrid pricer.
 */
#ifndef _SPXHYBRIDPR_H_
#define _SPXHYBRIDPR_H_

#include <assert.h>

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
   representation. Currently it does so by switching between #SPxSteepPR,
   #SPxDevexPR and #SPxParMultPR.

   See #SPxPricer for a class documentation.
*/
class SPxHybridPR : public SPxPricer
{
   SPxSteepPR steep;
   SPxParMultPR parmult;
   SPxDevexPR devex;

   SPxPricer* thepricer;
   SoPlex* thesolver;
   double theeps;

   double hybridFactor; 

public:
   ///
   SoPlex* solver() const
   {
      return thesolver;
   }

   ///
   double epsilon() const
   {
      return theeps;
   }
   ///
   void setEpsilon(double eps);

   ///
   void load(SoPlex* solver);

   ///
   void clear();

   ///
   void setType(SoPlex::Type tp);
   ///
   void setRep(SoPlex::Representation rep);

   ///
   int selectLeave();
   ///
   void left4(int n, SoPlex::Id id);

   ///
   SoPlex::Id selectEnter();
   ///
   void entered4(SoPlex::Id id, int n);

   ///
   void addedVecs (int) 
   {}
   ///
   void addedCoVecs(int)
   {}
   ///
   void removedVec(int)
   {}
   ///
   void removedVecs(const int*)
   {}
   ///
   void removedCoVec(int)
   {}
   ///
   void removedCoVecs(const int*)
   {}
   ///
   int isConsistent() const;

   ///
   SPxHybridPR()
   {
      thesolver = 0;
      /**@todo TK20011102 I have no idea what is a reasonable value here.
       */
      hybridFactor = 0.5;
   }
   virtual ~SPxHybridPR()
   {}

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
