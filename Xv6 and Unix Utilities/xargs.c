#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


int main(int argc,char* argv[]){
    char argument[MAXARG];
    char c[1];
    int i=0;
     if(argc < 2){
    fprintf(2, "missing arguments\n");
    exit(0);
  }
    while(read(0,c,1)>0){
        if(c[0]!='\n'){
            argument[i++]=c[0];
        }else{
            argument[i]=0;
            i=0;
            if(fork()!=0){
                wait(0);
            }else{
                char* cmd[MAXARG];
                cmd[0]=argv[1];
                for(int j=2;j<argc;j++){
                    cmd[j-1]=argv[j];
                }
                cmd[argc-1]= argument;
                cmd[argc]=0;
                exec(argv[1],cmd);
                exit(0);
            }
        }
    }
    exit(0);
}
