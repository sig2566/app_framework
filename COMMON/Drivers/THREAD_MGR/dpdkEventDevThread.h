#ifndef _DPDK_EVENT_DEV_THREAD_H_
#define _DPDK_EVENT_DEV_THREAD_H_

#include <as_infra_envdep.h>
#include <rte_config.h>
#include <rte_malloc.h>
#include <rte_mempool.h>
#include <rte_cycles.h>
#include <stdint.h>
#include <unistd.h>
#include "i_api_common.h"
#include "global_defines.inc"
#include "i_thread_api.h"


using namespace ai_framework_proj;
class ThreadFactory;
// Abstract base class for all threads in dpdk EventDev
class DpdkEventDevThread: public WorkerThread_api
{
  public:
    DpdkEventDevThread()
    {
    	Reset();
    }

    virtual ~DpdkEventDevThread()
    {
    }

    //WorkerThread_api methods
	//The following methods should be called in ColdStart (connecting) stage.

	//Set affinity, attach the logical thread to some core. If the requested affinity are selected already then return false.
    bool set_affinity(uint32_t core_num);
	//Add queue of data engine tasks. This method should be added before the thread is started. Return false,  - error.
	bool attach_task_fifo(CMemArea *ptr, TASK_QUEUE_T task_queue_t, DETask_priority_t priority);

	//These methods can be called in run time.
	//Get affinity number. If the affinity was not set yet, return -1.
	uint32_t get_affinity();

	//Thread start
	void start()
	{
		stop_flag_ = false;
	}

	//Thread stop
	bool sync_stop()
	{
		uint32_t attempts= 0;
		stop();
		while(run_flag_)
		{
			attempts++;
			wait(50);
			if(attempts > 10)
			{
				return false; //Wait for 0.5 msec befor to decide that the task was stuck
			}
		}
		return true;
	}



    //General methods


    //Initialization of thread. It gets pointer to the Framework callback class and to the parent aggregated class
    void Init(IModuleControlCallBackAPI *callback_ptr, ThreadFactory *factory_ptr)
    {
//    	char thread_trace_format[]= "Calling Task:, task_data_cookie= %d";
    	char thread_trace_format[]= "ThreadManager Profiling:, duration=,\t%lld";
    	Reset();
    	callback_ptr_= callback_ptr;
    	daddy_p_ = factory_ptr;
        //Initialization of the profiler counter
    	INIT_PROF_CNT(THREAD_PROF, callback_ptr_, thread_prof_);
    	callback_ptr_->IRegistryTraceEntry(thread_trace_format, &thread_trace_id_);
    }

    // thread functions. User can replace ThreadFunc in derived classes
    // \param[in]  arg   some argument sending to thread function
    // \return some integer value depend of thread specific
    static int static_thread_func(void* arg);

    // Start thread
    // \param[in] arg        argument sending to thread member function
    // \param[in] lcore_id   id of core in which performed thread
    // \return AS_INFRA_RETCODE_ROK or EBUSY
    AS_INFRA_RETCODE start_thread(void* arg, unsigned lcore_id);


    int thread_func(void* arg);

    //struct to pass thread argument
    struct ThreadStaticArg
    {
        void*               member_arg = 0;
        DpdkEventDevThread* class_ptr  = 0;
    };

  protected:
    // Waiting for thread
    // \param[in]  micro seconds   timeout in micro seconds to wait
    // \return AS_INFRA_RETCODE_ROK if thread complete successfully and AS_INFRA_RETCODE_RFAILED if time is out
    AS_INFRA_RETCODE wait(uint32_t useconds);

    // Stop thread
    // \always return AS_INFRA_RETCODE_ROK
    AS_INFRA_RETCODE stop()
    {
        stop_flag_ = true;
        return AS_INFRA_RETCODE_ROK;
    }

    void Reset()
    {
    	int i;
    	stop_flag_ = true;
    	run_flag_ = false;
    	thread_retval_ = 0;
    	callback_ptr_ = NULL;
    	daddy_p_ = NULL;
    	core_num_ = -1;
    	permanent_q_ = NULL;
    	for(i=0; i< NUM_TASK_PRIO; i++)
    	{
    		temporary_q_[i] = NULL;
    	}
    }

    //set copy constructor and assignment operator in a "protected:" zone to prevent uncontrolled copying of class instances
    DpdkEventDevThread(const DpdkEventDevThread&)
    {
    }

    DpdkEventDevThread& operator=(const DpdkEventDevThread&)
    {
        return *this;
    }

    //thread return value
    /*__align(SYS_CACHE_ALIGNMENT)*/ int thread_retval_ = 0;

    __align(SYS_CACHE_ALIGNMENT) IModuleControlCallBackAPI *callback_ptr_;
    ThreadFactory *daddy_p_;
	uint32_t core_num_;
	CMemArea *permanent_q_, *(temporary_q_[NUM_TASK_PRIO]);
    //Thread profiler
    CProfileCnt thread_prof_;
    uint32_t	thread_trace_id_= -1;
    //"thread should be stopped" flag
    volatile /*__align(SYS_CACHE_ALIGNMENT)*/ bool stop_flag_ = true;

    //"thread is running" flag
    volatile /*__align(SYS_CACHE_ALIGNMENT)*/ bool run_flag_ = false;



};


#endif // _DPDK_EVENT_DEV_THREAD_H_
