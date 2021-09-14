#include <stdlib.h>

#include "test.h"

SUITE(test_char_stream);

int main(int argc, char *argv[])
{
    sigma_test_main(argc, argv);

    test_char_stream();

    return EXIT_SUCCESS;
}
