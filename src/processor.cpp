#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // Guest time is already accounted in usertime
  long user = data[LinuxParser::CPUStates::kUser_];
  long nice = data[LinuxParser::CPUStates::kNice_];
  long system = data[LinuxParser::CPUStates::kSystem_];
  long idle = data[LinuxParser::CPUStates::kIdle_];
  long iowait = data[LinuxParser::CPUStates::kIOwait_];
  long irq = data[LinuxParser::CPUStates::kIRQ_];
  long softirq = data[LinuxParser::CPUStates::kSoftIRQ_];
  long steal = data[LinuxParser::CPUStates::kSteal_];
  long guest = data[LinuxParser::CPUStates::kGuest_];
  long guestnice = data[LinuxParser::CPUStates::kGuestNice_];
  user = user - guest;
  nice = nice - guestnice;
  long long idleall = idle + iowait;
  long long systemall = system + irq + softirq;
  long long virtall = guest + guestnice;
  long long total = user + nice + systemall + idleall + steal + virtall;
  return (total - idleall) / (float)total;
}