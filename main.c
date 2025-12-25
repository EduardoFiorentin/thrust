#include "source/source.h"

int main() 
{   
    Genfile genfile;
    RandomServiceContext random;
    Context context; 
    context.random = random;

    run_thread_source(&genfile, &context);


    return 0;
}