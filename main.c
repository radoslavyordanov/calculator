#include "shunting-yard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void show_error(Status status, FILE *fp);

int main(int argc, char *argv[]) {
    FILE *fp = stdout;
    if (argc > 1) {
        fp = fopen(argv[1], "w+");
    }



    while(1) {
        int     fd[2], nbytes;
        pid_t   childpid;
        char    readbuffer[200];

        pipe(fd);

        if((childpid = fork()) == -1) {
            perror("fork");
            exit(1);
        }

        if(childpid == 0)  {
                /* Child process closes up input side of pipe */
            close(fd[0]);

                /* Send "string" through the output side of pipe */
            char buff[200];
            fgets (buff, 200, stdin);
            write(fd[1], buff, (strlen(buff)+1));
            exit(0);
        }
        else {
                /* Parent process closes up output side of pipe */
            close(fd[1]);

                /* Read in a string from the pipe */
            nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
            double result = 0.0;
            Status status = shunting_yard(readbuffer, &result);
            if (status != OK) {
                show_error(status, fp);
            } else {
                fprintf(fp,"%.14g\n", result);
                fflush(fp);
            }

        }
    }

    if (fp != stdout) {
        fclose(fp);
    } 

    return EXIT_SUCCESS;
}

void show_error(Status status, FILE *fp) {
    char *message = NULL;
    switch (status) {
        case ERROR_SYNTAX:
        message = "Syntax error";
        break;
        case ERROR_OPEN_PARENTHESIS:
        message = "Missing parenthesis";
        break;
        case ERROR_CLOSE_PARENTHESIS:
        message = "Extra parenthesis";
        break;
        case ERROR_UNRECOGNIZED:
        message = "Unknown character";
        break;
        case ERROR_NO_INPUT:
        message = "Empty expression";
        break;
        case ERROR_UNDEFINED_FUNCTION:
        message = "Unknown function";
        break;
        case ERROR_FUNCTION_ARGUMENTS:
        message = "Missing function arguments";
        break;
        case ERROR_UNDEFINED_CONSTANT:
        message = "Unknown constant";
        break;
        default:
        message = "Unknown error";
    }
    fprintf(fp, "%s\n", message);
    fflush(fp);
}
