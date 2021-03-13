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
  int Pid() const;                         // TODO: See src/process.cpp
  std::string User() const;                // TODO: See src/process.cpp
  std::string Command() const;             // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  std::string Ram() const;                 // TODO: See src/process.cpp
  long int UpTime() const;                 // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  void refresh();

  // TODO: Declare any necessary private members
 private:
  int pid;
  float cpuUtilization;
  std::vector<std::string> cpu;
  std::string ram;
  std::string command;
  std::string user;

  long double last_total_time;
  long double last_seconds;

  long int uptime;
};

#endif