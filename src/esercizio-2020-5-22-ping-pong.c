#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUE 1000000

int parent_pid;

void err_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void write_int(int fd, int n)
{
	int written, total_written = 0;
	while ((written = write(fd, &n, sizeof(int))) > 0) {
		total_written += written;
		if (total_written < sizeof(int))
			continue;
		else
			return;
	}
}

void read_incr_write(int rfd, int wfd)
{
	char *msg;
	if (getpid() == parent_pid)
		msg = "parent received\t%d\n";
	else
		msg = "child received\t%d\n";

	int count, bytes_read, total_read = 0;
	while ((bytes_read = read(rfd, &count, sizeof(int))) > 0) {
		total_read += bytes_read;
		if (total_read < sizeof(int))
			continue;
		if (count < MAX_VALUE) {
			printf(msg, count);
			count++;
		} else {
			break;
		}
		write_int(wfd, count);
		total_read = 0;
	}
	printf("The end! count = %d\n", count);
}

int main()
{
	int ptc[2]; // parent-to-child pipe
	int ctp[2]; // child-to-parent pipe

	if (pipe(ptc))
		err_exit("pipe() error");
	if (pipe(ctp))
		err_exit("pipe() error");

	parent_pid = getpid();

	switch (fork()) {
	case 0:
		if (close(ptc[1]))
			err_exit("close() error");
		if (close(ctp[0]))
			err_exit("close() error");

		read_incr_write(ptc[0], ctp[1]);

		if (close(ptc[0]))
			err_exit("close() error");
		if (close(ctp[1]))
			err_exit("close() error");
		break;

	case -1:
		break;

	default:
		if (close(ptc[0]))
			err_exit("close() error");
		if (close(ctp[1]))
			err_exit("close() error");

		write_int(ptc[1], 0); // start the ping-pong
		read_incr_write(ctp[0], ptc[1]);

		if (close(ptc[1]))
			err_exit("close() error");
		if (close(ctp[0]))
			err_exit("close() error");
		if (wait(NULL) == -1)
			err_exit("wait() error");
		break;
	}

	return 0;
}
