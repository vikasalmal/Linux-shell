#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

char str1[10][100];
int flag=0;
char file_name[20];
int args=-1;
void func_more_argv(char str1[10][100], int count);

int parse(char * str)
{
	int i=0,j=0,k=0;
	char temp[100];
	i=0;
	
	if( str [ strlen(str) -1 ] == ' ')		
		str[strlen(str)-1]='\0';


	while(str[i]!='\0')
	{
		temp[k]=str[i];
		if(str[i]==' ' || str[i]=='>' || str[i]=='\n')
		{
			temp[k]='\0';
			strcpy( str1[j] , temp );
			k=-1;
			++j;
			if(str[i]=='>' || flag ==1 )
			{
				flag=1;	++args;	
				if ( str[i+1] == '\0' || str[i+1] == '\n' )
					--args;
			}	
		}
		++i;	++k;
		
	}
	if(k!=-1)
	{
		temp[k]='\0';		
		if(flag==1)
		{
			strcpy( file_name , temp);
			args++;
		}
		

		else
		{	
			strcpy( str1[j] , temp);
			++j;
		}
	}
	return j;
}



void func_more_argv(char str1[10][100], int count)
{

    if(fork()==0)
    {	
   	char **newargv;
    	newargv = malloc(10*sizeof(char*));
    	int i=0;
	
	if(flag==1)
	{
		if( args==0 )
		{
			printf("File name not given\n ");
			exit(1);
		}
		if ( args >1 )
		{
			printf("Too many arguments given\n ");
			exit(1);
		}
		int fd=creat(file_name,O_RDWR);
		if(fd<0)
			printf("Cannot create file");
		else
			dup2(fd,1);
	}
	
	while(i<count)
	{
	     newargv[i] = malloc(100*sizeof(char));
	     strcpy(newargv[i],str1[i]);
	     ++i;
	}

        newargv[i+1] = malloc(100*sizeof(char));
	newargv[i+1]=NULL;

	char ch[10]={"/bin/ls"};
	int j;
	if(strcmp(str1[0],"ls")==0)
		 j=execvp (ch, newargv);

	else	     
		 j=execvp (str1[0], newargv);
	printf("\n");

	if(j==-1)
		printf("No such Command found\n");
	exit(1);
    }
    else 
    	wait();
}

int main(int argc, char *argv[])
{
    char* str, shell_prompt[100];
    rl_bind_key('\t', rl_complete);
    while(1)
    { 
        str = readline("MyShell >>> ");
        add_history(str);

	int count=parse(str);
        if(strcmp(str1[0],"exit")==0)
    	     exit(0);
	
        if(strcmp(str1[0],"cd")==0)
        {
             int fd=chdir(str1[1]);
             if(fd==-1)
             	printf("No such file or directory\n");
        }
        
	else 
		func_more_argv( str1, count );	
	printf("\n");

	flag=0;
	args=-1;
	memset(str1,'\0',sizeof(str1));
        free(str);
	
    }
 }  

