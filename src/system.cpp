#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  this->processes_.clear();
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process p(pid);
    this->processes_.push_back(p);
  }
  // sort proccesses
  std::sort(processes_.rbegin(), processes_.rend());

  return this->processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() {
  return LinuxParser::Processes("procs_running");
}

int System::TotalProcesses() { return LinuxParser::Processes("processes"); }

long int System::UpTime() { return LinuxParser::UpTime(); }
