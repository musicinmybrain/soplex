/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1997-1999 Roland Wunderling                              */
/*                  1997-2002 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: spxbasis.cpp,v 1.46 2005/07/14 17:38:36 bzforlow Exp $"

//#define DEBUGGING 1

#include <assert.h>
#include <iostream>
#include <iomanip>

#include "spxdefines.h"
#include "spxbasis.h"
#include "didxset.h"
#include "dvector.h"
#include "spxsolver.h"
#include "mpsinput.h"
#include "message.h"
#include "spxout.h"

namespace soplex
{
SPxBasis::Desc::Status
SPxBasis::dualStatus(const SPxColId& id) const
{
   METHOD( "SPxBasis::dualStatus()" );
   return dualColStatus(static_cast<SPxLP*>(theLP)->number(id));
}

SPxBasis::Desc::Status
SPxBasis::dualStatus(const SPxRowId& id) const
{
   METHOD( "SPxBasis::dualStatus()" );
   return dualRowStatus((static_cast<SPxLP*>(theLP))->number(id));
}

SPxBasis::Desc::Status
SPxBasis::dualRowStatus(int i) const
{
   METHOD( "SPxBasis::dualRowStatus()" );
   assert(theLP != 0);

   if (theLP->rhs(i) < infinity)
   {
      if (theLP->lhs(i) > -infinity)
      {
         if (theLP->lhs(i) == theLP->rhs(i))
            return Desc::D_FREE;
         else
            return Desc::D_ON_BOTH;
      }
      else
         return Desc::D_ON_LOWER;
   }
   else if (theLP->lhs(i) > -infinity)
      return Desc::D_ON_UPPER;
   else
      return Desc::D_UNDEFINED;
}

SPxBasis::Desc::Status
SPxBasis::dualColStatus(int i) const
{
   METHOD( "SPxBasis::dualColStatus()" );
   assert(theLP != 0);

   if (theLP->SPxLP::upper(i) < infinity)
   {
      if (theLP->SPxLP::lower(i) > -infinity)
      {
         if (theLP->SPxLP::lower(i) == theLP->SPxLP::upper(i))
            return Desc::D_FREE;
         else
            return Desc::D_ON_BOTH;
      }
      else
         return Desc::D_ON_LOWER;
   }
   else if (theLP->SPxLP::lower(i) > -infinity)
      return Desc::D_ON_UPPER;
   else
      return Desc::D_UNDEFINED;
}

void SPxBasis::loadMatrixVecs()
{
   METHOD( "SPxBasis::loadMatrixVecs()" );
   assert(theLP != 0);
   assert(theLP->dim() == matrix.size());

   VERBOSE3({ spxout << "IBASIS01 loadMatrixVecs() invalidates factorization" << std::endl; });

   int i;
   nzCount = 0;
   for (i = theLP->dim() - 1; i >= 0; --i)
   {
      matrix[i] = &theLP->vector(baseId(i));
      nzCount += matrix[i]->size();
   }
   matrixIsSetup = true;
   factorized = false;
   if (factor != 0)
      factor->clear();
}

/*
    Loading a #Desc# into the basis can be done more efficiently, by
    explicitely programming both cases, for the rowwise and for the columnwise
    representation. This implementation hides this distingtion in the use of
    methods #isBasic()# and #vector()#.
 */
void SPxBasis::loadDesc(const Desc& ds)
{
   METHOD( "SPxBasis::load()" );
   assert(status() > NO_PROBLEM);
   assert(theLP != 0);
   assert(ds.nRows() == theLP->nRows());
   assert(ds.nCols() == theLP->nCols());

   SPxId none;
   int   i;
   int   j;

   VERBOSE3({ spxout << "IBASIS02 loading of Basis invalidates factorization" << std::endl; });

   lastin      = none;
   lastout     = none;
   lastidx     = -1;
   iterCount   = 0;
   updateCount = 0;

   if (&ds != &thedesc)
   {
      thedesc = ds;
      setRep();
   }

   assert(theLP->dim() == matrix.size());

   DEBUG( dump(); );

   nzCount = 0;
   for (j = i = 0; i < theLP->nRows(); ++i)
   {
      if (theLP->isBasic(thedesc.rowStatus(i)))
      {
         SPxRowId id = theLP->SPxLP::rId(i);
         theBaseId[j] = id;
         matrix[j] = &theLP->vector(id);
         nzCount += matrix[j++]->size();
      }
   }

   for (i = 0; i < theLP->nCols(); ++i)
   {
      if (theLP->isBasic(thedesc.colStatus(i)))
      {
         SPxColId id = theLP->SPxLP::cId(i);
         theBaseId[j] = id;
         matrix[j] = &theLP->vector(id);
         nzCount += matrix[j++]->size();      
      }
   }

   assert(j == matrix.size());

   matrixIsSetup = true;
   factorized = false;
   if (factor != 0)
      factor->clear();
}

