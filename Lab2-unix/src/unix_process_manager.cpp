#include "unix_process_manager.h"
#include <unistd.h>
#include <stdio.h>

UNIX_PROCESS_CREATE_HANDLE create_unix_process(const struct unix_process_st* unix_proc)
{
	pid_t proc_id = vfork();

	switch (proc_id)
	{
	case 0:
	{
		if (execv(unix_proc->name, unix_proc->argv) == -1)
		{
			return UNIX_PROCESS_CREATE_ERROR;
		}
	}
	case -1:
	{
		return UNIX_PROCESS_CREATE_ERROR;
	}
	default:
	{
		int wait_status;

		waitpid(proc_id, &wait_status, 0);
		
		return WEXITSTATUS(wait_status) != 0 ? UNIX_PROCESS_CREATE_ERROR : UNIX_PROCESS_CREATE_SUCCESS;
	}
	}

	return UNIX_PROCESS_CREATE_SUCCESS;
}