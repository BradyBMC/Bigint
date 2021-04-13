// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

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
   //cout << that << endl;
   while(that > 0) {
     ubig_value.push_back(that%10);
     that = that/10;
   }
   //DEBUGF ('~', this << " -> " << ubig_value.back)
}

ubigint::ubigint (const string& that) : ubig_value(0){
   DEBUGF ('~', "that = \"" << that << "\"");
   /*
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      uvalue = uvalue * 10 + digit - '0';
   }
   */
   for(auto it = that.crbegin(); it != that.crend();it++) {
      if(not isdigit(*it)) {
        throw invalid_argument ("ubigint:: ubigint(" + that + ")");
      }
      ubig_value.push_back(*it - '0');
   }
}

//May crap out if one number is actually negative
ubigint ubigint::operator+ (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   //ubigint result (uvalue + that.uvalue);
   ubigint result;
   //Index for that
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
   //DEBUGF ('u', result);
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   //if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint result;
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
     //cout << "ubig" << (int)ubig_value[indx] << endl;
     //cout << "that" << (int)that.ubig_value[indxt] << endl;
     //cout << sub << endl;
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
         //cout << ubig_value[indx] - 1 << endl;
         result.ubig_value.push_back(ubig_value[indx] - 1);
       }
     } else {
       result.ubig_value.push_back(ubig_value[indx]);
     }
     indx++;
   }
   while(result.ubig_value.size() > 0 and result.ubig_value.back() == 0) result.ubig_value.pop_back();
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
       uint8_t total = result.ubig_value[i+j] + ubig_value[i] * that.ubig_value[j] + c;
       result.ubig_value[i+j] = total % 10;
       c = total/10;
     }
     result.ubig_value[i+that.ubig_value.size()] += c;
   }
   while(result.ubig_value.size() > 0 and result.ubig_value.back() == 0){
     result.ubig_value.pop_back();
   }
   return result;
   //return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   //uvalue *= 2;
}

void ubigint::divide_by_2() {
   //uvalue /= 2;
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
   //return uvalue == that.uvalue;
   
   if(ubig_value.size() != that.ubig_value.size()) return false;
   int index = ubig_value.size() - 1;
   while(index > 0) {
       if(ubig_value[index] < that.ubig_value[index]) {
         return false;
       } else if(that.ubig_value[index] < ubig_value[index]) {
         return false;
       }
       if(index == 0) {
         return true;
       }
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   //return uvalue < that.uvalue;
   cout << "here" << endl;
   if(ubig_value.size() < that.ubig_value.size()) {
     cout << "out" << endl;
     return true;
   } else if(ubig_value.size() > that.ubig_value.size()) {
     cout << "out2" << endl;
     return false;
   } else {
     for(int i = ubig_value.size() - 1;i >= 0;i--) {
       if(ubig_value[i] < that.ubig_value[i]) {
         cout << "error" << endl;
         return true;
       } else if(ubig_value[i] > that.ubig_value[i]) {
         cout << "error" << endl;
         return false;
       }
     }
   }
   //cout << "def" << endl;
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   //return out << "ubigint(" << that.uvalue << ")";
   out << "ubigint(";
   for(int i = that.ubig_value.size() - 1;i >= 0;i--) {
     int result = that.ubig_value[i];
     out << result;
   }
   return out <<  ")";
}
