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

Process::Process(int p) { pid = p; }

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    vector<string> const& cpu = LinuxParser::ProcessLevelCpuUtilization(pid);
    using namespace LinuxParser;
    float uptime = std::stoi(cpu[kUptime_]);
    float utime = std::stoi(cpu[kUtime_]);
    float stime = std::stoi(cpu[kStime_]);
    float cutime = std::stoi(cpu[kCutime_]);
    float cstime = std::stoi(cpu[kCstime_]);
    float starttime = std::stoi(cpu[kStarttime_]);
    float hertz = sysconf(_SC_CLK_TCK);
    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    float total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    float seconds = uptime - (starttime / hertz);
    return (total_time / hertz) / seconds;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// TODO: Return this process's memory utilization
string Process::Ram() {
    long long ram = std::stoi(LinuxParser::Ram(pid));
    std::stringstream ss;
    ss << (ram / 1024);
    return ss.str();
}

// TODO: Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }