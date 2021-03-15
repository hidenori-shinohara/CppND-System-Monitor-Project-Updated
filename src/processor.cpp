#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {}

long getNonIdle(std::vector<long> const& data) {
  long user = data[LinuxParser::CPUStats::kUser_];
  long nice = data[LinuxParser::CPUStats::kNice_];
  long system = data[LinuxParser::CPUStats::kSystem_];
  long irq = data[LinuxParser::CPUStats::kIRQ_];
  long softirq = data[LinuxParser::CPUStats::kSoftIRQ_];
  long steal = data[LinuxParser::CPUStats::kSteal_];
  long guest = data[LinuxParser::CPUStats::kGuest_];
  long guestnice = data[LinuxParser::CPUStats::kGuestNice_];
  user = user - guest;
  nice = nice - guestnice;
  return user + nice + system + irq + softirq + steal;
}

long getIdle(std::vector<long> const& data) {
  long user = data[LinuxParser::CPUStats::kUser_];
  long nice = data[LinuxParser::CPUStats::kNice_];
  long idle = data[LinuxParser::CPUStats::kIdle_];
  long iowait = data[LinuxParser::CPUStats::kIOwait_];
  long guest = data[LinuxParser::CPUStats::kGuest_];
  long guestnice = data[LinuxParser::CPUStats::kGuestNice_];
  user = user - guest;
  nice = nice - guestnice;
  return idle + iowait;
}

void Processor::refresh() {
  std::vector<std::string> const& stringVec = LinuxParser::CpuUtilization();
  std::vector<long> data;
  for (auto const& s : stringVec) {
    long val = 0;
    for (auto const& ch : s) {
      val = 10 * val + (ch - '0');
    }
    data.push_back(val);
  }
  if (data.size() < LinuxParser::CPUStats::kGuestNice_) {
    return;
  }
  if (prevData.size() < LinuxParser::CPUStats::kGuestNice_) {
    prevData = data;
    utilization = 0;
    return;
  }
  long prevIdle = getIdle(prevData);
  long idle = getIdle(data);

  long prevNonIdle = getNonIdle(prevData);
  long nonIdle = getNonIdle(data);

  long prevTotal = prevIdle + prevNonIdle;
  long total = idle + nonIdle;

  long totald = total - prevTotal;
  long idled = idle - prevIdle;

  if (totald < 1) return;

  utilization = (totald - idled) / (float)totald;

  prevData = data;
}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { return utilization; }