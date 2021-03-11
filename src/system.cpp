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

Processor::Processor() {}

Processor::Processor(std::vector<string> stringVec) {
  std::vector<long> cpuUtilization;
  for (auto const& s : stringVec) {
    long val = 0;
    for (auto const& ch : s) {
      val = 10 * val + (ch - '0');
    }
    cpuUtilization.push_back(val);
  }
  user = cpuUtilization[0];
  nice = cpuUtilization[1];
  system = cpuUtilization[2];
  idle = cpuUtilization[3];
  iowait = cpuUtilization[4];
  irq = cpuUtilization[5];
  softirq = cpuUtilization[6];
  steal = cpuUtilization[7];
  guest = cpuUtilization[8];
  guest_nice = cpuUtilization[7];
}

// TODO: Return the system's CPU
Processor& System::Cpu() {
  cpu_ = Processor(LinuxParser::CpuUtilization());
  return cpu_;
}

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