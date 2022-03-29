#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string memory_utilization;
  string line;
  string key, value, unit;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  float mem_total, mem_free;

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          mem_total = stoi(value);
        }

        if (key == "MemFree:") {
          mem_free = stoi(value);
        }
      }
    }
  }
  stream.close();

  return (mem_total - mem_free) / mem_total;
}

long LinuxParser::UpTime() {
  string line;
  string up_time_string;
  string idle_time_string;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time_string >> idle_time_string;
  }
  stream.close();

  return stoi(up_time_string);
}

long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies{0};
  std::stringstream path;
  path << kProcDirectory << pid << kStatFilename;

  std::ifstream filestream(path.str());
  if (filestream.is_open()) {
    const int position = 13;
    std::string not_needed_string;
    for (int pos = 0; pos < position; pos++) {
      if (!(filestream >> not_needed_string)) {
        return 100000;
      }
    }

    long user_time_ticks{0}, user_children_time_ticks{0};
    long kernel_time_ticks{0}, kernel_children_time_ticks{0};

    if (filestream >> user_time_ticks >> kernel_time_ticks >>
        user_children_time_ticks >> kernel_children_time_ticks) {
      active_jiffies = (user_time_ticks + kernel_time_ticks +
                        user_children_time_ticks + kernel_children_time_ticks);
    }
  }
  return active_jiffies / sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies() {
  std::vector<long> cpu_jffies = CpuUtilization();

  return cpu_jffies[kUser_] + cpu_jffies[kNice_] + cpu_jffies[kSystem_] +
         cpu_jffies[kIRQ_] + cpu_jffies[kSoftIRQ_] + cpu_jffies[kSteal_];
}

long LinuxParser::IdleJiffies() {
  std::vector<long> cpu_jffies = CpuUtilization();
  return cpu_jffies[kIdle_] + cpu_jffies[kIOwait_];
}

vector<long> LinuxParser::CpuUtilization() {
  std::vector<long> cpu_jffies;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string cpu;
    filestream >> cpu;
    if (cpu == "cpu") {
      for (int i = 0; i < CPUStates::End; i++) {
        long value;
        filestream >> value;
        cpu_jffies.push_back(value);
      }
    }
  }

  return cpu_jffies;
}

int LinuxParser::TotalProcesses() {
  int total_processes;
  string line, key, value;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          total_processes = stoi(value);
        }
      }
    }
  }

  return total_processes;
}

int LinuxParser::RunningProcesses() {
  int running_processes;
  string line, key, value;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          running_processes = stoi(value);
        }
      }
    }
  }

  return running_processes;
}

string LinuxParser::Command(int pid) {
  std::string command = " ";
  std::stringstream path;
  path << kProcDirectory << pid << kCmdlineFilename;

  std::ifstream filestream(path.str());
  if (filestream.is_open()) {
    std::string line;
    if (std::getline(filestream, line)) {
      command = line;
    }
  }
  return command;
}

string LinuxParser::Ram(int pid) {
  std::stringstream path;
  path << kProcDirectory << pid << kStatusFilename;

  // used VmRSS instead of VmSize to be more accurate
  int ram_kb = ReadStringValueFromFile<int>(path.str(), "VmRSS:");
  std::stringstream ram_mb;
  ram_mb << (ram_kb / 1000);

  return ram_mb.str();
}

string LinuxParser::Uid(int pid) {
  std::stringstream path;
  path << kProcDirectory << pid << kStatusFilename;

  return ReadStringValueFromFile<std::string>(path.str(), "Uid:");
}

string LinuxParser::User(int pid) {
  std::string user_id = Uid(pid);

  string user_name = " ";
  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      std::string id;
      std::string filler;
      if (linestream >> user_name >> filler >> id) {
        if (id == user_id) {
          return user_name;
        }
      }
    }
  }
  return user_name;
}

long LinuxParser::UpTime(int pid) {
  long up_time = 0;

  std::stringstream path;
  path << kProcDirectory << pid << kStatFilename;

  std::ifstream filestream(path.str());
  if (filestream.is_open()) {
    const int relevant_position = 22;
    std::string value;
    for (int pos = 0; pos < relevant_position; pos++) {
      if (!(filestream >> value)) {
        return 100000;
      }
    }
    up_time = std::stol(value);
  }
  return up_time / sysconf(_SC_CLK_TCK);
}

template <typename T>
T LinuxParser::ReadStringValueFromFile(const std::string& path,
                                       const std::string& search_string) {
  T value;
  std::ifstream filestream(path);

  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream line_stream(line);
      std::string argument;
      line_stream >> argument;
      if (argument == search_string) {
        line_stream >> value;
        return value;
      }
    }
  }
  return value;
}