void SPxBasis::setRep()
{
   METHOD( "SPxBasis::setRep()" );
   assert(theLP != 0);

   reDim();
   minStab = 0.0;

   if (theLP->rep() == SPxSolver::ROW)
   {
      thedesc.stat   = &thedesc.rowstat;
      thedesc.costat = &thedesc.colstat;
   }
   else
   {
      thedesc.stat   = &thedesc.colstat;
      thedesc.costat = &thedesc.rowstat;
   }
}

void SPxBasis::load(SPxSolver* lp)
{
   METHOD( "SPxBasis::load()" );
   assert(lp != 0);
   theLP = lp;

   setRep();

   addedRows(lp->nRows());
   addedCols(lp->nCols());

   setStatus(REGULAR);

   loadDesc(thedesc);
}

void SPxBasis::loadSolver(SLinSolver* p_solver)
{
   METHOD( "SPxBasis::load()" );

   VERBOSE3({ spxout << "IBASIS03 loading of Solver invalidates factorization" << std::endl; });

   factor = p_solver;
   factorized = false;
   factor->clear();
}

/** 
 *  The specification is taken from the
 *
 *  ILOG CPLEX 7.0 Reference Manual, Appendix E, Page 543.
 *
 *  This routine should read valid BAS format files. 
 *
 *  @return true if the file was read correctly.
 *
 *  @todo This routine is untested.
 *  @todo We have to check for P_FIXED, if lower == upper
 */
bool SPxBasis::readBasis(
   std::istream&  is, 
   const NameSet& rownames, 
   const NameSet& colnames)
{
   METHOD( "SPxBasis::readBasis()" );
   assert(theLP != 0);

   int  i;
   Desc l_desc(thedesc);

   for(i = 0; i < theLP->nRows(); i++)
      l_desc.rowstat[i] = dualRowStatus(i);

   for(i = 0; i < theLP->nCols(); i++)
      l_desc.colstat[i] = Desc::P_ON_LOWER;

   MPSInput mps(is);

   if (mps.readLine() && (mps.field0() != 0) && !strcmp(mps.field0(), "NAME"))
   {
      while(mps.readLine())
      {
         int c = -1;
         int r = -1;

         if ((mps.field0() != 0) && !strcmp(mps.field0(), "ENDATA"))
         {
            mps.setSection(MPSInput::ENDATA);
            break;
         }
         if ((mps.field1() == 0) || (mps.field2() == 0))
            break;

         if ((c = colnames.number(mps.field2())) < 0)
            break;

         if (*mps.field1() == 'X')
            if (mps.field3() == 0 || (r = rownames.number(mps.field3())) < 0)
               break;

         if (!strcmp(mps.field1(), "XU"))
         {
            l_desc.colstat[c] = dualColStatus(c);
            l_desc.rowstat[r] = Desc::P_ON_UPPER;
         }
         else if (!strcmp(mps.field1(), "XL"))
         {
            l_desc.colstat[c] = dualColStatus(c);
            l_desc.rowstat[r] = Desc::P_ON_LOWER;
         }
         else if (!strcmp(mps.field1(), "UL"))
         {
            l_desc.colstat[c] = Desc::P_ON_UPPER;
         }
         else if (!strcmp(mps.field1(), "LL"))
         {
            l_desc.colstat[c] = Desc::P_ON_LOWER;
         }
         else
         {
            mps.syntaxError();
            break;
         }
      }
   }
   if (!mps.hasError())
   {
      if (mps.section() == MPSInput::ENDATA)
         loadDesc(l_desc);
      else
         mps.syntaxError();
   }
   thedesc.dump();

   return !mps.hasError();
}

