#pragma once
#ifndef MACROS_H_
#define MACROS_H_
//return (temp!=null)
#define CHECK_RETURN_0(temp) if (temp==NULL) return 0;
#define CHECK_MSG_RETURN_0(temp) if (temp==NULL) {printf("The allocation is faild");return 0;}
#define FREE_CLOSE_FILE_RETURN_0(temp,file) free(temp); fclose(file); return 0;
#define CLOSE_RETURN_0(file) fclose(file); return 0;

#define DETAIL_PRINT 

#endif /* MACROS_H_ */