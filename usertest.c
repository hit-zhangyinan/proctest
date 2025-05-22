#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(void)
{
	char buf[100];
    int s32Ret = 0;
	int fd = open("/proc/mydev", O_RDWR);

    // when fail, return -1 and set errno to indicate the error.
	s32Ret = read(fd, buf, 100);
    if (s32Ret < 0) {
        printf("Line %d, error code: %s\n", __LINE__, strerror(errno));
        return errno;
    }
	puts(buf);

	s32Ret = lseek(fd, 0 , SEEK_SET);
    if (s32Ret < 0) {
        printf("Line %d, error code: %s\n", __LINE__, strerror(errno));
        return errno;
    }
	write(fd, "33 4", 5);

	lseek(fd, 0 , SEEK_SET);
	read(fd, buf, 100);
	puts(buf);

    close(fd);
    return 0;
}