void SPxBasis::writeBasis(   
   std::ostream&  os, 
   const NameSet& rownames, 
   const NameSet& colnames)
{
   METHOD( "SPxBasis::writeBasis()" );
   assert(theLP != 0);

   int col   = 0;
   int row   = 0;

   os.setf(std::ios::left);
   os << "NAME  soplex.bas\n";     

   if (theLP->rep() == SPxSolver::ROW)
   {
      ERROR( spxout << "writing basis for row representation not yet "
                         " implemented!" << std::endl; )
      return;
   }
   assert(theLP->rep() == SPxSolver::COLUMN);

   for(; col < theLP->nCols(); col++)
   {
      if( theLP->isBasic( thedesc.colStatus( col ))) 
      {
         /* Find non basic row
          */
         for(; row < theLP->nRows(); row++)
            if( !theLP->isBasic( thedesc.rowStatus( row )))
               break;

         assert( row != theLP->nRows() );

         os << ( thedesc.rowStatus( row ) == Desc::P_ON_UPPER ? " XU " : " XL " )
            << std::setw(8) << colnames[theLP->SPxLP::cId( col )]
            << "       " 
            << rownames[theLP->SPxLP::rId( row )]
            << std::endl;

         row++;
      }
      else
      {
         if( thedesc.colStatus( col ) == Desc::P_ON_UPPER )
         {
            os << " UL "
               << colnames[theLP->SPxLP::cId( col )]
               << std::endl;
         }
         else
         {
            /* Default is all Slacks basic, all variables on lower bound,
             * nothing to do in this case.
             * Non basic free variable should better not accure.
             */
            assert(thedesc.colStatus( col ) == Desc::P_ON_LOWER
               || thedesc.colStatus( col ) == Desc::P_FIXED);
         }
      }
   }
#ifndef NDEBUG
   for(; row < theLP->nRows(); row++)
      if( !theLP->isBasic( thedesc.rowStatus( row )))
         break;
   
   assert( row == theLP->nRows() );


   thedesc.dump();

   for(; row < theLP->nRows(); row++)
      if( !theLP->isBasic( thedesc.rowStatus( row )))
         break;
   std::cout << row << " " << theLP->nRows() << std::endl;

   assert( row == theLP->nRows() );
#endif // NDEBUG

   os << "ENDATA" << std::endl;
}


void SPxBasis::change(
   int i,
   SPxId& id,
   const SVector* enterVec,
   const SSVector* eta)
{
   METHOD( "SPxBasis::change()" );

   assert(matrixIsSetup);
   assert(!id.isValid() || (enterVec != 0));
   assert(factor != 0);

   lastidx = i;
   lastin  = id;

   if (id.isValid() && i >= 0)
   {
      assert(enterVec != 0);

      nzCount      = nzCount - matrix[i]->size() + enterVec->size();
      matrix[i]    = enterVec;
      lastout      = theBaseId[i];
      theBaseId[i] = id;

      ++iterCount;
      ++updateCount;

      // never factorize? Just do it !
      if (!factorized)
         factorize();
      // relative fill too high ?
      else if (Real(factor->memory()) > lastFill * Real(lastMem))
      {
         VERBOSE3({ spxout << "IBASIS04 fill factor triggers refactorization"
                             << " memory= " << factor->memory()
                             << " lastMem= " << lastMem
                             << " lastFill= " << lastFill
                             << std::endl; });

         factorize();
      }
      // absolute fill too high ?
      else if (Real(factor->memory()) > nonzeroFactor * Real(nzCount))
      {
         VERBOSE3({ spxout << "IBASIS05 nonzero factor triggers refactorization"
                             << " memory= " << factor->memory()
                             << " nzCount= " << nzCount
                             << " nonzeroFactor= " << nonzeroFactor
                             << std::endl; });
         factorize();
      }
      // too many updates ?
      else if (updateCount >= maxUpdates)
      {
         VERBOSE3({ spxout << "IBASIS06 update count triggers refactorization"
                             << " updateCount= " << updateCount
                             << " maxUpdates= " << maxUpdates
                             << std::endl; });
         factorize();
      }
      else
      {
         factor->change(i, *enterVec, eta);

         assert(minStab > 0.0);

         if (factor->status() != SLinSolver::OK || factor->stability() < minStab)
         {
            // VERBOSE3( spxout << s << " -> " << factor->stability() << '\t'; )
            VERBOSE3({ spxout << "IBASIS07 stability triggers refactorization"
                                << " stability= " << factor->stability()
                                << " minStab= " << minStab
                                << std::endl; });
            factorize();
         }
      }
   }
   else
      lastout = id;
}

