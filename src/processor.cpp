#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // Guest time is already accounted in usertime
  user = user - guest;
  nice = nice - guest_nice;
  long long idleall = idle + iowait;
  long long systemall = system + irq + softirq;
  long long virtall = guest + guest_nice;
  long long total = user + nice + systemall + idleall + steal + virtall;
  return (total - idleall) / (float)total;
}