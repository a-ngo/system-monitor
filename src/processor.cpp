#include "processor.h"

#include "linux_parser.h"

Processor::Processor()
    : m_idle_jiffies(LinuxParser::IdleJiffies()),
      m_active_jiffies(LinuxParser::ActiveJiffies()),
      m_total_jiffies(m_idle_jiffies + m_active_jiffies) {}

float Processor::Utilization() {
  const long current_idle_jiffies = LinuxParser::IdleJiffies();
  const long current_active_jiffies = LinuxParser::ActiveJiffies();
  const long current_total_jiffies = LinuxParser::Jiffies();

  const long delta_idle_jiffies = current_idle_jiffies - this->m_idle_jiffies;
  const long delta_total_jiffies =
      current_total_jiffies - this->m_total_jiffies;

  this->m_idle_jiffies = current_idle_jiffies;
  this->m_active_jiffies = current_active_jiffies;
  this->m_total_jiffies = current_total_jiffies;

  return delta_total_jiffies == 0 ? 0.0f
                                  : (delta_total_jiffies - delta_idle_jiffies) *
                                        1.0 / delta_total_jiffies;
}