#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <iostream>

#include "tools.h"
#include "tests.h"
#include "stats.h"

FILE* generateFILE (int test_id)
{
	// generate test FILE* test value
	// see tests.h
	// use the functions specified by tools.h to create appropriate test values
	// you can use a copy of test.txt as file to test on
}

const char* generateCSTR (int test_id)
{
	// generate test const char* test value
	// see tests.h
	// use the functions specified by tools.h to create appropriate test values
}

const double wait_time = 0.01;   // wait time before quering childs exit status

void test_fputs (const TestCase& str_testCase, const TestCase& file_testCase)
{
	// execute single test
	// use function of stats.h to record all tests
}

int main (int argc, const char** argv)
{
	// execute all tests and catch exceptions
	
	return 0;
}

