#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
// #include "Exeptions.h"

extern char *strsignal(int sig);

int main(int argc, char *argv[])
{
    if (signal(SIGTSTP, ctrlZHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if (signal(SIGINT, ctrlCHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-C handler");
    }

    struct sigaction new_action;
    new_action.sa_handler = &alarmHandler;
    new_action.sa_flags = SA_RESTART;
    // sigemptyset(&new_action.sa_mask); // שני השורות האלה פותרים בעיה שיש בvalgrind
    // sigaddset(&new_action.sa_mask, SIGINT);
    if (sigaction(SIGALRM, &new_action, NULL) < 0)
        perror("smash error: failed to set alarm");

    // TODO: setup sig alarm handler

    SmallShell &smash = SmallShell::getInstance();
    while (true)
    {
        smash.printPrompt();
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        try
        {
            smash.executeCommand(cmd_line.c_str());
        }
        catch (SystemCallFailed &e)
        {
            perror(e.what());
        }
        catch (std::exception &e)
        {
            std::cerr << e.what()<<std::endl;
        }
    }

    return 0;
}