void SPxBasis::factorize()
{
   METHOD( "SPxBasis::factorize()" );

   assert(factor != 0);

   if (!matrixIsSetup)
      loadDesc(thedesc);

   assert(matrixIsSetup);

   updateCount = 0;

   switch(factor->load(matrix.get_ptr(), matrix.size()))
   {
   case SLinSolver::OK :
      if (status() == SINGULAR)
         setStatus(REGULAR);

      minStab = factor->stability();

      // This seems allways be about 1e-7 
      if (minStab > 1e-4)
         minStab *= 0.001;
      if (minStab > 1e-5)
         minStab *= 0.01;
      if (minStab > 1e-6)
         minStab *= 0.1;
      break;
   case SLinSolver::SINGULAR :
      setStatus(SINGULAR);
      break;
   default :
      ERROR( spxout << "EBASIS08 error: unknown status of factorization.\n"; )
      assert(false);
      // factorized = false;
   }

   lastMem    = factor->memory();
   lastFill   = fillFactor * Real(factor->memory()) / Real(nzCount > 0 ? nzCount : 1);
   factorized = true;
}

Vector& SPxBasis::multWithBase(Vector& x) const
{
   METHOD( "SPxBasis::multWithBase()" );
   assert(status() > SINGULAR);
   assert(theLP->dim() == x.dim());

   int i;
   DVector tmp(x);

   if (!matrixIsSetup)
      (const_cast<SPxBasis*>(this))->loadDesc(thedesc);

   assert( matrixIsSetup );

   for (i = x.dim() - 1; i >= 0; --i)
      x[i] = *(matrix[i]) * tmp;

   return x;
}

Vector& SPxBasis::multBaseWith(Vector& x) const
{
   METHOD( "SPxBasis::multBaseWith()" );
   assert(status() > SINGULAR);
   assert(theLP->dim() == x.dim());

   int i;
   DVector tmp(x);

   if (!matrixIsSetup)
      (const_cast<SPxBasis*>(this))->loadDesc(thedesc);

   assert( matrixIsSetup );

   x.clear();
   for (i = x.dim() - 1; i >= 0; --i)
   {
      if (tmp[i])
         x.multAdd(tmp[i], *(matrix[i]));
   }

   return x;
}

void SPxBasis::dump()
{
   METHOD( "SPxBasis::dump()" );
   assert(status() > NO_PROBLEM);
   assert(theLP != 0);
   assert(thedesc.nRows() == theLP->nRows());
   assert(thedesc.nCols() == theLP->nCols());
   assert(theLP->dim() == matrix.size());

   int i, basesize;

   // Dump regardless of the verbosity level if this method is called.
   const SPxOut::Verbosity tmp_verbosity = spxout.getVerbosity();
   spxout.setVerbosity( SPxOut::ERROR );

   spxout << "Basis entries:" << std::endl;
   basesize = 0;
   for (i = 0; i < theLP->nRows(); ++i)
   {
      if (theLP->isBasic(thedesc.rowStatus(i)))
      {
         SPxRowId id = theLP->SPxLP::rId(i);
         spxout << "\tR" << theLP->number(id);
         basesize++;
         if(basesize % 8 == 0)
            spxout << std::endl;
      }
   }

   for (i = 0; i < theLP->nCols(); ++i)
   {
      if (theLP->isBasic(thedesc.colStatus(i)))
      {
         SPxColId id = theLP->SPxLP::cId(i);
         spxout << "\tC" << theLP->number(id);
         basesize++;
         if(basesize % 8 == 0)
            spxout << std::endl;
      }
   }
   spxout << std::endl;

   assert(basesize == matrix.size());
   spxout.setVerbosity( tmp_verbosity );
}

