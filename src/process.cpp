#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p) : pid(p) { refresh(); }

float parse(std::string s) {
  std::stringstream ss;
  ss << s;
  float a = 0;
  ss >> a;
  return a;
}

void Process::refresh() {
  {
    vector<string> const& cpu = LinuxParser::ProcessLevelCpuUtilization(pid);
    using namespace LinuxParser;
    float uptime = parse(cpu[kUptime_]);
    float utime = parse(cpu[kUtime_]);
    float stime = parse(cpu[kStime_]);
    float cutime = parse(cpu[kCutime_]);
    float cstime = parse(cpu[kCstime_]);
    float starttime = parse(cpu[kStarttime_]);
    float hertz = sysconf(_SC_CLK_TCK);
    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    float total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    float seconds = uptime - (starttime / hertz);
    cpuUtilization = std::max<float>(0, (total_time / hertz) / seconds);
  }

  { command = LinuxParser::Command(pid); }

  {
    std::stringstream ss;
    ss << (long long)(parse(LinuxParser::Ram(pid)) / 1024);
    ram = ss.str();
  }

  { user = LinuxParser::User(pid); }

  { uptime = LinuxParser::UpTime(pid) / sysconf(_SC_CLK_TCK); }
}

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization; }

// TODO: Return the command that generated this process
string Process::Command() { return command; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}