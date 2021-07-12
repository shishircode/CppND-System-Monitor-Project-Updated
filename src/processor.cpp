#include "processor.h"
#include "linux_parser.h"

#include <chrono>
#include <thread>

float Processor::Utilization() { 

    long total = LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    float totald =  LinuxParser::Jiffies() - total; 
    float idled =  LinuxParser::IdleJiffies() - idle;

    float cpu_percentage = (totald - idled)/totald;
    return cpu_percentage;
 }