#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "config.h"
#include "types.h"
#include "cli.h"
#include "commands.h"

CLICOMMAND builtInCmds[] =
{
#ifdef __INCLUDE_BUILTINS
    { "dir", "list external commands", listFilesCmd },
#endif
    { "exit", "logout", logoutCmd },
#ifdef __INCLUDE_BUILTINS
    { "help", "list internal commands", helpCmd },
#endif
    { "logout", "logout", logoutCmd },
    { "", "", NULL }
};

int logout;

void catFile(char *fName)
{
    FILE *fp;
    char *catBuff;
    int readed;

    catBuff = malloc(CATBUFFSIZE);
    if(catBuff == NULL)
    {
        printf("malloc() error!\n");
        return;
    }

    fp = fopen(fName, "r");
    if(fp == NULL)
    {
        return;
    }

    do
    {
        readed = fread(catBuff, 1, 64, fp);
        fwrite(catBuff, 1, readed, stdout); 
    }
    while(readed == 64);
    fflush(stdout);

    free(catBuff);

    fclose(fp);
}

void helpCmd(char *param)
{
    CLICOMMAND *cmd;

    catFile(HELP);

    cmd = builtInCmds;
    while(cmd -> name[0] != '\0')
    {
        printf("  %-16s %s\n", cmd -> name, cmd -> helpStr);
        cmd++;
    }
}

void logoutCmd(char *param)
{
    logout = 1;
}

void listFilesCmd(char *ptr)
{
    DIR *dp;
    struct dirent *dEnt;
    struct stat statBuff;
    struct tm *tmTime;
    char strBuff[300];
    int fileCount;
    unsigned long int totalSize;

    dp = opendir(BINDIR);
    if(dp == NULL)
    {
        printf("Can't open directory\n");
        return;
    }

    printf("Directory listing of \"%s\"\n", BINDIR);

    fileCount = 0;
    totalSize = 0;

    do
    {
        dEnt = readdir(dp);
        if(dEnt != NULL)
        {
            if(strcmp(dEnt -> d_name, ".") != 0 &&
               strcmp(dEnt -> d_name, "..") != 0)
            {
                sprintf(strBuff, "%s/%s", BINDIR, dEnt -> d_name);
                stat(strBuff, &statBuff);

                if(dEnt -> d_type == DT_REG)
                {
                    fileCount++;
                    totalSize = totalSize + statBuff.st_size;

                    tmTime = localtime(&statBuff.st_mtime);
                    strftime(strBuff, 80, "%d/%m/%y %H:%M  ", tmTime);

                    printf("  %s %16ld %s\n",
                                    strBuff, statBuff.st_size, dEnt -> d_name);
                }
            }
        }
    }
    while(dEnt != NULL);

    printf("%16d File(s)     %ld bytes\n\n", fileCount, totalSize);

    closedir(dp);
}

void showMotd()
{
    catFile(MOTD);
}

void parseCommand(char *cmdLine)
{
    CLICOMMAND *cmd;
    int gotCmd;
    char *extPath;
    char *fullCmd;
    FILE *fp;
    char *cmdName;
    char *paramPtr;

    cmdName = strtok(cmdLine, " ");
    paramPtr = strtok(NULL, " ");

    cmd = builtInCmds;
    gotCmd = 0;
    while(cmd -> name[0] != '\0' && gotCmd == 0)
    {
        if(strcmp(cmd -> name, cmdName) == 0)
        {
            gotCmd = 1;
            cmd -> fn(paramPtr);
        }
        cmd++;
    }

    if(gotCmd == 0)
    {
       extPath = malloc(strlen(BINDIR) + strlen(cmdName) + 2);
       sprintf(extPath, "%s/%s", BINDIR, cmdName);
       fp = fopen(extPath, "r");
       if(fp == NULL)
       {
           fullCmd = extPath;
           printf("Command not found\n");
       }
       else
       {
           fclose(fp);
           if(paramPtr)
           {
               fullCmd = malloc(strlen(extPath) + strlen(paramPtr) + 2);
               sprintf(fullCmd, "%s %s", extPath, paramPtr);

               free(extPath);
           }
           else
           {
               fullCmd = extPath;
           }

           system(fullCmd);
       }

       free(fullCmd);
    }
}

int main(int argc, char *argv[])
{
    char *ln;

    showMotd();

    printf("\n\n");
    printf("%s\n", WELCOME);
#ifdef __INCLUDE_BUILTINS
    printf("'?' to list built-in commands\n");
#endif

    logout = 0;
    while(logout == 0)
    {
        ln = readline(PROMPT);

        switch(*ln)
        {
            case '\0':;
                break;

            case '?':
                helpCmd(NULL);
                break;

            default:
                parseCommand(ln);
        }

        free(ln);
    }

}
