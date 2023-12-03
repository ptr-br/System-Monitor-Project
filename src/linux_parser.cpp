#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
vector<T> readValues(const string& path, const string& argumentName) {
  vector<T> values;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream lineStream(line);
      T value;
      if (argumentName == "") {
        while (lineStream >> value) {
          values.push_back(value);
        }
      } else {
        string argument;
        lineStream >> argument;
        if (argument == argumentName) {
          while (lineStream >> value) {
            values.push_back(value);
          };
        }
      }
    }
  }
  return values;
}

template <typename T>
T readValue(const string& path, const string& argumentName) {
  T value;
  std::ifstream filestream(path);

  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream lineStream(line);

      if (argumentName == "") {
        T argument;
        lineStream >> argument;
        return argument;
      }
      string argument;
      lineStream >> argument;
      if (argument == argumentName) {
        lineStream >> value;
        return value;
      }
    }
  }
  return value;
}

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

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Update  to use std::filesystem
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

float LinuxParser::MemoryUtilization() {
  const string path = kProcDirectory + kMeminfoFilename;
  // calc memory utilization
  const long int totalMemory = readValue<long int>(path, "MemTotal:");
  const long int freeMemory = readValue<long int>(path, "MemFree:");
  const float usedMemory = totalMemory - freeMemory;
  // check zero divison and return -1 if no memory is available at all
  return totalMemory == 0.0 ? -1.0 : (usedMemory / totalMemory);
}

long LinuxParser::UpTime() {
  const string path = kProcDirectory + kUptimeFilename;
  const long int upTime = readValue<long int>(path, "");
  return upTime;
}

long LinuxParser::Jiffies() {
  const long int active_jiffies = ActiveJiffies();
  const long int idle_jiffies = IdleJiffies();
  return active_jiffies + idle_jiffies;
}

long LinuxParser::ActiveJiffies(int pid) {
  const string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  const vector<string> jiffies = readValues<string>(path, "");
  return std::stol(jiffies[13]) + std::stol(jiffies[14]);
}

long LinuxParser::ActiveJiffies() {
  const vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStates::kUser_]) +
         std::stol(jiffies[CPUStates::kNice_]) +
         std::stol(jiffies[CPUStates::kSoftIRQ_]) +
         std::stol(jiffies[CPUStates::kSteal_]) +
         std::stol(jiffies[CPUStates::kSystem_]) +
         std::stol(jiffies[CPUStates::kIRQ_]);
}

long LinuxParser::IdleJiffies() {
  const vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStates::kIdle_]) +
         std::stol(jiffies[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  const string path = kProcDirectory + kStatFilename;
  const vector<string> cpu_utilizations = readValues<string>(path, "cpu");
  return cpu_utilizations;
}

int LinuxParser::Processes(string procs_name) {
  string line;
  string key;
  string value;
  int intValue;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == procs_name) {
        try {
          intValue = std::stoi(value);
        } catch (const std::invalid_argument& e) {
          std::cerr << "Unable to convert " << e.what()
                    << " to an integer. Please check LinuxParser::Processes.";
          intValue = -2;
        }
        return intValue;
      }
    }
  }
  return -1;
}

string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command;
}

string LinuxParser::Ram(int pid [[maybe_unused]]) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> value;
          return to_string(stol(value) / 1024);
        }
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string line;
  std::ostringstream dirStrStream;
  dirStrStream << kProcDirectory << pid << kStatusFilename;
  const string uid = readValue<string>(dirStrStream.str(), "Uid:");
  return uid;
}

string LinuxParser::User(int pid) {
  string line;
  string userName, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> userName >> x >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return userName;
        }
      }
    }
  }
  return LinuxParser::Uid(pid);  // at least return pid if no user is found
}

long LinuxParser::UpTime(int pid) {
  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  const vector<string> cpu_utilizations = readValues<string>(path, "");
  return LinuxParser::UpTime() -
         (std::stol(cpu_utilizations[21]) / sysconf(_SC_CLK_TCK));
  ;
}
