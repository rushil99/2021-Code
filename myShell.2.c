//Rushil Mistry
//ysv968

#include <stdio.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <stdlib.h>


char SHELL_NAME[50] = "Shell";
int QUIT = 0;

char *readLine()
{
	char *line = (char *)malloc(sizeof(char) * 1024); 
	char c;
	int pos = 0, bufsize = 1024;
	if (!line) 
	{
		printf("\nBuffer Error.");
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		c=getchar();
		if (c == EOF || c == '\n') 
		{
			line[pos] = '\0';
			return line;
		}
		else
		{
			line[pos] = c;
		}
		pos ++;
		if (pos >= bufsize)
		{
			bufsize += 1024;
			line = realloc(line, sizeof(char) * bufsize);
			if (!line)
			{
			printf("\nBuffer Error.");
			exit(EXIT_FAILURE);
			}
		}
	}

}

char **splitLine(char *line)
{
	char **tokens = (char **)malloc(sizeof(char *) * 64);
	char *token;
	char delim[10] = " \t\n\r\a;";
	int pos = 0, bufsize = 64;
	if (!tokens)
	{
		printf("\nBuffer Error.");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, delim);
	while (token != NULL)
	{
		tokens[pos] = token;
		pos ++;
		if (pos >= bufsize)
		{
			bufsize += 64;
			line = realloc(line, bufsize * sizeof(char *));
			if (!line)
			{
			printf("\nBuffer Allocation Error.");
			exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[pos] = NULL;
	return tokens;
}

int myShell_exit();
int myShell_cd(char **args);

char *builtin_cmd[] = {"cd", "quit"};

int (*builtin_func[]) (char **) = {&myShell_cd, &myShell_exit}; 

int numBuiltin() 
{
	return sizeof(builtin_cmd)/sizeof(char *);
}

int myShell_cd(char **args)
{
	if (args[1] == NULL) 
	{
		printf("myShell: expected argument to \"cd\"\n");
	} 
	else 
	{
		if (chdir(args[1]) != 0) 
		{
			perror("myShell: ");
		}
	}
	return 1;
}

int myShell_exit()
{
	QUIT = 1;
	return 0;
}

int myShellLaunch(char **args)
{
	pid_t pid, wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
		{
			perror("myShell: ");
		}
	exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("myShell: ");
	}
	else
	{
	do 
	{
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}
int execShell(char **args)
{
	int ret;
	if (args[0] == NULL)
	{
	
		return 1;
	}
	for (int i=0; i< numBuiltin(); i++) 
	{
		if(strcmp(args[0], builtin_cmd[i])==0) 
			return (*builtin_func[i])(args); 
	}
	ret = myShellLaunch(args);
	return ret;
}

int myShellInteract()
{
	char *line;
	char **args;
	while(QUIT == 0)
	{
		printf("%s> ", SHELL_NAME);
		line=readLine();
		args=splitLine(line);
		execShell(args);
		free(line);
		free(args);
	}
	return 1;
}

int myShellScript(char filename[100])
{
	printf("Received, Opening %s", filename);
	FILE *fptr;
	char line[200];
	char **args;
	fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		printf("\nUnable to open file.");
		return 1;
	}
	else
	{
		printf("\nFile Opened.");
		while(fgets(line, sizeof(line), fptr)!= NULL)
		{
			printf("\n%s", line);
			args=splitLine(line);
			execShell(args);
		}
	}
	free(args);
	fclose(fptr);
	return 1;
}

int main(int argc, char **argv)
{
	if (argc == 1)
		myShellInteract();
	else if (argc == 2)
		myShellScript(argv[1]);
	else
		printf("\nInvalid Number of Arguments");

	return EXIT_SUCCESS;
}
