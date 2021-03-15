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

Process::Process() : last_total_time(0), last_seconds(0) {}
Process::Process(int p) : pid(p), last_total_time(0), last_seconds(0) {
  refresh();
}

float parse(std::string s) {
  std::stringstream ss;
  ss << s;
  float a = 0;
  ss >> a;
  return a;
}

#include <iostream>

void Process::refresh() {
  {
    using namespace LinuxParser;
    vector<string> const& cpu = LinuxParser::ReadStatFile(pid);
    if (cpu.size() < kStarttime_) return;  // this process is dead.
    long double system_uptime = LinuxParser::UpTime();
    long double utime = parse(cpu[kUtime_ - 1]);
    long double stime = parse(cpu[kStime_ - 1]);
    long double cutime = parse(cpu[kCutime_ - 1]);
    long double cstime = parse(cpu[kCstime_ - 1]);
    long double starttime = parse(cpu[kStarttime_ - 1]);
    long double hertz = sysconf(_SC_CLK_TCK);
    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    long double total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    long double seconds = system_uptime - (starttime / hertz);
    if (last_seconds < 1) {
      last_seconds = seconds;
      last_total_time = total_time;
      cpuUtilization = 0;
    } else if (seconds - last_seconds > 1) {
      long double total_time_diff = total_time - last_total_time;
      float seconds_diff = seconds - last_seconds;
      last_total_time = total_time;
      last_seconds = seconds;
      cpuUtilization = (total_time_diff / hertz) / seconds_diff;
    }
  }

  { uptime = LinuxParser::UpTime(pid); }

  { command = LinuxParser::Command(pid); }

  {
    std::stringstream ss;
    ss << (long long)(parse(LinuxParser::Ram(pid)) / 1024);
    ram = ss.str();
  }

  { user = LinuxParser::User(pid); }
}

// TODO: Return this process's ID
int Process::Pid() const { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization; }

// TODO: Return the command that generated this process
string Process::Command() const { return command; }

// TODO: Return this process's memory utilization
string Process::Ram() const { return ram; }

// TODO: Return the user (name) that generated this process
string Process::User() const { return user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}