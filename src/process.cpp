#include "process.h"

#include <unistd.h>

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() const { return this->pid_; }

float Process::CpuUtilization() const {
  const int pid = this->Pid();
  float totalTime = static_cast<float>(LinuxParser::ActiveJiffies(pid));
  long int seconds = LinuxParser::UpTime(pid);
  float cpuLoad = ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);
  return cpuLoad;
}

string Process::Command() { return LinuxParser::Command(this->Pid()); }

string Process::Ram() const { return LinuxParser::Ram(this->Pid()); }

string Process::User() { return LinuxParser::User(this->Pid()); }

long int Process::UpTime() { return LinuxParser::UpTime(this->Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return stol(Ram()) < stol(a.Ram());
}

/*bool Process::operator<(const Process& other) const {
    float thisCpuUtilization = this->CpuUtilization();
    float otherCpuUtilization = other.CpuUtilization();

    if (std::isnan(thisCpuUtilization)) {
        return !std::isnan(otherCpuUtilization); // nan is considered greater
than any other value } else if (std::isnan(otherCpuUtilization)) { return false;
// other is nan, so this is considered smaller } else if
(std::isinf(thisCpuUtilization) && thisCpuUtilization > 0) { return false; //
this is +inf, considered greater than any finite value } else if
(std::isinf(otherCpuUtilization) && otherCpuUtilization > 0) { return true; //
other is +inf, considered smaller than any finite value } else if
(std::isinf(thisCpuUtilization) && thisCpuUtilization < 0) { return true; //
this is -inf, considered smaller than any finite value } else if
(std::isinf(otherCpuUtilization) && otherCpuUtilization < 0) { return false; //
other is -inf, considered greater than any finite value } else {
        // Neither is nan or inf, perform the regular comparison
        return thisCpuUtilization < otherCpuUtilization;
    }
} */