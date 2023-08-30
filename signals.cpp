#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sched.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <iomanip>
#include "Commands.h"
#include <signal.h>
#include <sys/types.h>
#include <memory>
#include <thread>
#include <errno.h>
using namespace std;

void ctrlCHandler(int sig_num)
{
  //  print massage
  std::cout << "smash: got ctrl-C" << std::endl;

  //  get current command
  SmallShell &smash = SmallShell::getInstance();
  shared_ptr<Command> current_command = smash.getCurrentCommand();
  if (current_command != nullptr)
  {
    std::string cmd_line(current_command->getCmdL());

    // we assume ctrl C will not be sent in Special Commands
    if (!(isPipe(cmd_line) || isRedirect(cmd_line) || isSterrPipe(cmd_line) || isAppendRedirect(cmd_line)))
    {
      //  send kill signal to the process
      int pid = current_command->getProcessId();
      if (kill(pid, SIGKILL) == -1)
      {
        SystemCallFailed e("kill");
        throw e;
      }
      else
      {
//          if (current_command->isTimeout())
//          {
//              pid = getpid();
//          }
        //  if the job was in the job list once, remove it
        if (current_command->getJobId() != -1)
        {
          smash.removeJob(current_command->getJobId());
        }

        //  update current command

        smash.setCurrentCommand(nullptr);

        //  prints massage
        std::cout << "smash: process " << pid << " was killed" << std::endl;
      }
    }
  }
}

void ctrlZHandler(int sig_num)
{
  //  print massage
  std::cout << "smash: got ctrl-Z" << std::endl;

  //  get current command
  SmallShell &smash = SmallShell::getInstance();
  std::shared_ptr<Command> current_command = smash.getCurrentCommand();
  if (current_command != nullptr)
  {

    // we assume ctrl Z will not be sent in Special Commands
    std::string cmd_line(current_command->getCmdL());
    if (!(isPipe(cmd_line) || isRedirect(cmd_line) || isSterrPipe(cmd_line) || isAppendRedirect(cmd_line)))
    {
      //  send kill signal
      int pid = current_command->getProcessId();
      if (kill(pid, SIGSTOP) == -1)
      {
        SystemCallFailed e("kill");
        throw e;
      }
      else
      {
//          if (current_command->isTimeout())
//          {
//              pid = getpid();
//          }

        //  add to job list
        smash.addJob(current_command, true);

        //  update current command
        smash.setCurrentCommand(nullptr);

        //  prints massage
        std::cout << "smash: process " << pid << " was stopped" << std::endl;
      }
    }
  }
}

///-------------------------bonus start---------------------------------------
void alarmHandler(int sig_num)
{
  cout << "smash: got an alarm" << endl;
  SmallShell &smash = SmallShell::getInstance();
  smash.handleAlarm();
}

///------------------------bonus end---------------------------------------
