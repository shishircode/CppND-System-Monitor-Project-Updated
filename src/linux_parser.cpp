#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string GetValue(string given_key, string file_path){
  string line, key, val;
  std::ifstream filestream(file_path);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> val){
        if (key == given_key) {
          return val;
        }
      }
    }
    filestream.close();
  }
  return "";
}

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  std::ifstream file(kProcDirectory + kMeminfoFilename);
  float mem_total = 0;
  float mem_free = 0;
  if (file.is_open()){
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") 
          mem_total = stof(value);
        else if (key == "MemFree:")
          mem_free = stof(value);
  }
    }
    file.close();
  }
  return (mem_total - mem_free)/mem_total; 
  }

long LinuxParser::UpTime() { 
  string uptime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    stream.close();
  }
  return std::stol(uptime);
}

long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) { 
  long total = 0, utime = 0, stime = 0, cutime = 0, cstime = 0;
  string line, value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      values.push_back(value);
    }
    filestream.close();
  }
  if(values.size() > 16){
  if(values[13] != "") utime = std::stol(values[13]);
  if(values[14] != "") stime = std::stol(values[14]);
  if(values[15] != "") cutime = std::stol(values[15]);
  if(values[16] != "") cstime = std::stol(values[16]);
  }
  total = utime + stime + cutime + cstime;
  return total;
 }

long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  vector<CPUStates> states = {CPUStates::kUser_, CPUStates::kNice_, CPUStates::kSystem_, CPUStates::kIRQ_, CPUStates::kSoftIRQ_, CPUStates::kSteal_};
  long active_jiffies = 0;
  for(auto state:states){
    if(state < jiffies.size() && jiffies[state] != ""){
      active_jiffies += std::stol(jiffies[state]);
    }
  }
  return active_jiffies;
}

long LinuxParser::IdleJiffies() {
  long idle_jiffies = 0;
  vector<string> jiffies = CpuUtilization();
  if(jiffies.size() > CPUStates::kIOwait_){
    if(jiffies[CPUStates::kIdle_] != "" && jiffies[CPUStates::kIOwait_] != ""){
      idle_jiffies = std::stol(jiffies[CPUStates::kIdle_]) + std::stol(jiffies[CPUStates::kIOwait_]);
    }
 }
 return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, value;
  vector<string> jiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    while(linestream >> value){
      if(value != "cpu") jiffies.push_back(std::move(value));
    }
    filestream.close();
  }
  return jiffies;
 }

int LinuxParser::TotalProcesses() { 
  string num = GetValue("processes", kProcDirectory+kStatFilename);
  return std::stoi(num);
 }

int LinuxParser::RunningProcesses() {
  string num = GetValue("procs_running", kProcDirectory+kStatFilename);
  return std::stoi(num);
 }

string LinuxParser::Command(int pid) {
  string val;
  std::ifstream filestream(kProcDirectory +std::to_string(pid)+kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, val);
    filestream.close();
  }
  return val;
 }

string LinuxParser::Ram(int pid) { 
  string ram = GetValue("VmRSS:", kProcDirectory+std::to_string(pid)+kStatusFilename); // Replacing VmSize with VmRSS as that is the actual physical Memory size.
  return std::to_string(stol(ram)/1024);
}

string LinuxParser::Uid(int pid) { 
  string uid = GetValue("Uid:", kProcDirectory + std::to_string(pid)+kStatusFilename);
  return uid;
}

string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line, key, value, username;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> value >> key;
      if (key == uid) return username;
    }
    filestream.close();
  }
  return "";
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      values.push_back(value);
    }
    filestream.close();
  }
  if (values.size() > 21)
    return UpTime() - std::stol(values[21])/sysconf(_SC_CLK_TCK);
  else
  {
    return 0;
  }
  
}
