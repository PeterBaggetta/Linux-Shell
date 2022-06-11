/* Assignment 1
 * Pietro Baggetta
 * 1092160
 * Sources used in code
    - Lecture code
    - Textbook code
    - GeeksForGeeks.com
    - Linux man pages
    - Communicating between processes (using pipes) in C by CodeVault [https://www.youtube.com/watch?v=Mqb2dVRe0uo]
*/

#include "myHeader.h"

#define HISTORYNAME ".CIS3110_history"

/*** Global Variables ***/
int output = 1;             /* output redirect is present (0 = no, 1 = yes) */
int input = 1;              /* input redirect is present (0 = no, 1 = yes) */
int outputAndInput = 1;     /* output and input redirect is present (0 = no, 1 = yes) */
int ampersand = 1;          /* if ampersand is present then put it background (0 = no, 1 = yes) */
int checkForPipe = 1;       /* if pipe is present then execute the pipe commands (0 = no, 1 = yes) */
char outputFile[50];        /* holds the output redirect file name */
char inputFile[50];         /* holds the input redirect file name */
char *commandBefore[50];    /* holds the command(s) before the pipe */
char *commandAfter[50];     /* holds the command(s) after the pipe */
char userInput[100];        /* temp variable for user input */
int backgroundCounter = 0;  /* counter to tell a loop when it is done processing the background process */

/* This struct stores the background process and its commands */
typedef struct
{
    pid_t child;
    char command[100];

} pid_struct;

pid_struct my_pid_array[50];

/* Read user command from terminal */
int getCommand (char **cmd, int lineNumber, char starting_dir[]) {
    char command_line[1025], pipedString[1025], *pipe[1025];
    int i = 0, count = 0, readhistory = 0;

    /* read the command from the terminal */
    fgets (command_line, 100, stdin);

    /* remove \n for better use */
    strtok (command_line, "\n");
    strcpy (pipedString, command_line);
    strcpy (userInput, command_line);

    /* if the user wants to clear the history file */
    if (!strcmp (userInput, "history -c")){
        readhistory = appendHistory (command_line, 0, starting_dir);
    } else {
        /* add new command to the history file */
        readhistory = appendHistory (command_line, lineNumber, starting_dir);
    }
    
    /* split and store words in array */
    char *ptr = strtok(command_line, " ");

    while (ptr != NULL) {

        if (strncmp (ptr, ">", 1) == 0 && input == 0) {

            output = 1;
            ptr = strtok (NULL, " ");
            strcpy (outputFile, ptr);

        } else if (strncmp (ptr, "<", 1) == 0 && output == 0) {

            input = 1;
            ptr = strtok (NULL, " ");
            strcpy (inputFile, ptr);

        } else if (strncmp (ptr, ">", 1) == 0 && input == 1) {

            outputAndInput = 1;
            ptr = strtok (NULL, " ");
            strcpy (outputFile, ptr);

        } else if (strncmp (ptr, "<", 1) == 0 && output == 1) {

            outputAndInput = 1;
            ptr = strtok (NULL, " ");
            strcpy (inputFile, ptr);

        } else if (strncmp (ptr, "&", 1) == 0) {

            ampersand = 1;

        } else if (strncmp (ptr, "|", 1) == 0) {

            checkForPipe = 1;

        } else {

            cmd[count] = ptr;
            count++;

        }
        ptr = strtok (NULL, " ");
    }

    cmd[count] = NULL;

    return readhistory;
}

/* Append the commands onto the history file */
int appendHistory (char command_line[], int lineNumber, char starting_dir[]) {

    char cwd[1025];

    getcwd(cwd, sizeof (cwd));

    if (strcmp (starting_dir, cwd) != 0) {
        chdir (starting_dir);
    }

    FILE *historyfile = fopen (HISTORYNAME, "a");

    if (historyfile == NULL) {
        printf("ERROR opening file!\n");
        exit(1);
    }
    
    fprintf (historyfile, " %d  ", lineNumber);
    fprintf (historyfile, "%s\n", command_line);

    lineNumber++;
    fclose (historyfile);

    chdir (cwd);
    return lineNumber;
}

/* Print full history */
void printHistory (char starting_dir[]) {

    char cwd[1025], line[1025], *status;

    getcwd (cwd, sizeof(cwd));

    if (strcmp (starting_dir, cwd) != 0) {
        chdir (starting_dir);
    }

    FILE *historyfile = fopen (HISTORYNAME, "r");
    status = fgets (line, sizeof (line), historyfile);

    while (status != NULL) {
        printf ("%s", line);
        status = fgets(line, sizeof (line), historyfile);
    }

    fclose (historyfile);
    chdir (cwd);
}

