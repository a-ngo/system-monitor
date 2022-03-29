#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int id) : m_id(id) {
  m_user_name = LinuxParser::User(m_id);
  m_command = LinuxParser::Command(m_id);
}

int Process::Id() { return this->m_id; }

float Process::GetCpuUtilization() const { return this->m_cpu_utilization; }

float Process::CpuUtilization() {
  const int up_time = LinuxParser::UpTime();
  const int total_time_active = LinuxParser::ActiveJiffies(m_id);
  const int process_up_time = LinuxParser::UpTime(m_id);

  const float total_time = up_time - process_up_time;

  this->m_cpu_utilization = total_time_active / total_time;
  return this->m_cpu_utilization;
}

string Process::Command() { return this->m_command; }

string Process::Ram() { return LinuxParser::Ram(this->m_id); }

string Process::User() { return this->m_user_name; }

long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(this->m_id); }

bool Process::operator>(Process const& a) const {
  return this->m_cpu_utilization > a.m_cpu_utilization;
}