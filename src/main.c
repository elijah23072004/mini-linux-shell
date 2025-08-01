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

int parse_input(char* str){
    return 0;
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
        printf("input received: %s",input_command);

        //parse input separate command and args 
        res = parse_input(input_command); 

        
    }
    return 0;
}
