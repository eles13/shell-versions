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
int flag1=0;
int palka=0;
int divisions[N];
char*** fcmd;
pid_t target_pid,pid,curpid;
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
    int dcounter=0;
    for(int k=0;k<N;k++)
    divisions[k]=0;//fcmd=calloc(N,1);
    char* cwd=calloc(N,1);
    getcwd(cwd,N);
    printf("%s>",cwd);
	  pipe(fd);
    int ent=0;
    int f=0;
    int flag=0;
    palka=0;
    int prevp=0;
    res = calloc(N,1);
    res[0]=NULL;
    res[1]=NULL;
    int imax=N;
    int jmax=N;
    *i=0;
    int j=0;
    while (ch=getchar())
    {
        if (prevp)
        {
          if ((ch=='\n')||(ch==EOF)||(ch=='|')||(j==1))
          { printf("ERR IN SYMBOL OF PIPE"); return NULL; }
          prevp=0;
        }
        if(ch==EOF)
        break;
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
          close(fd[1]);
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
					       fd[1]=open(res[*i],O_WRONLY|O_CREAT|O_TRUNC,0644);
          }
          else
          {
                fd[1]=open(res[*i],O_WRONLY|O_CREAT|O_APPEND,0644);
          }
					if (fd[1]<0)
					{
						printf("CANNOT OPEN FILE %s\n",res[*i]);
						return NULL;
					}
					dup2(fd[1],1);
					close(fd[1]);
          close(fd[0]);
					(*i)--;
					if ((ch=='\n')||(ch==EOF))
                    { ent=1; break; }
				}
        else if (ch=='|')
        {
          //char*** cmdl;
          palka=1;
          prevp=1;
          divisions[dcounter]=*i;
        //  (*i)++;
          dcounter++;

          break;

        }
                else
                {
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
            (*i)+=1; j=0;
        }
        if (((*i)==1)&&(ch=' ')) res[*i]=NULL;
        if (ent==1) { divisions[dcounter]=(*i)-1; return res;}
    }
    if (ch==EOF)
    exit(0);
    divisions[dcounter]=(*i)-1;
    return res;
}
void Sighndlr(int s)
{
	if(pid==waitpid(-1,&status,WNOHANG))
	{
		flag1=1;
	}

}
int main(int argc, char** argv)
{
	signal(SIGCHLD,Sighndlr);
    char** endm;
    int imax;
    dup2(0,8);
    dup2(1,9);
    char *cwd=calloc(N,sizeof(char));
    do
    {
		if (flag1==1)
		{
			printf("EXITED WITH %d pid %d\n",WEXITSTATUS(status1),pid);
			flag1=0;
		}

		endm=inps(&imax);

		if (endm==NULL) continue;
    if (palka)
    {
      int fd1[2];
      pipe(fd1);
      int pcount=0;
      int offset=0;
      if(fork()==0){
      if(fork()==0)
      {
        dup2(fd1[1],1);

        close(fd1[1]);
        close(fd1[0]);

        if (pcount)
        offset=divisions[pcount-1]+1;
        char** fcmd=calloc(divisions[pcount]+1,N+N);

        for(int j=0;j<=divisions[pcount];j++)
        {
          fcmd[j]=calloc(N,1);
          strcpy(fcmd[j],res[j+offset]);
          //printf("%s ",fcmd[0]);
         //printf("%s\n",res[0+offset]);
        }
        execvp(fcmd[0],fcmd);
        printf("ERR IN PIPING\n");
        exit(1);
      }
       pcount++;
        int between=abs(divisions[pcount]-divisions[pcount-1]);
        if (pcount)
        offset=divisions[pcount-1]+1;
        char** fcmd=calloc(N,sizeof(char*));
        for(int j=0;j<between;j++)
        {
          fcmd[j]=calloc(N,1);
          strcpy(fcmd[j],res[j+offset]);
        }
        dup2(fd1[0],0);
        close(fd1[0]);
        close(fd1[1]);
        execvp(fcmd[0],fcmd);
        printf("ERR IN PIPING\n");
        exit(status);}
        close(fd1[0]);
        close(fd1[1]);
        wait(&status);
        //dup2(8,0);
        //dup2(9,1);
        //continue;
    } else
		if (strcmp(res[imax-1],"&")==0)
		{
			endm[imax-1]=NULL;
			if ((pid=fork())==0)
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
				} 	else { printf("ERR");}
				exit(1);
			}
      curpid=pid;
			for (int i=0;i<=imax;i++) free(res[i]);
			free(res);
			//continue;
		} else
		//printf("dd\n");
        //  printf("%s %s \n",endm[0],endm[imax-1]);
      /*if (fork()==0)
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
    //signal(SIGCHLD,SIG_DFL);
    //while ((pid = waitpid(-1,&status1,WNOHANG))>0);
    for (int i=0;i<=imax;i++) free(res[i]);
    free(res);
    continue;
	}
	*/
	//printf("aa");
    if ((target_pid=fork())==0)
    {
      //printf("dd");
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
      curpid=target_pid;
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
        //waitpid(-1,&status,WNOHANG);
        curpid=pid;
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
