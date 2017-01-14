#include "types.h"
#include "user.h"

int main (void) {

	int childPid = fork();
	if(childPid<0)
	 	printf(1,"Failed to make a child process\n");
	 else if(childPid>0) {
	 	
	 	printf(1,"This is parent \n Parent Pid : %d \n Child Pid : %d \n", getpid(),childPid);
	 	wait();
	 
	   }
	 else {

	 	printf(1,"This is child \n Parent Pid : %d \n Child Pid : %d \n", getParentPid() , getpid());
	 	wait();
	 	 	
	 	
	 }

}
