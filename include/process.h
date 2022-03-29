#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int id);
  int Id();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator>(Process const& a) const;
  float GetCpuUtilization() const;

 private:
  int m_id;
  std::string m_user_name;
  std::string m_command;
  float m_cpu_utilization;
};

#endif