#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hrs, mins, rem_secs;
    hrs = seconds/3600;
    rem_secs = seconds % 3600;
    mins = rem_secs / 60;
    rem_secs = rem_secs % 60;
    return std::to_string(hrs) + ":" + std::to_string(mins) + ":" + std::to_string(rem_secs);
 }