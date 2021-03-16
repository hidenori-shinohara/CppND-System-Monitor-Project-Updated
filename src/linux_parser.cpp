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
  return "No OS info Found";
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    if (linestream >> os >> version >> kernel) {
      return kernel;
    }
  }
  return "No kernel info found";
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

// Read and return the system memory utilization
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
  if (values.size() != names.size()) {
    // Was not able to read some properties.
    // Since the calculation will not be correct,
    // return 0.
    return 0.0;
  }
  const long long totalUsedMemory = values["MemTotal"] - values["MemFree"];
  const long long cachedMemory =
      values["Cached"] + values["SReclaimable"] - values["Shmem"];
  const long long nonCacheBufferMemory =
      totalUsedMemory - (values["Buffers"] + cachedMemory);
  return nonCacheBufferMemory / (float)values["MemTotal"];
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  float totalUptime, idleTime;
  if (filestream >> totalUptime >> idleTime) {
    return (long)totalUptime;
  }
  return 0;
}

vector<string> LinuxParser::ReadStatFile(int pid) {
  std::stringstream ss;
  ss << LinuxParser::kProcDirectory << pid << LinuxParser::kStatFilename;
  std::ifstream filestream(ss.str());
  std::string line;
  std::getline(filestream, line);
  std::stringstream content;
  for (int i = 0; i < (int)line.size(); i++) {
    content << line[i];
    if (line[i] == '(') {
      // a counter to keep track of (# of open parentheses) - (# of closed
      // parentheses).
      int cnt = 0;
      for (; i < (int)line.size(); i++) {
        if (line[i] == '(') {
          cnt++;
        }
        if (line[i] == ')') {
          cnt--;
          if (cnt == 0) break;
        }
      }
    }
  }
  vector<string> ret;
  std::string result;
  while (content >> result) {
    ret.push_back(result);
  }
  return ret;
}

vector<string> LinuxParser::ProcessLevelCpuUtilization(int pid) {
  return LinuxParser::ReadStatFile(pid);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string word;
    while (filestream >> word) {
      if (word == "cpu") {
        vector<string> v(10);
        for (int i = 0; i < 10; i++) {
          if (!(filestream >> v[i])) {
            // Couldn't read.
            // Something went wrong.
            return {};
          }
        }
        return v;
      }
    }
  }
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string word;
    while (filestream >> word) {
      if (word == "processes") {
        int n;
        if (filestream >> n) {
          return n;
        } else {
          return -1;
        }
      }
    }
  }
  return -1;
}

// Read and return the number of running processes
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

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::stringstream ss;
  ss << kProcDirectory << pid << kCmdlineFilename;
  std::ifstream filestream(ss.str());
  if (filestream.is_open()) {
    std::string result;
    std::getline(filestream, result);
    if (result.empty()) return "No Command";
    return result;
  }
  return "No command info found";
}

// Read and return the memory used by a process
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

// Read and return the user ID associated with a process
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

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  string const expectedUid = LinuxParser::Uid(pid);
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

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  vector<string> v = LinuxParser::ReadStatFile(pid);
  if (v.size() < 22) {
    return -1;
  }
  long starttime;
  {
    std::stringstream ss;
    ss << v[21];
    ss >> starttime;
  }
  const long totalUptime = LinuxParser::UpTime();
  return totalUptime - starttime / sysconf(_SC_CLK_TCK);
}