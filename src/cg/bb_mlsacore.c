/*************************************************************************/
/*                This code has been modified for Bellbird.              */
/*                See COPYING for more copyright details.                */
/*                The unmodified source code copyright notice            */
/*                is included below.                                     */
/*************************************************************************/
/*  ---------------------------------------------------------------  */
/*      The HMM-Based Speech Synthesis System (HTS): version 1.1b    */
/*                        HTS Working Group                          */
/*                                                                   */
/*                   Department of Computer Science                  */
/*                   Nagoya Institute of Technology                  */
/*                                and                                */
/*    Interdisciplinary Graduate School of Science and Engineering   */
/*                   Tokyo Institute of Technology                   */
/*                      Copyright (c) 2001-2003                      */
/*                        All Rights Reserved.                       */
/*                                                                   */
/*  Permission is hereby granted, free of charge, to use and         */
/*  distribute this software and its documentation without           */
/*  restriction, including without limitation the rights to use,     */
/*  copy, modify, merge, publish, distribute, sublicense, and/or     */
/*  sell copies of this work, and to permit persons to whom this     */
/*  work is furnished to do so, subject to the following conditions: */
/*                                                                   */
/*    1. The code must retain the above copyright notice, this list  */
/*       of conditions and the following disclaimer.                 */
/*                                                                   */
/*    2. Any modifications must be clearly marked as such.           */
/*                                                                   */
/*  NAGOYA INSTITUTE OF TECHNOLOGY, TOKYO INSITITUTE OF TECHNOLOGY,  */
/*  HTS WORKING GROUP, AND THE CONTRIBUTORS TO THIS WORK DISCLAIM    */
/*  ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL       */
/*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL NAGOYA INSTITUTE OF TECHNOLOGY, TOKYO INSITITUTE OF        */
/*  TECHNOLOGY, HTS WORKING GROUP, NOR THE CONTRIBUTORS BE LIABLE    */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY        */
/*  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  */
/*  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS   */
/*  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR          */
/*  PERFORMANCE OF THIS SOFTWARE.                                    */
/*                                                                   */
/*  This is originally part of Heiga Zen's mlsa code.                */
/*  It was modified by Toda and Black eventually ending up in Flite. */
/*-------------------------------------------------------------------*/

/* This source file is included in more than one compilation unit.      */
/* Ordinarily we might use normal functions but we use static functions */
/* as experience has showed us this code is highly performance critical */
/* and the compiler likes to inline and improve performance of these    */
/* functions if we declare them static.                                 */

#define BELL_PORDER 5

#define RANDMAX 32767
static double mlsadf1(double x, const double *c, const double a,
                       double *d1, const double *ppade)
{
   double v, out = 0.0, *pt, aa;
   int i;

   aa = 1 - a*a;
   pt = &d1[BELL_PORDER+1];

   for (i=BELL_PORDER; i>=1; i--) {
      d1[i] = aa*pt[i-1] + a*d1[i];
      pt[i] = d1[i] * c[1];
      v = pt[i] * ppade[i];
      x += (1 & i) ? v : -v;
      out += v;
   }

   pt[0] = x;
   out += x;

   return(out);
}

