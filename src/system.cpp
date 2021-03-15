#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

#include <iostream>

void System::refresh() {
  std::map<int, Process> processMap;
  for (auto process : processes_) {
    processMap[process.Pid()] = process;
  }
  processes_.clear();
  std::vector<int> const& pids = LinuxParser::Pids();
  for (auto pid : pids) {
    if (processMap.find(pid) != processMap.end()) {
      processes_.push_back(processMap[pid]);
    } else {
      processes_.push_back(Process(pid));
    }
  }
  for (auto& process : processes_) {
    process.refresh();
  }
  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
  cpu_.refresh();
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }