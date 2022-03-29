#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();

 private:
  long m_idle_jiffies;
  long m_active_jiffies;
  long m_total_jiffies;
};

#endif