#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"
#include "process.h"
#include "processor.h"

/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

Processor& System::Cpu() { return m_cpu; }

std::vector<Process>& System::Processes() {
  std::set<int> unique_ids;
  for (Process process : m_processes) {
    unique_ids.insert(process.Id());
  }

  std::vector<int> ids = LinuxParser::Pids();
  for (int id : ids) {
    if (unique_ids.find(id) == unique_ids.end()) {
      m_processes.emplace_back(Process(id));
    }
  }

  for (Process& process : m_processes) {
    process.CpuUtilization();
  }

  std::sort(m_processes.begin(), m_processes.end(), std::greater<Process>());

  // alternatively via lambda
  // std::sort(m_processes.begin(), m_processes.end(), [] (const Process& a, const Process& b) {
  //   return a.GetCpuUtilization() > b.GetCpuUtilization();
  // }
  // );

  return m_processes;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
