#ifndef _MYHEADER_H
#define _MYHEADER_H

/* header files used in program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <setjmp.h>

/* Read user command from terminal */
int getCommand (char **cmd, int lineNumber, char starting_dir[]);

/* Append the commands onto the history file */
int appendHistory (char command_line[], int lineNumber, char starting_dir[]);

/* Print full history */
void printHistory (char starting_dir[]);

/* Print the history with a limited number of lines */
void printHistoryLim (char starting_dir[], int count);

/* Check which command is to be executed */
int isCommand (char **cmd);

/* To create processes */
void exeCommandsIn (char **cmd, char *pathValue, char homeDirectory[]);

/* Execute commands with execv */
int doCommand (char **cmd, char *pathValue, char homeDirectory[]);

/* Execute commands without execv */
void exeCommands (char **cmd, char starting_dir[], char *path, char *home);

/* Cd function for the linux 'cd' command (change directory) */
void changeDir (char **cmd, char starting_dir[]);

/* Create new directory like linux 'mkdir' (make new directory) */
void makeDirectory (char **cmd);

/* Change the environment variables */
void exportCmd (char **cmd);

#endif