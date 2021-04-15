// $Id: bigint.cpp,v 1.4 2021-04-14 13:24:23-07 - - $
// Evan Clark Brady Chan
// 4/15/21
// Bigint
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   ubigint zero {0};
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
   if(uvalue == zero) is_negative = false;

}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   ubigint result;
   bool b;
   bigint larger;
   if (!(is_negative != that.is_negative)) {
     result = uvalue + that.uvalue;
     b = is_negative;
   } else {
     if (uvalue == that.uvalue) {
       result = uvalue - that.uvalue;
       b = false;
     } else if (uvalue > that.uvalue) {
       result = uvalue - that.uvalue;
       b = is_negative;
     } else {
       result = that.uvalue - uvalue;
       b = that.is_negative;
     }
   }
   return {result, b};
}

bigint bigint::operator- (const bigint& that) const {
  bigint result;
  if(is_negative != that.is_negative) {
    result = uvalue + that.uvalue;
    result.is_negative = (is_negative) ? true:false;
  } else {
    if(uvalue > that.uvalue) {
      result = uvalue - that.uvalue;
      result.is_negative = false;
    } else {
      result = that.uvalue - uvalue;
      result.is_negative = true;
    }
    if(uvalue == that.uvalue) {
      result.is_negative = false;
    }
  }
  return result;
}


bigint bigint::operator* (const bigint& that) const {
  bigint zero = {0};
  bigint result = uvalue * that.uvalue;
  if(result != zero) {
    result.is_negative = (is_negative == that.is_negative) ? false:true;
  } else {
    result.is_negative = false;
  }
  return result;
}

bigint bigint::operator/ (const bigint& that) const {
  bigint zero = {0};
  bigint result = uvalue / that.uvalue;
  if(result != zero) {
    result.is_negative = (is_negative == that.is_negative) ? false:true;
  } else {
    result.is_negative = false;
  }
  return result;
}

bigint bigint::operator% (const bigint& that) const {
  bigint zero = {0};
  bigint result = uvalue % that.uvalue;
  if(result != zero) {
    result.is_negative = (is_negative == that.is_negative) ? false:true;
  } else {
    result.is_negative = false;
  }
  return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   bigint zero = {0};
   if(that == zero) {
     return out << "0";
   }
   if(that.is_negative) {
     return out << "-" << that.uvalue;
   } else {
     return out << that.uvalue;
   }
}

