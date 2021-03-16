#include <cassert>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

#include <iostream>

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  seconds %= 3600;
  long minutes = seconds / 60;
  seconds %= 60;
  std::stringstream ss;
  auto formatDigits = [&](int a) { ss << (a < 10 ? "0" : "") << a; };
  formatDigits(hours);
  ss << ":";
  formatDigits(minutes);
  ss << ":";
  formatDigits(seconds);
  return ss.str();
}