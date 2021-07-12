#include <string>
#include <iomanip>

#include "format.h"

using std::string;


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    std::ostringstream stream;
    int hrs, mins, rem_secs;
    hrs = seconds/3600;
    rem_secs = seconds % 3600;
    mins = rem_secs / 60;
    rem_secs = rem_secs % 60;
    stream << std::setw(2) << std::setfill('0') << hrs << ":" << std::setw(2) << std::setfill('0') << mins << ":" << std::setw(2) << std::setfill('0') << rem_secs;
    return stream.str();
 }