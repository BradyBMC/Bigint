// $Id: ubigint.h,v 1.6 2021-04-14 13:24:23-07 - - $
// Evan Clark Brady Chan
// 4/15/21
// Bigint
#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
using namespace std;

#include "debug.h"

class ubigint {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
      using ubigvalue_t = vector<uint8_t>;
      ubigvalue_t ubig_value {};
   public:
      void multiply_by_2();
      void divide_by_2();

      ubigint() = default; // Need default ctor as well.
      ubigint (unsigned long);
      ubigint (const string&);

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;
};

#endif

