/******************************************************************
 * i_thread_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_THREAD_API_H_
#define API_MODULE_THREAD_API_H_

//Maximum number of input queues of DataEngineTask
#define MAX_IN_Q (1)
//Maximum number of output queues of DataEngineTask
#define MAX_OUT_Q (1)

enum DETask_priority_t
{
	TASK_PRIO_0,
	TASK_PRIO_1,
	TASK_PRIO_2,
	TASK_PRIO_3,
	NUM_TASK_PRIO,
} ;

enum TASK_QUEUE_T
{
	PERMANENT_TASK_Q,
	TEMPORAL_TASK_Q
} ;

#define THREAD_PROF "THREAD_MGR"
#define THREAD_MOD "THREAD_MGR"

/******************************************************************************************//*!
 *@class DataEngine_api (Data engine API)
 *@brief The purpose of this class is :
 *@brief This class allow for the worker thread to run the data engine.
 *@brief
  *********************************************************************************************/

struct DataEngineTask_api{
	//Run the data engine function
	virtual void run(uint64_t cookie) = 0;

	//Optional methods for debugging:
	//Asynchronous stop request
	virtual void async_stop() =	0;
	//Get the data engine execution deadline time (sec+ nanosec)
};

//TASK allocation structure, contains information about the task
struct TaskAllocStruct {
	DataEngineTask_api  *data_engine_p;
	uint64_t			cookie;
};
/******************************************************************************************//*!
 *@class WorkerThread_api
 *@brief The purpose of this class is :
 *@brief This class represent one running thread (core). The interface allows to control the thread running and activation.
 *@brief
  *********************************************************************************************/
struct WorkerThread_api
{
	//The following methods should be called in ColdStart (connecting) stage.

	//Set affinity, attach the logical thread to some core. If the requested affinity are selected already then return false.
	virtual bool set_affinity(uint32_t core_num)=0;
	//Add queue of data engine tasks. This method should be added before the thread is started. Return false if error.
	virtual bool attach_task_fifo(CMemArea *ptr, TASK_QUEUE_T task_queue_t, DETask_priority_t priority)=0;

	//These methods can be called in run time.
	//Get affinity number. If the affinity was not set yet, return -1.
	virtual uint32_t get_affinity()=0;

	//Thread starts fetching and executing tasks from the attached tasks queues.
	virtual void start()= 0;
	//Thread stops fetching and executing tasks from the attached tasks queues.
	virtual bool sync_stop()= 0;

};


#endif /* API_MODULE_THREAD_API_H_ */