bool SPxBasis::isConsistent() const
{
   METHOD( "SPxBasis::isConsistent()" );
   int primals = 0;
   int i;

   if (status() > NO_PROBLEM)
   {
      if (theLP == 0)
         return MSGinconsistent("SPxBasis");

      if (theBaseId.size() != theLP->dim() || matrix.size() != theLP->dim())
         return MSGinconsistent("SPxBasis");

      if (thedesc.nCols() != theLP->nCols() || thedesc.nRows() != theLP->nRows())
         return MSGinconsistent("SPxBasis");

      for (i = 0; i < thedesc.nRows(); ++i)
      {
         if (thedesc.rowStatus(i) >= 0)
         {
            if (thedesc.rowStatus(i) != dualRowStatus(i))
               return MSGinconsistent("SPxBasis");
         }
         else
            ++primals;
      }
      
      for (i = 0; i < thedesc.nCols(); ++i)
      {
         if (thedesc.colStatus(i) >= 0)
         {
            if (thedesc.colStatus(i) != dualColStatus(i))
               return MSGinconsistent("SPxBasis");
         }
         else
            ++primals;
      }
      if (primals != thedesc.nCols())
         return MSGinconsistent("SPxBasis");
   }
   return thedesc.isConsistent() && theBaseId.isConsistent() 
      && matrix.isConsistent() && factor->isConsistent();
}

SPxBasis::SPxBasis()
   : theLP (0)
   , matrixIsSetup (false)
   , factor (0)
   , factorized (false)
   , maxUpdates (200) 
   , nonzeroFactor(10.0)
   , fillFactor(5.0)
   , nzCount (1)
   , minStab(0.0)
   , thestatus (NO_PROBLEM)
{
   METHOD( "SPxBasis::SPxBasis()" );
}

/**@warning Do not change the #LP or the #SLinSolver object.
 *  Only pointers to those objects are copied.
 */
SPxBasis::SPxBasis(const SPxBasis& old)
   : theLP(old.theLP)
   , theBaseId(old.theBaseId)
   , matrix(old.matrix)
   , matrixIsSetup(old.matrixIsSetup)
   , factor(old.factor)
   , factorized(old.factorized)
   , maxUpdates(old.maxUpdates)
   , nonzeroFactor(old.nonzeroFactor)
   , fillFactor(old.fillFactor)
   , iterCount(old.iterCount)
   , nzCount(old.nzCount)
   , lastFill(old.lastFill)
   , lastin(old.lastin)
   , lastout(old.lastout)
   , lastidx(old.lastidx)
   , minStab(old.minStab)
   , thestatus(old.thestatus)
   , thedesc(old.thedesc)
{
   METHOD( "SPxBasis::SPxBasis()" );
}

/**@warning Do not change the #LP or the #SLinSolver object.
 *  Only pointers to those objects are copied.
 */
SPxBasis& SPxBasis::operator=(const SPxBasis& rhs)
{
   METHOD( "SPxBasis::operator=()" );

   if (this != &rhs)
   {
      theLP         = rhs.theLP;
      theBaseId     = rhs.theBaseId;
      matrix        = rhs.matrix;
      matrixIsSetup = rhs.matrixIsSetup;
      factor        = rhs.factor;
      factorized    = rhs.factorized;
      maxUpdates    = rhs.maxUpdates;
      nonzeroFactor = rhs.nonzeroFactor;
      fillFactor    = rhs.fillFactor;
      iterCount     = rhs.iterCount;
      nzCount       = rhs.nzCount;
      lastFill      = rhs.lastFill;
      lastin        = rhs.lastin;
      lastout       = rhs.lastout;
      lastidx       = rhs.lastidx;
      minStab       = rhs.minStab;
      thestatus     = rhs.thestatus;
      thedesc       = rhs.thedesc;
   }
   return *this;
}

} // namespace soplex

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------
