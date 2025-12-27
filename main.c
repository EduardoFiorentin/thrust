#include "source/source.h"
#include "main.h"

int main() 
{   
    Genfile genfile;
    
    // load genfile

    // validate genfile

    
    RandomServiceContext random;
    random.initialized = CTX_NOT_INITIALIZED;
    
    Context context; 
    context.random = random;

    run_thread_source(&genfile, &context);


    return 0;
}