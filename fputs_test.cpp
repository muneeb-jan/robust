#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

#include <string>
#include <iostream>

#include "tools.h"
#include "tests.h"
#include "stats.h"

FILE *generateFILE(int test_id) {

    FILE *file;
    char *dest = "test2.txt";
    filecopy("test.txt", dest);
    
    if (test_id == TC_FILE_NULL)
    {
        file = NULL;
    }

    else if (test_id == TC_FILE_RONLY)
        file = fopen(dest, "r");
    
    else if (test_id == TC_FILE_WONLY)
        file = fopen(dest, "w");
    
    else if (test_id == TC_FILE_RW)
        file = fopen(dest, "r+");

    else if (test_id == TC_FILE_CLOSED)
    {
        file = fopen(dest, "r");
        fclose(file);
    }

    else if (test_id == TC_FILE_MEM_RONLY)
    {
        file = fopen(dest, "r");
        fseek(file,0,SEEK_END);
        long size = ftell(file);
        fseek(file,0,SEEK_SET);

        file = (FILE *) malloc_prot(size, file, PROT_READ);
    }

    else if (test_id == TC_FILE_MEM_WONLY)
    {
        file = fopen(dest, "w");
        fseek(file,0,SEEK_END);
        long size = ftell(file);
        fseek(file,0,SEEK_SET);

        file = (FILE *) malloc_prot(size, file, PROT_WRITE);
    }

    else if (test_id == TC_FILE_MEM_RW)
    {
        file = fopen(dest, "r+");
        fseek(file,0,SEEK_END);
        long size = ftell(file);
        fseek(file,0,SEEK_SET);

        file = (FILE *) malloc_prot(size, file, PROT_READ || PROT_WRITE);
    }

    else if (test_id == TC_FILE_MEM_0_RONLY)
    {
        file = (FILE *) malloc_prot(getpagesize(), NULLpage(), PROT_READ);
    }

    else if (test_id == TC_FILE_MEM_0_WONLY)
    {
        file = (FILE *) malloc_prot(10, NULLpage(), PROT_WRITE);
    }

    else if (test_id == TC_FILE_MEM_0_RW)
    {
        file = (FILE *) malloc_prot(getpagesize(), NULLpage(), PROT_READ || PROT_WRITE);
    }


    else if (test_id == TC_FILE_MEM_INACCESSIBLE)
    {
        file = NULL;
    }

    else
        printf("\nNo cases matched, DumDum!\n");


    return file;
}

const char *generateCSTR(int test_id) {

    const char *ptr;
    char *temp = "Hello there\0";

    if (test_id == TC_CSTR_NULL)
    {   
        ptr = NULL;
    }

    else if (test_id == TC_CSTR_MEM_RONLY)
    {
        ptr = (char *) malloc_prot(sizeof(char)*strlen(temp) + 1,(const void *)temp, PROT_READ);
    }

    else if (test_id == TC_CSTR_MEM_WONLY)
    {
        ptr = (const char *) malloc_prot(sizeof(char)*strlen(temp) + 1,(const void *)temp, PROT_WRITE);
    }
    
    else if (test_id == TC_CSTR_MEM_RW)
    {
        ptr = (char*) malloc_prot(sizeof(char)*strlen(temp) + 1,(const void *)temp, PROT_READ || PROT_WRITE);
    }

    else if (test_id == TC_CSTR_MEM_0_RONLY)
    {
        temp = "Hello there";
        ptr = (char*) malloc_prot(sizeof(char)*strlen(temp),(const void *)temp, PROT_READ);
    }

    else if (test_id == TC_CSTR_MEM_0_WONLY)
    {
        temp = "Hello there";
        ptr = (const char *) malloc_prot(sizeof(char)*strlen(temp),(const void *)temp, PROT_WRITE);
    }

    else if (test_id == TC_CSTR_MEM_0_RW)
    {
        temp = "Hello there";
        ptr = (char*) malloc_prot(sizeof(char)*strlen(temp),(const void *)temp, PROT_READ || PROT_WRITE);
    }

    else if (test_id == TC_CSTR_MEM_INACCESSIBLE)
    {
        ptr = NULL;
    }

    else
    {
        printf("\nAAARRGGGHHH!!!\n");
    }


    return ptr;

}

const double wait_time = 1.0;

void test_fputs(const TestCase &str_testCase, const TestCase &file_testCase) {
  
    pid_t pid, parent;
    int status;
    
    record_start_test_fputs(str_testCase,file_testCase);
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid==0)
    {
        const char *generateCSTROutput= generateCSTR(str_testCase.id);
        FILE *generateFILEoutput= generateFILE(file_testCase.id);

        int return_fputs = fputs(generateCSTROutput, generateFILEoutput);
        if (return_fputs==EOF || return_fputs < 0)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
    }
    else
    {
        sleep(wait_time);
        parent = waitpid(pid, &status, WNOHANG);
            
        if (parent == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        
        if (parent == 0) {
            kill(pid, SIGTERM);
            record_timedout_test_fputs();
            return;
        }
        else if (WIFSIGNALED(status)) {
            record_crashed_test_fputs(WTERMSIG(status));
            return;
        } 
        else if (WIFSTOPPED(status)) {
            record_stopped_test_fputs(WSTOPSIG(status));
            return;
        } 
        else if (WIFEXITED(status)) {
            record_ok_test_fputs(WEXITSTATUS(status));
            return;
        }
    }

}

int main(int argc, const char **argv) {

    for (int i = 0; i < testCases_CSTR_count; i++)
    {
        for (int j = 0; j < testCases_FILE_count; j++)
        {
                test_fputs(testCases_CSTR[i], testCases_FILE[j]);
        }
    }

    print_summary();
    return 0;
}

