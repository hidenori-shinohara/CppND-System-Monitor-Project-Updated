#include <unistd.h>
#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process() : lastTotalTime(0), lastSeconds(0) {}
Process::Process(int p) : pid(p), lastTotalTime(0), lastSeconds(0) {
  refresh();
}

void Process::refresh() {
  auto floatParser = [](std::string const& input) {
    std::stringstream ss;
    ss << input;
    float result{-1};
    ss >> result;
    return result;
  };

  {
    using namespace LinuxParser;
    vector<string> const& cpu = LinuxParser::ReadStatFile(pid);
    if (cpu.size() < kStarttime_) return;  // this process is dead.
    const float systemUptime = LinuxParser::UpTime();
    const float utime = floatParser(cpu[kUtime_ - 1]);
    const float stime = floatParser(cpu[kStime_ - 1]);
    const float cutime = floatParser(cpu[kCutime_ - 1]);
    const float cstime = floatParser(cpu[kCstime_ - 1]);
    const float starttime = floatParser(cpu[kStarttime_ - 1]);
    const float hertz = sysconf(_SC_CLK_TCK);
    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    const float totalTime = utime + stime + cutime + cstime;
    const float seconds = systemUptime - (starttime / hertz);
    if (lastSeconds < 1) {
      lastSeconds = seconds;
      lastTotalTime = totalTime;
      cpuUtilization = 0;
    } else if (seconds - lastSeconds > 1) {
      const float totalTimeDiff = totalTime - lastTotalTime;
      const float secondsDiff = seconds - lastSeconds;
      lastTotalTime = totalTime;
      lastSeconds = seconds;
      cpuUtilization = (totalTimeDiff / hertz) / secondsDiff;
    }
  }

  { uptime = LinuxParser::UpTime(pid); }

  { command = LinuxParser::Command(pid); }

  {
    std::stringstream ss;
    ss << (long long)(floatParser(LinuxParser::Ram(pid)) / 1024);
    ram = ss.str();
  }

  { user = LinuxParser::User(pid); }
}

// Return this process's ID
int Process::Pid() const { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization; }

// Return the command that generated this process
string Process::Command() const { return command; }

// Return this process's memory utilization
string Process::Ram() const { return ram; }

// Return the user (name) that generated this process
string Process::User() const { return user; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}