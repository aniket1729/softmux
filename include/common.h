#ifndef COMMON_H
#define COMMON_H

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
