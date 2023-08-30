#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <iostream>
#include <string>

class UnspecifiedError : public std::exception
{
private:
  std::string error_str;

public:
  UnspecifiedError(std::string error_line) : error_str("smash error:> \"" + error_line + "\"") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

class InvaildArgument : public std::exception
{
private:
  std::string error_str;

public:
  InvaildArgument(std::string error_type) : error_str("smash error: " + error_type + ": invalid arguments") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

class TooManyArguments : public std::exception
{
  std::string error_str;

public:
  TooManyArguments(std::string error_type) : error_str("smash error: " + error_type + ": too many arguments") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct SystemCallFailed : public std::exception
{
  std::string error_str;

public:
  SystemCallFailed(std::string error_type) : error_str("smash error: " + error_type + " failed") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

class OldPWDNotSet : public std::exception
{
  std::string error_str;

public:
  OldPWDNotSet() : error_str("smash error: cd: OLDPWD not set") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct JobIdDoesntExist : public std::exception
{
  std::string error_str;

public:
  JobIdDoesntExist(std::string error_type, int job_id) : error_str("smash error: " + error_type + ": job-id " + std::to_string(job_id) + " does not exist") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct JobsListEmpty : public std::exception
{
  std::string error_str;

public:
  JobsListEmpty() : error_str("smash error: fg: jobs list is empty") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct JobAlreadyRunning : public std::exception
{
  std::string error_str;

public:
  JobAlreadyRunning(int job_id) : error_str("smash error: bg: job-id " + std::to_string(job_id) + " is already running in the background") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct NoStoppedJobs : public std::exception
{
  std::string error_str;

public:
  NoStoppedJobs() : error_str("smash error: bg: there is no stopped jobs to resume") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct InvaildCoreNumber : public std::exception
{
  std::string error_str;

public:
  InvaildCoreNumber() : error_str("smash error: setcore: invalid core number") {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};

struct DefaultError : public std::exception
{
  std::string error_str;

public:
  DefaultError(const char *cmd_l) : error_str("smash error: " + std::string(cmd_l)) {}
  const char *what() const noexcept
  {
    return error_str.c_str();
  }
};


#endif