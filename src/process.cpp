#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return _pid; }

float Process::CpuUtilization() const { 
    long _totaltime = LinuxParser::ActiveJiffies(_pid);
    long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(_pid)/sysconf(_SC_CLK_TCK);
    long cpu = (_totaltime/sysconf(_SC_CLK_TCK))/seconds;
    return cpu;
 }

string Process::Command() { return LinuxParser::Command(_pid); }

string Process::Ram() { return LinuxParser::Ram(_pid); }

string Process::User() { return LinuxParser::User(_pid); }

long int Process::UpTime() { return LinuxParser::UpTime(_pid); }

bool Process::operator<(Process &a) { return CpuUtilization() < a.CpuUtilization(); }