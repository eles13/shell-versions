#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#define N 100
char ch;
char** res;
int status,status1;
int fd[2];
pid_t target_pid;
void conchg(char* cdir,char* dst)
{
    int i=0;
    int l=strlen(cdir);
    if (strcmp(dst,"")==0)
    {
      char* homedir=getenv("HOME");
      printf("%s",homedir);
      chdir(homedir);
    }
    if (dst[0]=='/')
    {
        chdir(dst);
    }   else
    {
    cdir=realloc(cdir,strlen(dst)+strlen(cdir)+2);
    cdir[i+l]='/';
    while (dst[i]!=0)
    {
        cdir[i+l+1]=dst[i];
            i++;
    }
        cdir[i+l+1]=0;
        if (chdir(cdir)==-1)
           printf("DST NOT FOUND\n");
    }
}
char** inps(int* i)
{
    char* cwd=calloc(N,1);
    getcwd(cwd,N);
    printf("%s>",cwd);
	  pipe(fd);
    int ent=0;
    int f=0;
    int flag=0;
    res = calloc(N,1);
    res[0]=NULL;
    res[1]=NULL;
    int imax=N;
    int jmax=N;
    *i=0;
    int j=0;
    res[0]=NULL;
    while (((ch=getchar())!=EOF))
    {
        printf("%s \n",res[0]);
        ent=0;
        if (ch=='\n')
        {
            getcwd(cwd,N);
            printf("%s>",cwd);
            *i=0;
            continue;
        }

        if ((*i)>=imax)
        {
            imax+=1;
            res=realloc(res,imax);
        }
        if(isspace(ch)==0)
        {
            res[*i]=calloc(jmax,1);
            while(((ch!=' ')||((f==1)))&&(ch!=EOF))
            {
                if ((f==0)&&(ch=='"')) f++;
                else if((f==1)&&(ch=='"')) f--;
                else if (ch=='&')
                {
                  (*i)++;
                  res[*i]=calloc(jmax,1);
                  res[(*i)][0]=ch;
                  if ((ch=getchar())!='\n')
                  {
                    printf("ERR IN AMP\n");

                    return NULL;
                  }
                  else { ent=1; break; }
                }
                else if (ch=='<')
                {
					ch = getchar();
					while((ch!=' ')&&(ch!='\n')&&(ch!=EOF))
					{
						res[*i][j]=ch;
						j++;
						ch=getchar();
					}
					fd[0]=open(res[*i],O_RDONLY);
					if (fd[0]<0)
					{
						printf("CANNOT OPEN FILE %s\n",res[*i]);
						return NULL;
					}
					dup2(fd[0],0);
					close(fd[0]);
					(*i)--;
					if ((ch=='\n')||(ch==EOF))
                    { ent=1; break; }
				}
				      else if (ch=='>')
                {
					ch = getchar();
          if (ch=='>')
          {
            flag=1;
            ch=getchar();
          }

					while((ch!=' ')&&(ch!='\n')&&(ch!=EOF))
					{
						res[*i][j]=ch;
						j++;
						ch=getchar();
					}
          if (flag==0)
          {
					       fd[1]=open(res[*i],O_WRONLY|O_CREAT|O_TRUNC);
          }
          else
          {
                fd[1]=open(res[*i],O_WRONLY|O_CREAT|O_APPEND);
          }
					if (fd[1]<0)
					{
						printf("CANNOT OPEN FILE %s\n",res[*i]);
						return NULL;
					}
					dup2(fd[1],1);
					close(fd[1]);
					(*i)--;
					if ((ch=='\n')||(ch==EOF))
                    { ent=1; break; }
				}
                else
                {            //printf("ddd\n");
                              if (jmax<=j)
                    {
                            jmax+=1;
                            res[*i]=realloc(res[*i],jmax);
                    }
                    res[*i][j]=ch;
                }
                j++;
                ch=getchar();
                if ((ch=='\n')||(ch==EOF))
                { ent=1; break; }
            }

        }
        (*i)+=1; j=0;
        if (((*i)==1)&&(ch=' ')) res[1]=NULL;
        if (ent==1) {return res;}
//printf("ddd\n");
    }
    if (ch==EOF)
    exit(0);
    return res;
}
void Sighndlr(int s)
{
	printf("EXITED WITH %d\n",WEXITSTATUS(status));

}
int main(int argc, char** argv)
{
    char** endm;
    int imax;
    dup2(0,8);
    dup2(1,9);
    char *cwd=calloc(N,sizeof(char));
    do
    { printf("dddd\n");
    endm=inps(&imax);
    if (endm==NULL) continue;
    //printf("%s %s \n",endm[0],endm[imax-1]);
    if (strcmp(res[imax-1],"&")==0)
    {
      signal(SIGCHLD,SIG_IGN);
      endm[imax-1]=NULL;
        //  printf("%s %s \n",endm[0],endm[imax-1]);
      if (fork()==0)
      {
		  if ((fork())==0)
		  {
			if ((strcmp(endm[0],"cd"))!=0)
			{
				execvp(endm[0],endm);
				switch (errno)
				{
					case ENOENT:
					printf("NO FILE\n");
					//exit(1);
					continue;
					case ENOEXEC:
					printf("FORMAT ERR\n");
					//exit(1);
					continue;
					case EINVAL:
					printf("ARGUMENT ERR\n");
					//exit(1);
					continue;
					break;
					default:
					break;
				}
			} else { printf("ERR");}
			exit(status1);
      //kill(getppid(),SIGKILL);
		  }
		  else
		  {
			  wait(&status1);
        //wait(NULL);
        printf("EXITED WITH %d\n",WEXITSTATUS(status1));
        //ungetc('\n',stdin);
        //kill(getpid(),SIGKILL);
        //ch=getchar();

		  }
      exit(0);
    }
    //pid_t pid;
    signal(SIGCHLD,SIG_DFL);
    //while ((pid = waitpid(-1,&status1,WNOHANG))>0);
    for (int i=0;i<=imax;i++) free(res[i]);
    free(res);
    continue;
	}
    if ((target_pid=fork())==0)
    {
      printf("dd\n");
      if (res[0]==NULL) {printf("TOO FEW ARGS\n"); continue;}
        if ((strcmp(res[0],"cd"))!=0)
        {

            execvp(res[0],res);
            switch (errno)
            {
                case ENOENT:
                printf("NO FILE\n");
                //exit(1);
                continue;
                case ENOEXEC:
                printf("FORMAT ERR\n");
                //exit(1);
                continue;
                case EINVAL:
                printf("ARGUMENT ERR\n");
                //exit(1);
                continue;
                break;
                default:
                break;
            }
        }
        exit(status);
    }
    else
    {
        wait(&status);
        if(!WIFEXITED(status)){ printf("err in anc");}
        if ((strcmp(res[0],"cd"))==0)
        {
          if ((res[1]==NULL)||(strcmp(res[1],"")==0))
          {
            char* homedir=getenv("HOME");
            chdir(homedir);
            continue;
          }
          if (res[2]!=NULL)
          {
            printf("TOO MANY ARGS\n");
            continue;
          }
          getcwd(cwd,N);
          conchg(cwd, res[1]);
        }
        waitpid(-1,&status,WNOHANG);
        for (int i=0;i<=imax;i++) free(res[i]);
           free(res);
    }

        dup2(8,0);
        dup2(9,1);
        close(fd[0]);
        close(fd[1]);
    }while (ch!=EOF);
    return 0;
}
