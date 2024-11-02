#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    if (isatty(STDERR_FILENO))
	{
		write(STDERR_FILENO, "Standard error is connected to a terminal.\n", 43);
    }
	else
	{
		write(STDERR_FILENO, "Standard error is not connected to a terminal.\n", 47);
    }
}
