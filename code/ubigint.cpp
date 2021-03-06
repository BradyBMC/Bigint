// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $
// Evan Clark Brady Chan
// 4/15/21
// Bigint
#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cstring>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

ubigint::ubigint (unsigned long that) : ubig_value(0) {
   while(that > 0) {
     ubig_value.push_back(that%10);
     that = that/10;
   }
}

ubigint::ubigint (const string& that) : ubig_value(0){
   DEBUGF ('~', "that = \"" << that << "\"");
   for(auto it = that.crbegin(); it != that.crend();it++) {
      if(not isdigit(*it)) {
        throw invalid_argument ("ubigint:: ubigint(" + that + ")");
      }
      ubig_value.push_back(*it - '0');
   }
   while(ubig_value.size() > 0 && ubig_value.back() == 0){
     ubig_value.pop_back();
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   ubigint result;
   int indxt = 0;
   int indx = 0;
   int size = ubig_value.size();
   int sizet = that.ubig_value.size();
   bool carry = false;
   while(indx < size && indxt < sizet) {
     uint8_t sum = 0;
     if(carry) {
       sum++;
       carry = false;
     }
     sum = ubig_value[indx] + that.ubig_value[indxt] + sum;
     if(sum > 9) {
       sum = sum - 10;
       carry = true;
     }
     result.ubig_value.push_back(sum);
     indx++;
     indxt++;
   }
   if(carry && indx == size - 1 && indxt == sizet - 1) {
     result.ubig_value.push_back(1);
     return result;
   }
   while(indx < size) {
     uint8_t sum = 0;
     if(carry) {
       sum++;
       carry = false;
     }
     sum += ubig_value[indx];
     if(sum > 9) {
       sum = sum - 10;
       carry = true;
     }
     result.ubig_value.push_back(sum);
     indx++;
   }
   while(indxt < sizet) { 
     uint8_t sum = 0;
     if(carry) {
       sum++;
       carry = false;
     }
     sum += that.ubig_value[indxt];
     if(sum > 9) {
       sum = sum - 10;
       carry = true;
     }
     result.ubig_value.push_back(sum);
     indxt++;
   }
   if(carry) {
     result.ubig_value.push_back(1);
   }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if(*this < that) throw domain_error ("ubigint::operator~(a<b)");
   ubigint result {0};
   if(*this == that) {
     return result;
   }
   int indx = 0;
   int indxt = 0;
   int size = ubig_value.size();
   int sizet = that.ubig_value.size();
   bool carry = false;

   while(indx < size && indxt < sizet) {
     int sub = 0;
     if(carry) {
       sub = -1;
       carry = false;
     }
     sub = ubig_value[indx] - that.ubig_value[indxt] + sub;
     if(sub < 0) {
       sub += 10;
       carry = true;
     }
     int temp = sub;
     result.ubig_value.push_back(temp);
     indx++;
     indxt++;
   }
   while(indx < size) {
     if(carry) {
       if(ubig_value[indx] - 1 < 0) {
         result.ubig_value.push_back(ubig_value[indx] + 9);
       } else {
         carry = false;
         result.ubig_value.push_back(ubig_value[indx] - 1);
       }
     } else {
       result.ubig_value.push_back(ubig_value[indx]);
     }
     indx++;
   }
   while(result.ubig_value.size() > 0 &&result.ubig_value.back() ==0){
     result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint result;
   int size = ubig_value.size();
   int sizet = that.ubig_value.size();
   int length = ubig_value.size() + that.ubig_value.size();
   for(int i = 0;i < length;i++) result.ubig_value.push_back(0);
   for(int i = 0;i < size;i++) {
     uint8_t c = 0;
     for(int j = 0;j < sizet;j++) {
       uint8_t total = result.ubig_value[i+j];
       total = total + ubig_value[i] * that.ubig_value[j] + c;
       result.ubig_value[i+j] = total % 10;
       c = total/10;
     }
     result.ubig_value[i+that.ubig_value.size()] += c;
   }
   while(result.ubig_value.size() > 0 &&result.ubig_value.back() == 0){
     result.ubig_value.pop_back();
   }
   return result;
}

void ubigint::multiply_by_2() {
   int size = ubig_value.size();
   bool carry = false;
   for(int i = 0;i < size;i++) {
     uint8_t dub = 0;
     if(carry) {
       dub = 1;
       carry = false;
     }
     dub = dub + (ubig_value[i] << 1);
     if(dub > 9) {
       dub -= 10;
       carry = true;
     }
     ubig_value[i] = dub;
   }
   if(carry) {
       ubig_value.push_back(1);
   }
}

void ubigint::divide_by_2() {
   int size = ubig_value.size();
   bool carry = false;
   for(int i = size - 1;i > 0;i--) {
     uint8_t div = 0;
     if(carry) {
       div = 10;
       carry = false;
     }
     uint8_t temp = div + ubig_value[i];
     div = (div + ubig_value[i]) >> 1;
     if(temp%2 == 1) {
       carry = true;
     }
     ubig_value[i] = div;
   }
   if(carry) {
     ubig_value[0] = (ubig_value[0] + 10) >> 1;
   } else {
     ubig_value[0] = (ubig_value[0]) >> 1;
   }
   while(ubig_value.size() > 0 && ubig_value.back() == 0){
     ubig_value.pop_back();
   }
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(ubig_value.size() != that.ubig_value.size()) {
     return false;
   }
   int index = ubig_value.size() - 1;
   while(index >= 0) {
       if(ubig_value[index] < that.ubig_value[index]) {
         return false;
       } else if(that.ubig_value[index] < ubig_value[index]) {
         return false;
       }
       index--;
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   if(ubig_value.size() < that.ubig_value.size()) {
     return true;
   } else if(ubig_value.size() > that.ubig_value.size()) {
     return false;
   } else {
     for(int i = ubig_value.size() - 1;i >= 0;i--) {
       if(ubig_value[i] < that.ubig_value[i]) {
         return true;
       } else if(ubig_value[i] > that.ubig_value[i]) {
         return false;
       }
     }
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   int cnt = 1;
   for(int i = that.ubig_value.size() - 1;i >= 0;i--) {
     int result = that.ubig_value[i];
     if(cnt == 70) 
     {
       out << "\\" << "\n" << result;
       cnt = 1;
     } else {
       out << result;
     }
     cnt++;
   }
   if(that.ubig_value.size() == 0) {
     out << "0";
   }
   return out;
}
