#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
struct Process {
	int Priority,Rtime,Atime,Btime,Qtime,ID;
	clock_t Arrival;
	int Preemtion_Flag,Arrival_Flag,Flag,Completion;
	sem_t se;
	struct Process *Next;
};
typedef struct Process node;
clock_t Start,Count;
pthread_t P[10];
node *Front_S=NULL,*Rear_S=NULL,*Front_F=NULL,*Rear_F=NULL,*Front_A=NULL,*Temp;
int i=0,n,TQ,l=1;
float TAT=0,WT=0;
void Spush();
void Fpush();
void *SudeshSharma(node *S) {
	int Null_Flag=0;
	while(1) {
		sem_wait(&S->se);
		if((S->Atime<=(clock()-Start)/CLOCKS_PER_SEC && S->Arrival_Flag==1) || S->Preemtion_Flag==1) {
			S->Arrival_Flag=0;
			S->Preemtion_Flag=0;
			Count=clock();
		}
		if(S->Flag==1) {
			printf("\nQuery-%d Started \t\t\t\t\tTimer :%d",S->ID,(clock()-Start)/CLOCKS_PER_SEC);
			S->Flag=0;
			S->Arrival=clock();
		}
		if((clock()-Count)/CLOCKS_PER_SEC==1) {
			Count=clock();
			printf("\nQuery-%d Running\t\t\t\t\t\tTimer :%d",S->ID,(clock()-Start)/CLOCKS_PER_SEC);
			S->Rtime-=1;
			S->Qtime+=1;
			if(S->Rtime==0) {
				S->Completion=1;
				TAT+=(clock()-Start)/CLOCKS_PER_SEC-S->Atime;
				WT+=((clock()-Start)/CLOCKS_PER_SEC)-S->Atime-S->Btime;
				if(S->Priority==1) {
					if(Front_F!=Rear_F) {
						Null_Flag=1;
						Front_F=Front_F->Next;
						sem_post(&Front_F->se);
					}
					printf("\nQuery-%d completed executing, ",S->ID);
					if(Front_F==Rear_F && Null_Flag==0){				
						Rear_F=NULL;
						Front_F=NULL;
						if(Front_S!=NULL) {
							sem_post(&Front_S->se);
							printf("next Query is: %d",Front_S->ID);
						}
						else {
							l=1;
						}
					}
					else {
					printf("next Query is: %d",Front_F->ID);
					}	
				}
				else {
					if(Front_S!=Rear_S) {
						Null_Flag=1;
						Front_S=Front_S->Next;
						sem_post(&Front_S->se);
					}
					printf("\nQuery-%d completed executing,\n",S->ID);
					if(Front_S==Rear_S && Null_Flag==0){				
						Rear_S=NULL;
						Front_S=NULL;
						if(Front_F!=NULL) {
							sem_post(Front_F->se);
							printf("next Query is: %d",Front_F->ID);
						}
						else {
							l=1;
						}
					}
					else{
						printf("next Query is: %d",Front_S->ID);
					}	
				}
			}
			else if(S->Qtime>=TQ) {
				if(S->Priority==1 && Front_F!=NULL && Front_F->Next!=NULL) {
					S->Qtime=0;
					Front_F=Front_F->Next;
					Fpush(S);
					printf("\nQuery-%d Context Switched to Query-%d",S->ID,Front_F->ID);
					S->Preemtion_Flag=1;
					sem_post(&Front_F->se);
					sem_wait(&S->se);
				}
				else if(S->Priority==2 && Front_S!=NULL && Front_S->Next!=NULL){
					S->Qtime=0;
					Front_S=Front_S->Next;
					Spush(S);
					printf("\nQuery-%d Context Switched to Query-%d",S->ID,Front_S->ID);
					S->Preemtion_Flag=1;
					sem_post(&Front_S->se);
					sem_wait(&S->se);
				}
			}
		}
		if(S->Completion==1) {
			break;
		}
		sem_post(&S->se);	
	}
}
void Spush(node *Temp) {
	if(Front_S==NULL) {
		Front_S=Temp;
		Rear_S=Temp;
		Temp->Next=NULL;
	}
	else {
		Temp->Next=NULL;
		Rear_S->Next=Temp;
		Rear_S=Temp;
	}
}
void Fpush(node *Temp) {
	if(Front_F==NULL) {
		Front_F=Temp;
		Rear_F=Temp;
		Temp->Next=NULL;
	}
	else {
		Temp->Next=NULL;
		Rear_F->Next=Temp;
		Rear_F=Temp;
	}
}
void push() {
	Temp=(node *)malloc(sizeof(node));
	Temp->ID=i+1;
	printf("\nQuery no: %d\n1.Faculty or 2.Student?",Temp->ID);
	do{
		scanf("%d",&Temp->Priority);
		if(Temp->Priority!=1 && Temp->Priority!=2) {
			printf("\nWrong input select from above.\n1.Faculty or 2.Student?");
		}
	}while(Temp->Priority!=1 && Temp->Priority!=2);
	printf("\nQuery Registered Time :");
	scanf("%d",&Temp->Atime);
	printf("\nTime taken to solve this Query :");
	scanf("%d",&Temp->Btime);
	Temp->Rtime=Temp->Btime;
	Temp->Qtime=0;
	sem_init(&Temp->se,0,0);
	Temp->Arrival_Flag=1;
	Temp->Preemtion_Flag=0;
	Temp->Flag=1;
	Temp->Completion=0;
	node *Start=Front_A;
	if(Front_A==NULL) {
		Front_A=Temp;
		Temp->Next=NULL;
	}
	else if (Start->Atime > Temp->Atime || (Start->Atime == Temp->Atime && Start->Priority > Temp->Priority)){
        	Temp->Next = Front_A;
        	Front_A=Temp;
    }
    else {
    	while (Start->Next != NULL && Start->Next->Atime <= Temp->Atime) {
       		if(Start->Next->Atime==Temp->Atime && Temp->Priority<Start->Next->Priority) {
           		break;
			}
			else
				Start = Start->Next;	
        	}
        Temp->Next = Start->Next;
        Start->Next = Temp;
    }
}
void main() {
	time_t my_time;
	struct tm * timeinfo; 
	time (&my_time);
	timeinfo = localtime (&my_time);
	char ch;
   	printf("*");
	int l=1;
	printf("Enter Number of Queries :");
	scanf("%d",&n);
	while(i<n) {
		push();
		i+=1;
	}
	printf("\n\t\tWELCOME SUDESH SHARMA.\n");
	while(1){
		printf("Enter Your password:-\n");
		int z=0;
		char pass[20];
		while(1){	
   			ch = getch();
      		if(ch==13) {
      			pass[z]='\0';
      			break;
	  		}
      		pass[z++]=ch;
   			printf("*");
  		}
  		if(strcmp(pass,"sudesh@123")==0) {
  			printf("\nlogged in\n");
  			break;
		  }
		  else {
		  	printf("Wrong Password Please reenter:\n");
		  }
	}
	if(timeinfo->tm_hour>=18 && timeinfo->tm_hour<19)
	{
	printf("\nEnter your time quantum you want to share with each query: ");
	scanf("%d",&TQ);
	Start=clock();
	Count=clock();
	i=0;
	while(Front_A!=NULL) {
		if(Temp->Atime<0) {
			printf("Invalid Arrival Time for Process :%d",Temp->ID);
			Front_A=Front_A->Next;
			i++;
		}
		if((clock()-Start)/CLOCKS_PER_SEC>=Front_A->Atime) {
			if(l==1) {
				sem_post(&Front_A->se);
				l=0;
			}
			pthread_create(&P[i],NULL,SudeshSharma,Front_A);
			Temp=Front_A;
			Front_A=Front_A->Next;
			if(Temp->Priority==1) {
				Fpush(Temp);
			}
			else {
				Spush(Temp);
			}
			i+=1;
		}
		else if((clock()-Count)/CLOCKS_PER_SEC==1 && (Front_S==NULL  && Front_F==NULL)) { 
			Count=clock();
			l=1;
			printf("\nNo Query is Running\t\t\t\t\tTimer :%d",(clock()-Start)/CLOCKS_PER_SEC);
		}
	}
}
else{
	printf("\nsorry...! this is not your sheduled time please come between 10 - 12");
	exit(0);
}
	for(i=0;i<n;i++) {
		pthread_join(P[i],NULL);
	}
	printf("\naverage waiting time :- %f\naverage turn around time is:- %f",(float)WT/n,(float)TAT/n);
}
