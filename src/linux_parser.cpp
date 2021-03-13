#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  // This calculates the same number as the green bar in htop.
  // https://stackoverflow.com/a/41251290
  std::vector<std::string> names{"MemTotal", "MemFree",      "Buffers",
                                 "Cached",   "SReclaimable", "Shmem"};
  std::map<std::string, long long> values;

  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::stringstream ss;
      ss << line;
      std::string title, unit;
      long long number;
      if (ss >> title >> number >> unit) {
        for (auto const& name : names) {
          if (title == (name + ":")) {
            values[name] = number;
          }
        }
      }
    }
  }
  if (values.size() != names.size()) return 0.0;
  long long totalUsedMemory = values["MemTotal"] - values["MemFree"];
  long long cachedMemory =
      values["Cached"] + values["SReclaimable"] - values["Shmem"];
  long long nonCacheBufferMemory =
      totalUsedMemory - (values["Buffers"] + cachedMemory);
  return nonCacheBufferMemory / (float)values["MemTotal"];
}

#include <cassert>
#include <iostream>

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  float totalUptime, idleTime;
  filestream >> totalUptime >> idleTime;
  return (long)totalUptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::ProcessLevelCpuUtilization(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kStatFilename;
  std::ifstream filestream(ss.str());
  vector<string> v;
  if (filestream.is_open()) {
    for (int i = 0; i < 30; i++) {
      std::string s;
      filestream >> s;
      v.push_back(s);
    }
  }
  return v;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string word;
    while (filestream >> word) {
      if (word == "cpu") {
        vector<string> v(10);
        for (int i = 0; i < 10; i++) {
          v.push_back("");
          filestream >> v[i];
        }
        return v;
      }
    }
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string word;
    while (filestream >> word) {
      if (word == "processes") {
        int n;
        filestream >> n;
        return n;
      }
    }
  }
  return -1;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string word;
    while (filestream >> word) {
      if (word == "procs_running") {
        int n = -1;
        filestream >> n;
        return n;
      }
    }
  }
  return -1;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kCmdlineFilename;
  std::ifstream filestream(ss.str());
  if (filestream.is_open()) {
    std::string result;
    std::getline(filestream, result);
    if (result.empty()) result = "No Command";
    return result;
  }
  return "No command info found";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(ss.str());
  if (filestream.is_open()) {
    std::string result;
    while (filestream >> result) {
      if (result == "VmSize:") {
        filestream >> result;
        return result;
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kStatusFilename;
  std::ifstream filestream(ss.str());
  if (filestream.is_open()) {
    std::string result;
    while (filestream >> result) {
      if (result == "Uid:") {
        filestream >> result;
        return result;
      }
    }
  }
  return "Error: No Uid found";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  string expectedUid = LinuxParser::Uid(pid);
  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::stringstream liness;
      for (int i = 0; i < (int)line.size(); i++) {
        liness << (line[i] == ':' ? ' ' : line[i]);
      }
      std::string user, garbage, uid;
      liness >> user >> garbage >> uid;
      if (uid == expectedUid) {
        return user;
      }
    }
  }
  return "Error: No Uid found";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kStatFilename;
  std::ifstream filestream(ss.str());
  long totalUptime = LinuxParser::UpTime();
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    int cnt = 0;
    for (int i = 0; i < (int)line.size(); i++) {
      if (line[i] == '(') cnt++;
      if (line[i] == ')') {
        cnt--;
        if (cnt == 0) {
          i++;
          std::stringstream content;
          while (i < (int)line.size()) {
            content << line[i];
            i++;
          }
          std::string result;
          for (i = 0; i < 19; i++) {
            content >> result;
          }
          long starttime;
          content >> starttime;
          return totalUptime - starttime / sysconf(_SC_CLK_TCK);
        }
      }
    }
  }
  return -1;
}