#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process();
  explicit Process(int pid);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;
  void refresh();

 private:
  int pid;
  float cpuUtilization;
  std::vector<std::string> cpu;
  std::string ram;
  std::string command;
  std::string user;

  float lastTotalTime;
  float lastSeconds;

  long int uptime;
};

#endif