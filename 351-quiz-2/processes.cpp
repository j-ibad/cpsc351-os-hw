#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

/* The prototypes */
void parent();
void child1();
void child2();
void child3();
void child4();
void child5();
void child6();

/**
 * The function called by the FIRST child.
 */
void child1()
{
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
	
	//TODO: Fill me in	
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in child2() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child3();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in child2() process");
			exit(-1);
		}else if (child_b == 0) {
			child4();
			exit(0);
		} else {
			wait(NULL);
			exit(0);
		}
	}
}

/**
 * The function called by the SECOND child.
 */
void child2()
{
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
	
	//TODO: Fill me in
	pid_t child_b, child_a = fork();
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in child2() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child5();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in child2() process");
			exit(-1);
		}else if (child_b == 0) {
			child6();
			exit(0);
		} else {
			wait(NULL);
			exit(0);
		}
	}
}

/**
 * The function called by the THIRD child.
 */
void child3()
{
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
	
}

/**
 * The function called by the FOURTH child.
 */
void child4()
{

	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}


/**
 * The function called by the FIFTH child.
 */
void child5()
{

	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}

/**
 * The function called by the SIXTH child.
 */
void child6()
{
	
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}

/**
 * The function called by the parent
 */
void parent()
{
	fprintf(stderr, "I am the original parent; my process ID is pid=%d\n", getpid());
	/** Implementation **/
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in child2() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child1();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in child2() process");
			exit(-1);
		}else if (child_b == 0) {
			child2();
			exit(0);
		} else {
			wait(NULL);
			exit(0);
		}
	}
}


/**
 * The main function
 */
int main()
{
	parent();	
}
