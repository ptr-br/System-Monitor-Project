#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long int prevTotal_;
  long int prevIdle_;
};

#endif