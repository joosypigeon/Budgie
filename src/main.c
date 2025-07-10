#include <stdio.h>
#include <stdlib.h>

#include "budgie/core.h"

int main(void){

    buVector3 v = {1.0, 2.0, 3.0};
    
    printf("v: (" REAL_FMT ", " REAL_FMT ", " REAL_FMT ")\n", v.x, v.y, v.z);

    return 0;

}