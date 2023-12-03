#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {}

  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
};

#endif