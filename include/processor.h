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
  std::vector<long> data;
};
#endif