static double mlsadf2(double x, const double *c, const int m, const double a,
                       double *d2, const double *ppade, int *pd2offset)
{
// This function is partially optimized as it is extremely performance
// critical to bellbird. Before making changes
// ensure that changes have been profiled.
// This function avoids some unnecessary memory copies
// which improves performance at the cost of slightly more
// memory (a few hundred bytes).
// This function has been partially optimized with "by hand" loop
// unrolling and caching to reduce impact of recursion in the underlying equations.
// This outperforms gcc and clang's builtin optimizers.
// This function only works for BELL_PORDER=5.

   double v, out = 0.0;
   double * const pt = &d2[BELL_PORDER * (m+4)];
   const double aa = 1 - a*a;
   double ptcache[BELL_PORDER]; // temp holding pt[] values to reduce recursiveness
   const int d2offset = *pd2offset;
   int j,k;
// default values for start and end of loops through history terms
// set end of first loop
   const int offsetend1 = (d2offset == BELL_PORDER) ? BELL_PORDER*(m+1): BELL_PORDER*(m+2);
// set end of second loop
   const int offsetend2 = d2offset-2*BELL_PORDER;
// start of second loop
   int offsetstart2 = BELL_PORDER;
// index of second element
   int secelement = d2offset + BELL_PORDER;
// a special case for loop lengths
   if (d2offset == BELL_PORDER*(m+2)) {
      offsetstart2 = 2*BELL_PORDER;
      secelement = BELL_PORDER;
   }

// Filtering but without shuffling history terms.
// d2offset is index of start of history terms which are stored in a
// wrap around buffer.
// Elements d2[0] and d2[BELL_PORDER*(m+3)] are 'ghost' edge elements
// to avoid conditionals in stencil (d2[k+BELL_PORDER]-d2[k-BELL_PORDER])
   ptcache[0] = 0.0;
   ptcache[1] = 0.0;
   ptcache[2] = 0.0;
   ptcache[3] = 0.0;
   ptcache[4] = 0.0;
   j = 2; // initialize indexing for MLSA filter coefficients

// Second element is a special case calculate it here
   d2[secelement]   = aa*pt[0] + a*d2[secelement];
   d2[secelement+1] = aa*pt[1] + a*d2[secelement+1];
   d2[secelement+2] = aa*pt[2] + a*d2[secelement+2];
   d2[secelement+3] = aa*pt[3] + a*d2[secelement+3];
   d2[secelement+4] = aa*pt[4] + a*d2[secelement+4];

// First part of loop through wrap around buffer
   for (k=d2offset+2*BELL_PORDER; k<=offsetend1; k+=BELL_PORDER,j++) {
      d2[k]   += a*(d2[k+BELL_PORDER]  -d2[k-BELL_PORDER]);
      d2[k+1] += a*(d2[k+BELL_PORDER+1]-d2[k-BELL_PORDER+1]);
      d2[k+2] += a*(d2[k+BELL_PORDER+2]-d2[k-BELL_PORDER+2]);
      d2[k+3] += a*(d2[k+BELL_PORDER+3]-d2[k-BELL_PORDER+3]);
      d2[k+4] += a*(d2[k+BELL_PORDER+4]-d2[k-BELL_PORDER+4]);
      ptcache[0] += d2[k] * c[j];
      ptcache[1] += d2[k+1]*c[j];
      ptcache[2] += d2[k+2]*c[j];
      ptcache[3] += d2[k+3]*c[j];
      ptcache[4] += d2[k+4]*c[j];
   }

// Update 'ghost' edge element of stencil d2[0]
   d2[0] = d2[BELL_PORDER*(m+2)];
   d2[1] = d2[BELL_PORDER*(m+2)+1];
   d2[2] = d2[BELL_PORDER*(m+2)+2];
   d2[3] = d2[BELL_PORDER*(m+2)+3];
   d2[4] = d2[BELL_PORDER*(m+2)+4];

// Second part of loop through wrap around buffer
   for (k=offsetstart2; k<=offsetend2; k+=BELL_PORDER,j++) {
      d2[k]   += a*(d2[k+BELL_PORDER]  -d2[k-BELL_PORDER]);
      d2[k+1] += a*(d2[k+BELL_PORDER+1]-d2[k-BELL_PORDER+1]);
      d2[k+2] += a*(d2[k+BELL_PORDER+2]-d2[k-BELL_PORDER+2]);
      d2[k+3] += a*(d2[k+BELL_PORDER+3]-d2[k-BELL_PORDER+3]);
      d2[k+4] += a*(d2[k+BELL_PORDER+4]-d2[k-BELL_PORDER+4]);
      ptcache[0] += d2[k] * c[j];
      ptcache[1] += d2[k+1]*c[j];
      ptcache[2] += d2[k+2]*c[j];
      ptcache[3] += d2[k+3]*c[j];
      ptcache[4] += d2[k+4]*c[j];
   }

// Copy element which is not shuffled in usual implement. of this function
   d2[d2offset]   = d2[secelement];
   d2[d2offset+1] = d2[secelement+1];
   d2[d2offset+2] = d2[secelement+2];
   d2[d2offset+3] = d2[secelement+3];
   d2[d2offset+4] = d2[secelement+4];

// Copy 'ghost' edge elements for stencil in next call of this function
   d2[0] = d2[BELL_PORDER*(m+2)];
   d2[1] = d2[BELL_PORDER*(m+2)+1];
   d2[2] = d2[BELL_PORDER*(m+2)+2];
   d2[3] = d2[BELL_PORDER*(m+2)+3];
   d2[4] = d2[BELL_PORDER*(m+2)+4];
   d2[BELL_PORDER*(m+3)]   = d2[BELL_PORDER];
   d2[BELL_PORDER*(m+3)+1] = d2[BELL_PORDER+1];
   d2[BELL_PORDER*(m+3)+2] = d2[BELL_PORDER+2];
   d2[BELL_PORDER*(m+3)+3] = d2[BELL_PORDER+3];
   d2[BELL_PORDER*(m+3)+4] = d2[BELL_PORDER+4];

// Update pt values from cache
   pt[5] = ptcache[4];
   pt[4] = ptcache[3];
   pt[3] = ptcache[2];
   pt[2] = ptcache[1];
   pt[1] = ptcache[0];

   for (k=BELL_PORDER; k>=1; k--) {
       v = pt[k] * ppade[k];
       x  += (1 & k) ? v : -v;
       out += v;
   }
    
   pt[0] = x;
   out  += x;

// update index pointing to start of history terms
   (*pd2offset) -= BELL_PORDER;
   if (*pd2offset<BELL_PORDER) *pd2offset = BELL_PORDER*(m+2);

   return(out);
}

