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
#pragma ident "@(#) $Id: svector.cpp,v 1.7 2001/12/26 12:58:59 bzfkocht Exp $"

#include <assert.h>
#include <iostream>

#include "svector.h"
#include "ssvector.h"
#include "message.h"

namespace soplex
{
void SVector::toFront(int n)
{
   assert(n < size() && n >= 0);
   Element dummy = m_elem[n];
   m_elem[n] = m_elem[0];
   m_elem[0] = dummy;
}

void SVector::add(int n, const int i[], const double v[])
{
   assert(n + size() <= max());
   Element* e = m_elem + size();
   set_size( size() + n );
   while (n--)
   {
      e->idx = *i++;
      e->val = *v++;
      e++;
   }
}

void SVector::add(int n, const Element e[])
{
   assert(n + size() <= max());
   Element* ee = m_elem + size();
   set_size( size() + n );
   while (n--)
      *ee++ = *e++;
}

void SVector::remove(int n, int m)
{
   assert(n <= m && m < size() && n >= 0);
   ++m;

   int cpy = m - n;
   cpy = (size() - m >= cpy) ? cpy : size() - m;

   Element* e = &m_elem[size() - 1];
   Element* r = &m_elem[n];
   set_size( size() - cpy );
   do
   {
      *r++ = *e--;
   }
   while (--cpy);
}

int SVector::dim() const
{
   const Element* e = m_elem;
   int d = -1;
   int n = size();
   while (n--)
   {
      d = (d > e->idx) ? d : e->idx;
      e++;
   }
   return d;
}

void SVector::sort()
{
   Element dummy;
   Element* w;
   Element* l;
   Element* s = &(m_elem[0]);
   Element* e = s + size();
   for (l = s, w = s + 1; w < e; l = w, ++w)
   {
      if (l->idx > w->idx)
      {
         dummy = *w;
         do
         {
            l[1] = *l;
            if (l-- == s)
               break;
         }
         while (l->idx > dummy.idx);
         l[1] = dummy;
      }
   }
}

double SVector::length2() const
{
   double x = 0;
   int n = size();
   const Element* e = m_elem;
   while (n--)
   {
      x += e->val * e->val;
      e++;
   }
   return x;
}

double SVector::maxAbs() const
{
   double x = 0;
   int n = size();
   const Element* e = m_elem;
   while (n--)
   {
      x = (e->val > x) ? e->val : ((-e->val > x) ? -e->val : x);
      e++;
   }
   return x;
}

double SVector::minAbs() const
{
   double         x = 1e100;
   int            n = size();
   const Element* e = m_elem;

   while (n--)
   {
      x = (e->val < x) ? e->val : ((-e->val < x) ? -e->val : x);
      e++;
   }
   return x;
}

SVector& SVector::operator*=(double x)
{
   int n = size();
   Element* e = m_elem;
   while (n--)
   {
      e->val *= x;
      e++;
   }
   return *this;
}

SVector& SVector::operator=(const SSVector& sv)
{
   assert(max() >= sv.size());
   set_size( sv.size() );
   int i = size();
   Element *e = m_elem;

   while (i--)
   {
      e->idx = sv.index(i);
      e->val = sv[e->idx];
      ++e;
   }
   return *this;
}

SVector& SVector::operator=(const Vector& vec)
{
   int n = 0;
   int i = vec.dim();
   Element *e = m_elem;
   clear();
   while (i--)
   {
      if (vec[i])
      {
         assert(n < max());
         e->idx = i;
         e->val = vec[i];
         ++e;
         ++n;
      }
   }
   set_size( n );
   return *this;
}

SVector& SVector::assign(const Vector& vec, double eps)
{
   int n = 0;
   int i = vec.dim();
   double x;
   Element* e = m_elem;
   clear();
   while (i--)
   {
      x = vec[i];
      if (x > eps || x < -eps)
      {
         assert(n < max());
         e->idx = i;
         e->val = x;
         ++e;
         ++n;
      }
   }
   set_size( n );
   return *this;
}

SVector& SVector::operator=(const SVector& sv)
{
   assert(max() >= sv.size());
   int i = sv.size();
   Element       *e = m_elem;
   const Element *s = sv.m_elem;
   while (i--)
      *e++ = *s++;
   set_size( sv.size() );
   return *this;
}

std::ostream& operator<<(std::ostream& os, const SVector& v)
{
   int i, j;
   for (i = j = 0; i < v.size(); ++i)
   {
      if (j)
      {
         if (v.value(i) < 0)
            os << " - " << -v.value(i);
         else
            os << " + " << v.value(i);
      }
      else
         os << v.value(i);
      os << " x" << v.index(i);
      j = 1;
      if ((i + 1) % 4 == 0)
         os << "\n\t";
   }
   return os;
}

int SVector::isConsistent() const
{
   if (m_elem != 0)
   {
      if (size() > max()){
         return MSGinconsistent("SVector");
      }
      for (int i = 1; i < size(); ++i)
      {
         for (int j = 0; j < i; ++j)
         {
            if (m_elem[i].idx == m_elem[j].idx &&
                m_elem[i].idx != 0 ) { // allow trailing zeros
               return MSGinconsistent("SVector");
            }
         }
      }
   }
   return 1;
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
