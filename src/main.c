#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<readline/readline.h>
#include<readline/history.h>


//clear shell using escape sequences 
#define clear() printf("\033[H\033[J")
//max command length 1MB 
#define MAXCOMMANDLENGTH 1000000

//returns true if string pre is at start of str e.g. pre: abc str: abcde returns true  
bool is_prefix(const char* pre, const char* str){
    return strncmp(pre,str,strlen(pre)) ==0;

}



//gets home directory of user running program 
char* get_home_dir(){
    return getpwuid(getuid())->pw_dir;
}


void print_command_line(){
    
    char* currDir = (char*)malloc(1024);
    getcwd(currDir,1024);
    
    char* homeDir = get_home_dir();
    //replaces /home/user with ~
    if(is_prefix(homeDir,currDir)){
        char* tempStr = malloc(sizeof(char) * (strlen(currDir) + 1));
        tempStr[0]='~';
        strcpy(tempStr+1,currDir + ((sizeof(homeDir) + 1) * sizeof(char)));
        strcpy(currDir, tempStr);
        free(tempStr);
    }

    printf("\n%s\n", currDir);
    free(currDir);
}

void init_shell(){
    //use tab to autocomplete
    rl_bind_key('\t', rl_complete);
    //allow for history of commands (arrow buttons to go to previous inputs)
    using_history();

    clear();
    printf("\n\n");
    printf("Shell loaded.\n\n");
    char* username = getenv("USER");
    printf("Welcome %s", username);

}

int get_input(char* str){
    char* buf;
    buf = readline("> ");
    if(strlen(buf)> MAXCOMMANDLENGTH){
        printf("Command too long to be processed\n");
        return 2;
    }
    else if(strlen(buf) !=0){
        strcpy(str,buf);
        return 0;
    }
    //got length of 0 in readline
    return 1;
}

int changeDirectory(char** arguments, int numberArguments){
    if(numberArguments!=1){
        printf("Too many arguments\n");
        return 1;
    }
    chdir(arguments[0]);
    return 0;
}

int ls(){
   system("ls"); 
}

int handleArguments(char** arguments, int numberArguments){
    
    char* shellCommands[1];
    shellCommands[0]= "cd";
    if(strcmp(arguments[0], shellCommands[0]) == 0 ){
        changeDirectory(arguments+1,numberArguments-1);
    }
    else if(strcmp(arguments[0],"ls") == 0){
        ls();
    }
    else{
        printf("did not find command");
    }

    return 0;
}

int parse_input(char* input){
    char* token;
    const char delimiter[4] = " ";
    token = strtok(input, delimiter);
    int numberArguments=0;
    int size = 20;
    char** arguments = (char**)malloc(size * sizeof(char*));
    while(token != 0){
        printf(" %s\n",token);
        if(numberArguments+1>=size){
            size=size*1.5;
            char** tmp=arguments; 
            arguments = (char**)realloc(tmp, size*sizeof(char*));
            printf("reallocated new size:%d\n",size);
        }
        arguments[numberArguments] = token;
        numberArguments++;
        
        token= strtok(0, delimiter);
    }

    for(int j=0; j<numberArguments;j++){
        printf("%s,",arguments[j]);
    }
    printf("\n");


    //logic for handling input
    
    return handleArguments(arguments, numberArguments);

}

int main(){

    init_shell();
    char input_command[MAXCOMMANDLENGTH];
    while(1){
        print_command_line();
        int res = get_input(input_command);
        if(res != 0){
            continue;
        }
        //add input to history
        add_history(input_command);
        printf("input received: %s\n",input_command);

        //parse input separate command and args 
        res = parse_input(input_command); 

        
    }
    return 0;
}
