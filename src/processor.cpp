#include "processor.h"

#include <iostream>

#include "linux_parser.h"

float Processor::Utilization() {
  // get jiffies
  long int total = LinuxParser::Jiffies();
  long int idle = LinuxParser::IdleJiffies();
  // calc difference to last update step
  long int diffIdle = idle - this->prevIdle_;
  long int diffTotal = total - this->prevTotal_;
  // re-assign for next iteration
  this->prevIdle_ = idle;
  this->prevTotal_ = total;
  float utilization = static_cast<float>(diffTotal - diffIdle) / diffTotal;
  return utilization;
}