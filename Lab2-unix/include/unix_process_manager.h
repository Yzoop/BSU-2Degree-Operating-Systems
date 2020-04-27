#ifndef UNIX_PROCESS_MANAGER_H
#define UNIX_PROCESS_MANAGER_H
#define UNIX_PROCESS_NAME_SIZE 40
#define UNIX_PROCESS_CREATE_SUCCESS 0
#define UNIX_PROCESS_CREATE_ERROR 0xFFFF

typedef unsigned short UNIX_PROCESS_CREATE_HANDLE;

struct unix_process_st {
	char name[UNIX_PROCESS_NAME_SIZE];
	char* argv[];
};



/*
	Function: create_unix_process(const struct unix_process_st* unix_proc);
	
	Description :Tries to fork main process and create a child process with unix_proc data

	Arguments: 'unix_proc' - const structure with suitable data for a child process

	Returns: UNIX_ERROR_HANDLE

*/
UNIX_PROCESS_CREATE_HANDLE create_unix_process(const unix_process_st* unix_proc);


#endif // !UNIX_PROCESS_MANAGER_H
