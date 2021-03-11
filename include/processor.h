#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor();
  Processor(std::vector<std::string>);
  float Utilization();  // TODO: See src/processor.cpp

 private:
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
};
#endif