# Sigma Unit Test

Due to the apparent lack of a unit test framework *targeting ISO C* (instead of C++) *and* having a *convenient syntax*, I decided to roll my own.
Although this framework is developed as a project within a much larger solution, it is completely independent of any code outside the project.

## Getting Started

As an example, create a code file `test_sample.c` with the following content:

```C
#include "test.h"

TEST(first)
{
    EXPECT(7 == 7);
}

TEST(second)
{
    EXPECT(1 + 1 == 3);
}

SUITE(sample)
{
    RUN(first, second);
}

```

This way, we define two *tests* named `first` and `second` and a *suite* named `sample` that runs the two tests.
Note that the names of tests and suits must all be valid C identifiers.
This file by itself defines the tests and the suite but does not execute them.
To accomplish that, continuing from the above example, create another code file `main.c` and insert:

```C
#include "test.h"

SUITE(sample);

int main(void)
{
    sample();
}
```

While in `test_sample.c` we wrote `SUITE(sample)` to *define* the suite `sample`, here we only *declare* it.
As a result, the compiler recognizes `sample` as a function with prototype `void sample(void)` that we call in `main`.

## Test Structure

In general, a *suite* is a set of tests and a *test* is a block of C code containing *expectations*. If an expectation is not met (as will be the case in test `second` in the previous section), an error message will be printed.

In order to prevent name clashes, tests are local to the file they are defined in, i.e. they can not be referred to from another file.
Instead, tests are grouped into globally visible suits.
A test can be part of multiple suits and consequently a file can contain multiple suits.

Personally, I prefer to "import" all suits from all files in `main.c` and execute them inside the `main` function.
This way, `main.c` serves as a kind of table of contents and also the number of files is kept low.

## Platforms

I developed and tested the code on Windows 10 using both, Microsoft Visual C++ 2019 with C17 support turned on, and Clang 12.
In the future, I plan to add Linux support.
At the moment, I guess that due to Microsoft's implementation of `strtok_s` differing from the one on Linux, there will be compiler errors.
