#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99


FILE *fp; // file struct for stdin
char **tokens;
char *line;
int flag = 0;
pid_t pid;
char **buff;



void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

}
int join(i)
{
	int fd1[2];
	int pid1;
	//int fd2[2];
	if (pipe(fd1)== -1){
		perror("pipe");
		exit(1);
	}
	if (pid == 0)
	{
		dup2(fd1[0],0);
		close(fd1[0]);
		close(fd1[1]);
		execlp(tokens[i-1], tokens[i-1], (char *)0);
		//printf("excvp done \n");
		if( execlp("ps", "ps", "-elf", (char *) 0) < 0) 
		{
		perror("Child");
		exit(0);
		}
	}
	if ( (pid1 = fork()) < 0) {
	perror("fork");
	exit(1);
    }
	printf(" value of pid2 %d \n", pid1);
    if ( pid1 == 0 ) {
        dup2(fd1[1],1); 
		close(fd1[0]);
		close(fd1[1]); 
		printf(" I am in join - part2\n");
		if( read(fd1[0], tokens[0], 6) <= 0 ) {
		perror("Parent");
		exit(1);}
        //close(fd1[0]);
		execlp(tokens[i+1], tokens[i+1],NULL);  

        if( execlp("less", "less", (char *) 0) < 0) 
		{
		perror("Child");
		exit(0);
		}
    }

}

void check(){
	int newfd1, newfd2;
	if(pid == 0){
		//printf("here again \n");
	for(int i=0; tokens[i] !=NULL; i++){
		if(strcmp(tokens[i],">")==0){
			newfd1 = open(tokens[i+1], O_WRONLY|O_CREAT, 0666);
			dup2(newfd1,1);
			execvp(tokens[0], tokens);
			close(newfd1);
		}
		if (strcmp(tokens[i],"<")==0)
		{
			newfd2=open(tokens[i+1],O_RDONLY , 0666);
			dup2(newfd2,0);
			execvp(tokens[0], tokens);
			close(newfd2);
		}
		if (strcmp(tokens[i],"|")==0)
		{
			//printf("Pid = %d \n", pid);
			join(i);
			printf("excvp done %s\n",buff[0]);

		}
		
	}
	}
}



void child_process()
{
	
	int status;
	int ret;
	pid = fork();
 
	if (pid < 0) { 
		perror("fork failed:"); 
		exit(1); 
	} 

	if (pid == 0) { // Child executes this block
		//printf("This is the child\n"); 
		flag = 1;
		//exit(99); 
	} 

	if (pid > 0) { //Parent executes this block
		//printf("This is parent. The child is %d\n", pid); 

		ret = waitpid(pid, &status, 0);
		if (ret < 0) {
			perror("waitpid failed:");
			exit(2); 
		}
	}


}



void tokenize (char * string)
{
	int token_count = 0;
	int size = MAX_TOKENS;
	char *this_token;

	while ( (this_token = strsep( &string, " \t\v\f\n\r")) != NULL) {

		if (*this_token == '\0') continue;

		tokens[token_count] = this_token;

		//printf("Token %d: %s\n", token_count, tokens[token_count]);

		token_count++;

		// if there are more tokens than space ,reallocate more space
		if(token_count >= size){
			size*=2;

			assert ( (tokens = realloc(tokens, sizeof(char*) * size)) != NULL);
		}
	}
}

void read_command() 
{

	// getline will reallocate if input exceeds max length
	assert( getline(&line, &MAX_LINE_LEN, fp) > -1); 

	printf("Shell read this line: %s\n", line);

	tokenize(line);
}

int run_command() {

	child_process();
	if (flag == 1)
	{
		check();
		int ret = execvp(tokens[0], tokens);
	}
	if (strcmp( tokens[0], EXIT_STR ) == 0)
		return EXIT_CMD;

	return UNKNOWN_CMD;

}

int main()
{
	initialize();

	do {
		printf("mysh> ");
		read_command();
		
	} while( run_command() != EXIT_CMD );

	return 0;
}