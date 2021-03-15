#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor();
  void refresh();
  float Utilization();  // TODO: See src/processor.cpp

 private:
  std::vector<long> prevData;
  float utilization;
};
#endif