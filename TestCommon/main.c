#include <stdlib.h>

#include "../Test/test.h"

SUITE(test_hashmap);

int main(int argc, char *argv[])
{
    sigma_test_main(argc, argv);

    test_hashmap();

    return EXIT_SUCCESS;
}
