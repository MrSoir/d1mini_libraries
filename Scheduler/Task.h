#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>

#include <StaticFunctions.h>

class Task{
public:
  virtual int getType() const = 0;

  virtual bool operator==(const Task& ie) const = 0;

  virtual bool shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const = 0;
  virtual unsigned long getStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const = 0;
  virtual unsigned long getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const = 0;
  virtual unsigned long timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const = 0;
  
  virtual String toArduinoString() const = 0;
  virtual void print() const;
  virtual void println() const;

  int type = -1;
  bool started = false;
};


#endif
