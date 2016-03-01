#include "common.h"


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


int main()
{
    open("/abcdef", O_RDONLY);
}