/* Print the history with a limited number of lines */
void printHistoryLim (char starting_dir[], int count) {

    char cwd[1025], line[1025], *status;
    int total_lines = 0;

    getcwd (cwd, sizeof(cwd));

    if (strcmp (starting_dir, cwd) != 0) {
        chdir (starting_dir);
    }

    FILE *historyfile = fopen (HISTORYNAME, "r");
    status = fgets (line, sizeof (line), historyfile);

    while (status != NULL) {
        total_lines++;
        status = fgets (line, sizeof(line), historyfile);
    }

    rewind (historyfile);
    total_lines = total_lines - count;

    while (total_lines-- > 0) {
        status = fgets(line, sizeof (line), historyfile);
        if (status == NULL) {
            printf ("ERROR!\n");
            return;
        }
    }

    status = fgets(line, sizeof (line), historyfile);

    while (status != NULL) {
        printf ("%s", line);
        status = fgets(line, sizeof (line), historyfile);
    }

    fclose (historyfile);
    chdir (cwd);
}

/* Check which command is to be executed */
int isCommand (char **cmd) {

    if ( (!strcmp (cmd[0], "cd")) || (!strcmp (cmd[0], "mkdir")) || (!strcmp (cmd[0], "history")) || (!strcmp (cmd[0], "export")) || (!strcmp (cmd[0], "echo")) ) {
        return 0;
    } else if (checkForPipe == 1) {
        return 2;
    } else {
        return 1;
    }
}

/* To create processes */
void exeCommandsIn (char **cmd, char *pathValue, char homeDirectory[]) {
    pid_t pid;
    int status = 0;
    
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork Failed");
        exit(-1);
    } else if (pid == 0) {

        if (output == 1) {
            FILE *fp;
            fp = freopen (outputFile, "w", stdout);
        }
        if (input == 1) {
            FILE *fp1;
            fp1 = freopen (inputFile, "r", stdin);
        }
        if (outputAndInput == 1) {
            FILE *fp1;
            FILE *fp;
            fp = freopen (outputFile, "w", stdout);
            fp1 = freopen (inputFile, "r", stdin);
        }
  
        status = doCommand (cmd, pathValue, homeDirectory);

        if (!ampersand) {
            if (status < 0) {
                printf ("Command failed\n");
            }
        }
        exit (status);
    } else {
        if (!ampersand) {
            waitpid (pid, &status, 0);
        } else {
            
            my_pid_array->child = getpid();
            strcpy (my_pid_array->command, cmd[0]);
            waitpid (my_pid_array->child, &status, WNOHANG);
           
            backgroundCounter++;
            printf ("[%d] %d\n", backgroundCounter, getpid());
        }
    }
}

/* Execute commands with execv */
int doCommand (char **cmd, char *pathValue, char homeDirectory[]) {

    int status = 0, path_num = 0, i = 0;
    char path[PATH_MAX], path_list[100][PATH_MAX];

    char *ptr;
    ptr = strtok (pathValue, ":");
    
    while (ptr != NULL) {
        strcpy (path_list[path_num], ptr);
        ptr = strtok (NULL, ":");
        path_num++;
    }

    /* Check every file in directory for command */
    while (i < path_num) {
        
        strcpy (path,path_list[i]);
        strcat (path, "/");
        strcat (path, cmd[0]);
        status = execv (path, cmd);
        i++;
    }

    return status;
}

/* Execute commands without execv */
void exeCommands (char **cmd, char *starting_dir, char *path, char *home) {

    if (!strcmp (cmd[0], "cd")) {
        changeDir (cmd, starting_dir);
    } else if (!strcmp (cmd[0], "mkdir")) {
        makeDirectory (cmd);
    } else if (!strcmp (cmd[0], "history")) {
        if (cmd[1] == NULL) {
            printHistory (starting_dir);
        } else {
            if (!strcmp (cmd[1], "-c")) {
                FILE *fp = fopen (HISTORYNAME, "w");
                fclose (fp);
            } else {
                int num = atoi (cmd[1]);
                printHistoryLim (starting_dir, num);
            }
        }
    } else if (!strcmp (cmd[0], "export")) {
        exportCmd (cmd);
    } else if(!strcmp (cmd[0], "echo")){

        if (!strcmp (cmd[1], "$myPATH")) {
            printf ("%s\n", getenv (path));
        } else if (!strcmp (cmd[1], "$myHOME")) {
            printf ("%s\n", getenv (home));
        }
    } else {
        printf ("> command not found\n");
    }
}

