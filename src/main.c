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


int runCommand(char** arguments,int numberArguments){
    pid_t p = fork();
    if(p <0){
        printf("fork failed\n");
    }
    else if( p>0){
        printf("Parent process\n");
        wait(NULL);
        printf("Child process finished\n");

    }
    else{
        printf("Child process\n");
        char input[100];
        if(numberArguments == 1){
            execlp(arguments[0], arguments[0], NULL);
        }
        else{
            strcpy(input,arguments[1]);
            for(int i=2; i<numberArguments;i++){
                strcat(input, " ");
                strcat(input, arguments[i]); 
            }
            printf("command: %s\n",arguments[0]); 
            printf("arguments: %s\n",input);
            execlp(arguments[0], arguments[0], input, NULL);
        }
        printf("after execlp did not find command\n");
        
        execl(arguments[0], arguments[0], input,NULL);


        exit(0);
    }
    return 0;
}

int handleArguments(char** arguments, int numberArguments){
    
    char* shellCommands[1];
    shellCommands[0]= "cd";
    if(strcmp(arguments[0], shellCommands[0]) == 0 ){
        return changeDirectory(arguments+1,numberArguments-1);
    }
    else if(strcmp(arguments[0],"clear") == 0){
        clear();
        return 0;
    }
    else if(strcmp(arguments[0],"exit")==0){
        exit(0);
    }
    else{
        printf("did not find command searching path locations\n");

        return runCommand(arguments, numberArguments);
    }

    return 0;
}

char* parse_wildCards(char* input){
    int i =0;
    char prev = ' ';
    char* text = input;
    while(1){
        if(i > strlen(text)){
            return text;;
        } 
        if(text[i] == '~'){
            if(prev == ' ')
            {
                if(i == (strlen(text)-1) || text[i+1] == '/'){
                    char* home_Dir = get_home_dir();
                    //printf("input: %s\n",text);
                    //printf("%s\n",home_Dir);
                    int newLen = strlen(text) + strlen(home_Dir);
                    char* temp = (char*)malloc(newLen* sizeof(char));
                   // printf("%s\n",temp);
                    memcpy(temp, text, i);
                    //printf("after first memcpy: %s\n",temp);
                    memcpy(temp+i,home_Dir , sizeof(home_Dir)+1);
                    memcpy(temp+i+sizeof(home_Dir)+1,text+i+1,strlen(input)-i);
                    text=temp;
                    //printf("output: %s\n",text);

                    i+=strlen(home_Dir);
                    if(i>strlen(text)){
                        return text;
                    }
                    ///printf("i: %d\n",i);
                    //printf("%c\n", text[i]);
                }
            }
        }
        else if(text[i] == '*'){
        }
        else if(text[i] == '\\'){
        }
        prev = text[i];
        i++;

    }
    
}

int parse_input(char* input){
    input = parse_wildCards(input);
    printf("%s\n",input);
    char* token;
    const char delimiter[4] = " ";
    token = strtok(input, delimiter);
    int numberArguments=0;
    int size = 20;
    char** arguments = (char**)malloc(size * sizeof(char*));
    while(token != 0){
        //printf(" %s\n",token);
        if(numberArguments+1>=size){
            size=size*1.5;
            char** tmp=arguments; 
            arguments = (char**)realloc(tmp, size*sizeof(char*));
            //printf("reallocated new size:%d\n",size);
        }
        arguments[numberArguments] = token;
        numberArguments++;
        
        token= strtok(0, delimiter);
    }

    /*for(int j=0; j<numberArguments;j++){
        printf("%s,",arguments[j]);
    }
    */printf("\n");


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
