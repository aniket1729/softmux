#ifndef COMMON_H
#define COMMON_H

#include <execinfo.h>
#include <stdlib.h>

#define BT_FUNC_PTR_COUNT 10
#define BT_PRINT { \
        int i, nFuncPtrs; \
        void *btFuncPtrs[BT_FUNC_PTR_COUNT]; \
        char **btFuncName; \
        \
        nFuncPtrs = backtrace(btFuncPtrs, BT_FUNC_PTR_COUNT); \
        \
        btFuncName = backtrace_symbols(btFuncPtrs, nFuncPtrs); \
        if (btFuncName) { \
            printf("Backtrace: \n"); \
            for( i = 0; i < nFuncPtrs; i++) { \
                printf("\t %s\n", btFuncName[i]); \
            } \
            printf("\n"); \
            free(btFuncName); \
        } \
    }

#if 1    //CPP
#define ALLOC_POINTER_ARGS(var, TYPE, args...)  var = new TYPE(args)

#define ALLOC_POINTER_DEFAULT(var, TYPE)        var = new TYPE()
#define ALLOC_ARRAY(var, TYPE, size)            var = new TYPE[size]
#define DEALLOC_POINTER(var)                    delete var
#define DEALLOC_ARRAY(var)                      delete[] var
#else    //C
#define ALLOC_POINTER_DEFAULT(var, TYPE)    	var = (TYPE*) malloc(sizeof(TYPE))
#define ALLOC_ARRAY(var, TYPE, size)        	var = (TYPE*) calloc(size, sizeof(TYPE))
#define DEALLOC_POINTER(var)                	free(var)
#define DEALLOC_ARRAY(var)                  	free(var)
#endif


//----- Return values -----
#define RET_OK  0
#define RET_NOK -1

#endif    //COMMON_H