/* Cd function for the linux 'cd' command (change directory) */
void changeDir (char **cmd, char starting_dir[]) {
    int done;

    if (!strcmp (cmd[1], "~") ) {
        done = chdir (starting_dir);
    } else {
        done = chdir (cmd[1]);
    }
    if (done == -1) {
        printf ("No such file or directory\n");
    }
}

/* Create new directory like linux 'mkdir' (make new directory) */
void makeDirectory (char **cmd) {

    char *dir_name = cmd[1];
    int created = 0;

    created = mkdir (dir_name, 0777);

    if (created == 1) {
        printf ("Failed to create directory\n");
    } else {
        printf ("Directory created\n");
    }
}

/* Change the environment variables */
void exportCmd (char **cmd) {

    char string[1025], env[50];

    strcpy (string, cmd[1]);

    char *ptr = strtok (string, "=");
    strcpy (env, ptr);
    while (ptr != NULL) {
        strcpy (string, ptr);
        ptr = strtok (NULL, "=");
    }

    if ( (!strcmp (env, "myPATH")) || (!strcmp (env, "myHOME")) || (!strcmp (env, "myHISTFILE")) ) {
        setenv ("PATH", string, 1);
    } else {
        printf ("ERROR, environment variable\n");
    }
}

int main (int argc, char *argv[]) {

    char **cmd = malloc(sizeof(char) * 1025);
    char **cmd1 = malloc(sizeof(char) * 100);
    char **cmd2 = malloc(sizeof(char) * 100);
    int typeOfCommand;
    int *count = malloc(sizeof(int));
    char cwd[1025];
    int lineNumber = 1;


    /* create and set envionment variables */
    char *path = "PATH";
    char *pathValue =  malloc (sizeof(char) * PATH_MAX);
    char *hist = "HISTFILE";
    char *histValue = HISTORYNAME;
    char *home = "HOME";
    char homeDirectory[1025];

    /* get the starting home directory */
    getcwd (homeDirectory, sizeof(homeDirectory));

    /* set path for proper use (just in case) */
    strcpy (pathValue, "/usr/bin:/bin:");
    strcat (pathValue, homeDirectory);

    /* set the environment variables */
    setenv (path, pathValue, 1);
    setenv (hist, histValue, 1);
    setenv (home, homeDirectory, 1);

    /* create and/or clear the history file */
    FILE *fp = fopen (HISTORYNAME, "w");
    fclose(fp);

    while (1) {
        output = 0, input = 0, ampersand = 0, checkForPipe = 0, outputAndInput = 0;

        /* print out the completed process after ampersand is done in background */
        pid_t childWait;
        int i = 0;

        while (childWait = waitpid (-1, NULL, WNOHANG) > 0) {
            printf ("[%d]+ %d Done \t%s \n", i+1, my_pid_array->child, my_pid_array->command);
            backgroundCounter = 0;
            i++;
        }
      
        /* clear the output and input file variables */
        memset (outputFile, 0, sizeof(outputFile));
        memset (inputFile, 0, sizeof(inputFile));

        /* print out directory and start the shell */
        printf("%s> ", getcwd (cwd, sizeof(cwd)));

        /* get the user input from command line (stdin) */
        lineNumber = getCommand (cmd, lineNumber, getenv("HOME"));

        /* exit the shell */
        if (!strcmp (cmd[0], "exit")) {
            free (cmd);
            free (count);

            exit (0);
        } else {
            /* check if the command is executable with execv */
            typeOfCommand = isCommand (cmd);

            if (typeOfCommand == 1) {
                /* create the process */
                exeCommandsIn (cmd, pathValue, homeDirectory);
            } else if (typeOfCommand == 2) {
                /* pipeing code */
                /******* This code was implemented from CodeVault on YouTube and changed to better fit this assignment *******/

                char *buf1 = malloc (sizeof(char) * 100);
                char *buf2 = malloc (sizeof(char) * 100);
                char *temp = malloc (sizeof(char) * 100);
                strcpy(temp, userInput);
                int i = 0, j = 0;

                /* setparate string by the pipe */
                buf1 = strsep(&temp, "|");

                char *p1 = strtok(buf1, " ");

                /* go through input and check for cases (> and/or <) */
                while (p1 != NULL) {
                    if (strncmp (p1, ">", 1) == 0) {

                        output = 1;
                        p1 = strtok (NULL, " ");
                        p1 = strtok (NULL, " ");

                    } else if (strncmp (p1, "<", 1) == 0) {

                        input = 1;
                        p1 = strtok (NULL, " ");
                        p1 = strtok (NULL, " ");

                    } else if (strncmp (p1, ">", 1) == 0 && input == 1) {

                        outputAndInput = 1;
                        p1 = strtok (NULL, " ");
                        p1 = strtok (NULL, " ");

                    } else if (strncmp (p1, "<", 1) == 0 && output == 1) {

                        outputAndInput = 1;
                        p1 = strtok (NULL, " ");
                        p1 = strtok (NULL, " ");

                    } else {

                        commandBefore[i] = p1;
                        i++;
                        p1 = strtok (NULL, " ");
                    }
                }

                commandBefore[i] = NULL;

                /* get the second set of commands after pipe */
                buf2 = strsep (&temp, "|");

                char *ptr2 = strtok (buf2, " ");

                while (ptr2 != NULL) {
                    if (strncmp (ptr2, ">", 1) == 0) {

                        output = 1;
                        ptr2 = strtok (NULL, " ");
                        ptr2 = strtok (NULL, " ");

                    } else if (strncmp (ptr2, "<", 1) == 0) {

                        input = 1;
                        ptr2 = strtok (NULL, " ");
                        ptr2 = strtok (NULL, " ");

                    } else if (strncmp (ptr2, ">", 1) == 0 && input == 1) {

                        outputAndInput = 1;
                        ptr2 = strtok (NULL, " ");
                        ptr2 = strtok (NULL, " ");

                    } else if (strncmp (ptr2, "<", 1) == 0 && output == 1) {

                        outputAndInput = 1;
                        ptr2 = strtok (NULL, " ");
                        ptr2 = strtok (NULL, " ");

                    } else {

                        commandAfter[j] = ptr2;
                        j++;
                        ptr2 = strtok (NULL, " ");
                    }
                }
                commandAfter[j] = NULL;

                /* set the pipe process */
                int fd[2];
                if (pipe(fd) == -1) {
                    perror ("pipe failed");
                    return 1;
                }

                /* create the first child for the piping */
                int child1 = fork();
                if (child1 < 0) {
                    perror ("fork failed");
                    return 2;
                }

                if (child1 == 0) {
                    if (output == 1) {
                        /* open output file so stdout is redirected */
                        freopen(outputFile, "w", stdout);
                    }
                    if (input == 1) {
                        /* open input file so stdin is redirected */
                        freopen(inputFile, "r", stdin);
                    }
                    if (outputAndInput == 1) {
                        /* open output and input files */
                        freopen (outputFile, "w", stdout);
                        freopen (inputFile, "r", stdin);
                    }

                    /* copy stdout */
                    dup2 (fd[1], STDOUT_FILENO);
                    close (fd[0]);
                    close (fd[1]);
                    
                    /* run the commands from the first pipe */
                    doCommand (commandBefore, pathValue, homeDirectory);
                }

                /* create the second child for piping */
                int child2 = fork();
                if (child2 < 0) {
                    perror ("fork failed");
                    return 3;
                }

                if (child2 == 0) {
                    if (output == 1) {
                        /* open output file so stdout is redirected */
                        freopen (outputFile, "w", stdout);
                    }
                    if (input == 1) {
                        /* open input file so stdin is redirected */
                        freopen (inputFile, "r", stdin);
                    }
                    if (outputAndInput == 1) {
                        freopen (outputFile, "w", stdout);
                        freopen (inputFile, "r", stdin);
                    }

                    /* copy stdin */
                    dup2 (fd[0], STDIN_FILENO);
                    close (fd[0]);
                    close (fd[1]);

                    /* run the commands from the second pipe */
                    doCommand (commandAfter, pathValue, homeDirectory);
                  
                }

                /* close file descriptors then wait for process to finish */
                close (fd[0]);
                close (fd[1]);
                waitpid (child1, NULL, 0);
                waitpid (child2, NULL, 0);
            } else {
                /* execute commands with no pipe */
                exeCommands (cmd, getenv ("HOME"), path, home);
            }
        }
    }
    free (cmd);
    free (count);

    return 0;
}
