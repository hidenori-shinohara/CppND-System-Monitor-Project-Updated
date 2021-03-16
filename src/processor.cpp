#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {}

long getNonIdle(std::vector<long> const& data) {
  const long user = data[LinuxParser::CPUStats::kUser_];
  const long nice = data[LinuxParser::CPUStats::kNice_];
  const long system = data[LinuxParser::CPUStats::kSystem_];
  const long irq = data[LinuxParser::CPUStats::kIRQ_];
  const long softirq = data[LinuxParser::CPUStats::kSoftIRQ_];
  const long steal = data[LinuxParser::CPUStats::kSteal_];
  const long guest = data[LinuxParser::CPUStats::kGuest_];
  const long guestnice = data[LinuxParser::CPUStats::kGuestNice_];
  return user + nice + system + irq + softirq + steal - guest - guestnice;
}

long getIdle(std::vector<long> const& data) {
  const long idle = data[LinuxParser::CPUStats::kIdle_];
  const long iowait = data[LinuxParser::CPUStats::kIOwait_];
  return idle + iowait;
}

void Processor::refresh() {
  std::vector<std::string> const& cpuUtilizationInString =
      LinuxParser::CpuUtilization();
  std::vector<long> data;
  for (auto const& s : cpuUtilizationInString) {
    data.push_back(std::stol(s));
  }
  if (data.size() < LinuxParser::CPUStats::kGuestNice_) {
    // Something went wrong with parsing.
    // Return without modifying anything.
    return;
  }
  if (prevData.size() < LinuxParser::CPUStats::kGuestNice_) {
    // prevData is not usable. (e.g., The first time refresh() is called.)
    // Thus update prevData and move on.
    prevData = data;
    utilization = 0;
    return;
  }
  const long prevIdle = getIdle(prevData);
  const long idle = getIdle(data);

  const long prevNonIdle = getNonIdle(prevData);
  const long nonIdle = getNonIdle(data);

  const long prevTotal = prevIdle + prevNonIdle;
  const long total = idle + nonIdle;

  const long totald = total - prevTotal;
  const long idled = idle - prevIdle;

  if (totald < 1) {
    // Enough time hasn't passed to determine the current CPU usage.
    return;
  }

  utilization = (totald - idled) / (float)totald;

  prevData = data;
}

// Return the aggregate CPU utilization
float Processor::Utilization() { return utilization; }