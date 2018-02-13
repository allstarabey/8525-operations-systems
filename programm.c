
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int num_commands(char* c)
{
    int i,counter=0;

    for( i=0; i < strlen(c); i++)     //we find how the commands will be executed.
        {
            if (c[i] == ';') counter++;     //; exists
            if ((c[i] == '&') && (c[i+1] == '&')) counter++;  //&& exists
        }
    return counter ;
}

int version(char* c)
{
    int i;

    for( i=0; i < strlen(c); i++)     //we find how the commands will be executed.
        {
            if (c[i] == ';') return 1;;     //; exists
            if ((c[i] == '&') && (c[i+1] == '&')) return 2;  //&& exists
        }
    return 0;
}
void write_my_name(){

	printf("platias_8525>");
	return;
}

char *read_line(int mode,FILE **fp)
{
  char *line = NULL;
  size_t bufsize = 0; // have getline allocate a buffer for us
  if (mode ==1){//interactive mode {
    getline(&line, &bufsize, stdin);
  }else{ //batchfile mode
    getline(&line, &bufsize, *fp);
    
  }

  return line;
}

#define LSH_TOK_BUFSIZE 64

char **parse_line(char *line)
{
  int bufsize = 512, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }
//function strtok returns
//pointers to within the string you give it, 
//and place \0 bytes at the end of each token.

	  token = strtok(line,"\t\r\n\a-");
	  while (token != NULL) {
	  	//store each pointer in an array (as a buffer) of character pointers of the tokens

	    tokens[position] = token;
      
	    position++;

	    token = strtok(NULL,"\t\r\n\a-");
	  }
	  tokens[position] = NULL;
  
  return tokens;
}

int execute(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
     
      perror("error");
    }
    
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("error");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

char **parse_commands(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }
//function strtok returns
//pointers to within the string you give it, 
//and place \0 bytes at the end of each token.
  
    token = strtok(line,";&");
    while (token != NULL) {
      //store each pointer in an array (as a buffer) of character pointers of the tokens

      tokens[position] = token;
      

      position++;

     

      token = strtok(NULL,";&");
    }
    tokens[position] = NULL;
  
  return tokens;
}


int main(int argc, char *argv[]){

  //char *line;
  char **args;
  int status;
  char *line = NULL;  /* forces getline to allocate with malloc */
  size_t len = 0; 
  ssize_t read;

  char q[]="quit";  
  
   char **commands;
   int mode;
   printf("interactive(1) or batchfile(2)\n");
   scanf("%d" ,&mode);
   FILE *file=NULL;
   if (mode==2) { 
       
        file = fopen("batchfile.txt","r");
        if (file == NULL){
             printf("error reading the file\n");
             return -1;
         }
  }
  while(1){
        
    write_my_name();
      
    line = read_line(mode,&file);
    printf("%s\n",line );

    if ( strcmp( line , q) == 0) exit(0);

    if ( strcmp( line, "\0") == 0) continue;
        
    int num= num_commands(line);
    //now parse lines command by command

    commands=parse_commands(line);

    //now parse lines with delimeters="\t\r\n\a-" and execute according if ; or & exists;
    int vers = version(line);

      if (vers ==2){ //           &     exists 
        for ( int i =0 ; i< num; i++){

            args = parse_line(commands[i]);    
            status = execute(args);
            printf("\n");
            /*if status returns 0
            that means the command is not valid
            so we stop running any net commands*/
            if(status==0){
              
              break;
            }
        }
      }
      else if (vers ==1){     // ; exists 
           for ( int i =0 ; i< num; i++){

            args = parse_line(commands[i]);    
            status = execute(args);
            printf("\n");
           }
      }
      else {
            args = parse_line(line);    
            status = execute(args);
      }
      if (mode ==2){ //batchfile mode 
        if (getline(&line, &len, file)!= -1) {
          break;
        }

      }
      free(line);
      free(args);
      //call exit 
    //getline(&line, &len, stdin);
    
  }
  return 0;


}

