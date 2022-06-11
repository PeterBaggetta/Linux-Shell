To Compile:
    make all

To clean all .o and executable files:
    make clean
    - it will ask for confirmation of the files that are to be cleaned, type
      'y' to remove them or 'n' to keep them in the directory

To run the program:
    - type ./myShell
    - this must be done AFTER typeing 'make all' in the terminal

NOTES BEFORE LAUNCHING SHELL:
    - ensure the '.CIS3110_history' are in the directory of execution (aka. where the files are), the history file
      will be generated when the program starts if it is not present
    - .CIS3110_profile could not be implemented into this shell, the instructions seemed unclear of how to format
      the use of this file, if you want to add a path then use the export command

Files:
    - Makefile
        > contains the code used to make compilation easy
    - myHeader.h
        > contains the header files and declaration of functions in main
    - myShell.c
        > contains the code that makes up the shell
    - text.txt
        > contains a scrambled alphabet (used when testing sort with redirection)

Functionalities:
    SHELL commands:
        - ls
            > lists all the files in the current directory and displays information about them
            > FLAGS: Seach up the relevant flags for this command on the linux man page, any
                     comination of the flags ie. ls -lta will work
        - ps
            > lists all the curently running processes on the system
            > FLAGS: Seach up the relevant flags for this command on the linux man page
        - pwd
            > FLAGS: Seach up the relevant flags for this command on the linux man page
        - cd
            > changes the directory of execution
            > usage:
                * 'cd ~'         : changes the current directory to the home directory
                * 'cd ..'        : changes the directory to the directory currently above
                * 'cd .'         : will do nothing (aka; stays in current directory)
                * 'cd 'name'     : changed the directory to 'name', where 'name' is a path
                                   if is it a vaiable path
        - export
            > if no commands are entered after 'export', the program will print all variables in the
              directory of users
            > if there is a command after the export like ('export myPATH=/some/path:/etc'), the desired environment
              variable will be set to its respective value by the user
        - echo
            > if an echo is called with the $myPATH, $myHOME, or $myHISTFILE, the path of the respective
              variable will be shown in the terminal
        - sort
            > requires a redirect from a file ie ('sort < filename')
        - redirects ('<' and '>')
            > entering a command with the '<' will redirect from a file, ie (sort < filename) will
              sort the content in the file alphabetically and output to stdout
            > entering a command with the '>' will redirect the output to a file, ie (ls -lt > filename)
              will take the output of the command 'ls -lt' and write the content into the file
        - pipes
            > pipeing a command will take the output of the first command and make it the input of the 
              second command (only maximum of 1 pipe implemented). Look on the linux man page for more info
        - history
            > lists the history of the commands that have been entered into the shell
            > usage:
                * 'history'   : lists all the commands entered into the shell
                * 'history -c': clears the history file
                * "history n" : outputs the last 'n' commands from history, where 'n' is a real
                                number (if the number exceeds the history it will print all history)
        - Environment vairables (myPATH / myHOME / myHISTFILE)
            > if the user is going to set the environment variable to a different path, they must run a command
              like 'export myPATH=/disired:/path/of:/choice' to set the respective environment
            > cannot assign environment vairables values without exporting them
              (NO USE OF 'myPATH=/disired:/path/of:/choice' then 'export $myPATH' separately)
        - exit
            > either of these commands entered will exit the myShell program and kill all running children

WARNING OF USE:
    - this program does not work if there are not spaces between commands with redirects, or pipes
    - when using the 'cd' command, ensure the path of choice does not have any spaces within it, the
      only space key allowed is after 'cd' (ie. cd home/path/path)
