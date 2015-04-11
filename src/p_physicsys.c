#include "../include/p_physicsys.h"
#include "../include/p_memory.h"
#include "../include/p_buffer.h"
#include "../include/p_program.h"
#include "../include/p_entity.h"

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

cl_context p_context;

void P_physicinit()
{
	unsigned int tmp;
	cl_platform_id *pids, *pid;
	cl_device_id did;
	if(p_context)
		return;
	clGetPlatformIDs(0, 0, &tmp);
	if(!tmp)
	{
		printf("no OpenCL platforms detected\n");
		exit(-1);
	}
	pids = (cl_platform_id *) malloc(sizeof(cl_platform_id) * (tmp + 1));
	clGetPlatformIDs(tmp, pids, 0);
	pids[tmp] = 0; //used loop condition
	did = 0;
	for(pid = pids; *pid && !did; ++pid)
	{
		clGetDeviceIDs(*pid, CL_DEVICE_TYPE_GPU, 1, &did, 0);
	}
	free(pids);
	pids = pid = '\0';
	if(!did)
	{
		printf("no GPU OpenCL devices found\n");
		exit(-1);
	}
	if(!(p_context = clCreateContext(0, 1, &did, 0, 0, 0)))
	{
		printf("OpenCL context creation failed\n");
		exit(-1);
	}
	P_memoryinit();
	P_bufferinit();
	P_programinit();
	P_entityinit();
}

void P_physicclose()
{
	P_entityclose();
	P_programclose();
	P_bufferclose();
	P_memoryclose();
	if(p_context)
	{
		clReleaseContext(p_context);
		p_context = 0;
	}
}