static double mlsadf(double x, const double *c, const int m, const double a,
                      double *d1, int * pd2offset, const double *ppade)
{
// the mel log spectrum approximation (MLSA) digital filter
// x : input
// c : MLSA filter coefficients
// m : order of cepstrum
// a : alpha, the all-pass constant
// d1: working memory - history terms
// pd2offset: history term indexing parameter
// ppade: Pade approximant coefficients

   x = mlsadf1 (x, c, a, d1, ppade);
   x = mlsadf2 (x, c, m, a, &d1[2*(BELL_PORDER+1)], ppade, pd2offset);

   return(x);
}

static void freqt (const double * const mc, const int m, double *cep, const int irleng, const double a)
{ // frequency transformation
   int i, j;
   const double aa = 1 - a * a;
   double temp;  // pair of temps to hold cep values between loop iterations
   double temp1;

   for (i = 0; i < irleng; i++)
      cep[i] = 0.0;

   for (i = -m; i <= 0; i++) {
      temp = cep[0];
      cep[0] = mc[-i] + a * temp;
      temp1 = cep[1];
      cep[1] = aa * temp + a * temp1;
      for (j=2; j < irleng ; j++)
      {
         temp = temp1;
         temp1 = cep[j];
         cep[j] = temp + a * (temp1 - cep[j-1]);
      }
   }

   return;
}

static double rnd(unsigned long *next)
{
// the stock standard linear congruential random number generator
   double r;

   *next = *next * 1103515245L + 12345;
   r = (*next / 65536L) % 32768L;

   return(r/RANDMAX);
}

static void mc2b (double *mc, double *b, int m, const double a)
{
// transform mel-cepstrum to MLSA digital filter coefficients
   b[m] = mc[m];

   for (m--; m>=0; m--)
      b[m] = mc[m] - a * b[m+1];

   return;
}

static void b2mc(const double *b, double *mc, int m, const double a)
{
// transform MLSA digital filter coefficients to mel-cepstrum
   double d, o;

   d = mc[m] = b[m];
   for (m--; m >= 0; m--) {
      o = b[m] + a * d;
      d = b[m];
      mc[m] = o;
   }
}

static void c2ir (const double * const cep, const int irleng, double *ir)
{
// minimum phase impulse response is evaluated from the minimum phase cepstrum
   int n, k;
   double  d;

   ir[0] = exp(cep[0]);
   for (n = 1; n < irleng; n++) {
      d = 0;
      for (k = 1; k <= n; k++)
         d += k * cep[k] * ir[n-k];
      ir[n] = d / n;
   }

   return;
}
