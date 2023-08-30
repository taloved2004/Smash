#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <ctime>
#include <string>
#include <list>
#include <memory>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <iomanip>
#include <sys/types.h>
#include "Exceptions.h"

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command
{
protected:
  int job_id;
  int process_id;
  char *cmd_l;
  bool external;
  bool time_out;
  std::vector<std::string> args_vec;

public:
  Command(const char *cmd_line);
  virtual ~Command();
  virtual void execute() = 0;
  bool isExternal() { return external; }
  bool isTimeout() { return time_out; }
  void setShared(std::shared_ptr<Command>);
  std::shared_ptr<Command> getShared();

  // getters
  const char *getCmdL() const;
  int getJobId() const;
  int getProcessId() const;

  // setters
  void setJobId(int id);
  void setProcessId(int id);

};

class BuiltInCommand : public Command
{
public:
  BuiltInCommand(const char *cmd_line);
  virtual ~BuiltInCommand() = default;
};

class ExternalCommand : public Command
{

public:
  ExternalCommand(const char *cmd_line) : Command(cmd_line) { external = true; }
  virtual ~ExternalCommand() = default;
  void execute() override;
};

class PipeCommand : public Command
{
protected:
  std::shared_ptr<Command> write_command;
  std::shared_ptr<Command> read_command;
  int standard_in_pd;
  int standard_out_pd;
  int standard_error_pd;
  int fd[2];

public:
  PipeCommand(const char *cmd_line, std::string);
  virtual ~PipeCommand() = default;
  void execute() override{};
  void execute(int);
  void prepareWrite(int);
  void prepareRead();
  void cleanUp();
};

class PipeNormalCommand : public PipeCommand
{
public:
  PipeNormalCommand(const char *cmd_line) : PipeCommand(cmd_line, "|") {}
  void execute() override;
};

class PipeSterrCommand : public PipeCommand
{
public:
  PipeSterrCommand(const char *cmd_line) : PipeCommand(cmd_line, "|&") {}
  void execute() override;
};

class RedirectionCommand : public Command
{
protected:
  // command to be redirect
  std::shared_ptr<Command> base_command;

  // the destination file
  std::string dest;

  // index of a new FD that points to the standard output
  int out_pd;

public:
  explicit RedirectionCommand(const char *cmd_line, std::string); // Command::Command(cmd_line)
  virtual ~RedirectionCommand() = default;
  void execute() override;
  void prepareGeneral(bool);
  virtual void prepare() = 0;
  void cleanup();
};

class RedirectionAppendCommand : public RedirectionCommand
{
public:
  explicit RedirectionAppendCommand(const char *cmd_line) : RedirectionCommand(cmd_line, ">>"){};
  void prepare() override;
};

class RedirectionNormalCommand : public RedirectionCommand
{
public:
  explicit RedirectionNormalCommand(const char *cmd_line) : RedirectionCommand(cmd_line, ">"){};
  void prepare() override;
};

class ChangeDirCommand : public BuiltInCommand
{
public:
  ChangeDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};
  virtual ~ChangeDirCommand() = default;
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand
{
public:
  GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};
  virtual ~GetCurrDirCommand() = default;
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand
{
private:
public:
  ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};
  virtual ~ShowPidCommand() = default;
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  QuitCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~QuitCommand() = default;
  void execute() override;
};

class JobsList
{
public:
  class JobEntry
  {
  private:
    // pointer the the command object
    std::shared_ptr<Command> command;

    // initial time
    int init_time;

    // boolean that holdes the command Status (stopped/not stopped)
    bool is_stopped;

  public:
    // C'TOR & D'TOR
    JobEntry(std::shared_ptr<Command> command, bool is_stopped) : command(command), init_time(time(NULL)), is_stopped(is_stopped){};
    ~JobEntry() = default;

    //  getters
    bool getStopped() const;
    std::shared_ptr<Command> getCommand() const;
    int getJobId() const;

    //  setters
    void setTime();
    void setStopped(bool is_stopped);

    //  aux
    // prints the info of the job according to the format in jobs command
    void printInfo() const;
  };

  std::vector<std::shared_ptr<JobEntry>> jobs;

public:
  JobsList() : jobs(){};
  ~JobsList() = default;

  //  getters
  JobEntry *getJobById(int jobId);
  JobEntry *getLastJob(int *lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  int getMaxId() const;
  bool isEmpty() const;

  //  aux
  void addJob(std::shared_ptr<Command> cmd, bool isStopped = false);
  void removeJobById(int jobId);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
};

class JobsCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  JobsCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~JobsCommand() = default;
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  ForegroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~ForegroundCommand() = default;
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  BackgroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~BackgroundCommand() = default;
  void execute() override;
};

class ChmodCommand : public BuiltInCommand
{
public:
  ChmodCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};
  virtual ~ChmodCommand() = default;
  void execute() override;
};

class GetFileTypeCommand : public BuiltInCommand
{
public:
  GetFileTypeCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};
  virtual ~GetFileTypeCommand() = default;
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  SetcoreCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~SetcoreCommand() = default;
  void execute() override;
};

class KillCommand : public BuiltInCommand
{
  // A pointer to the JobsList variable in the Smash object
  JobsList *jobs;

public:
  KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs){};
  virtual ~KillCommand() = default;
  void execute() override;
};

///------------------------------------- Bonus start---------------------------------------------

class TimeoutCommand : public BuiltInCommand
{
  int dest_time;
  int m_pid;
  std::shared_ptr<Command> target_cmd;

public:
  explicit TimeoutCommand(const char *cmd_line);
  virtual ~TimeoutCommand() = default;
  void execute() override;
  int getTime() const;
  int getTimeoutTargetPid();
};

class TimeOutList
{
private:
  int time_to_next;
  std::shared_ptr<TimeoutCommand> next_cmd;
  std::list<std::shared_ptr<TimeoutCommand>> time_out_list;

public:
  void addToList(std::shared_ptr<TimeoutCommand>);
  void removeNext();
  void makeAlarm();
  void handleSignal();
  void removedFinished();
  void removeCommand(std::shared_ptr<TimeoutCommand>);
};

/// ---------------------------------------Bonus end-----------------------------------------

//  Implemented as a Singleton design pattern
class SmallShell
{
private:
  std::string prompt;
  std::string last_wd;
  std::shared_ptr<Command> current_command;
  JobsList *jobs_list;
  TimeOutList *timeOutList;

  SmallShell();

public:
  std::shared_ptr<Command> CreateCommand(const char *cmd_line);
  SmallShell(SmallShell const &) = delete;     // disable copy ctor
  void operator=(SmallShell const &) = delete; // disable = operator
  static SmallShell &getInstance()             // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();

  //  aux
  void executeCommand(const char *cmd_line);
  std::string get_last_wd() const;
  void set_last_wd(std::string);
  void setCurrentCommand(std::shared_ptr<Command>);

  std::shared_ptr<Command> getCurrentCommand() const;
  void printPrompt() const;
  void changeChprompt(const char *cmd_line);

  void addJob(std::shared_ptr<Command> cmd, bool is_stopped = false);
  void addTimeOutCommand(std::shared_ptr<TimeoutCommand>);
  void removeTimeOutCommand(std::shared_ptr<TimeoutCommand>);

  void handleAlarm();

  void removeJob(int job_id);
};

// declaration for signals.cpp
bool isAppendRedirect(std::string cmd_str);
bool isSterrPipe(std::string cmd_str);
bool isRedirect(std::string cmd_str);
bool isPipe(std::string cmd_str);



#endif // SMASH_COMMAND_H_
