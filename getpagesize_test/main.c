//
//  main.c
//  getpagesize_test
//
//  Created by Avishek Mondal on 26/2/18.
//  Copyright © 2018 Avishek Mondal. All rights reserved.
//

#include <stdio.h>
#include<unistd.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    long sz = sysconf(_SC_PAGESIZE);
    printf("Page size %ld\n",sz);
    return 0;
}
