#include "thread_mgr.h"

//#define TRACE_THREAD_MANAGER
//#define PROFILE_THREAD_MANAGER

//Set affinity, attach the logical thread to some core. If the requested affinity are selected already then return false.
bool DpdkEventDevThread::set_affinity(uint32_t core_num)
{
	bool ret = false;
	ASSERT(run_flag_ == false);
	if(daddy_p_->AllocThread(core_num)== true)
	{
		core_num_ = core_num;
		return true;
	}
	return ret;
}
//Add queue of data engine tasks. This method should be added before the thread is started. Return fifo ID, -1 - error.
bool DpdkEventDevThread::attach_task_fifo(CMemArea *ptr, TASK_QUEUE_T task_queue_t, DETask_priority_t priority)
{
	ASSERT(run_flag_ == false);
	ASSERT(ptr != NULL);
	if(task_queue_t == PERMANENT_TASK_Q)
	{
		ASSERT(permanent_q_ == NULL);
		permanent_q_ = ptr;
		return true;
	}
	else if(task_queue_t == TEMPORAL_TASK_Q)
	{
		ASSERT(priority < NUM_TASK_PRIO);
		ASSERT(temporary_q_[priority] == NULL);
		temporary_q_[priority] = ptr;
		return true;

	}
	return false;
}

//These methods can be called in run time.
//Get affinity number. If the affinity was not set yet, return -1.
uint32_t DpdkEventDevThread::get_affinity()
{
	return core_num_;
}


// Start thread
// \param[in] arg        argument sending to thread member function
// \param[in] lcore_id   id of core in which performed thread
// \return AS_INFRA_RETCODE_ROK or EBUSY
AS_INFRA_RETCODE DpdkEventDevThread::start_thread(void* arg, unsigned lcore_id)
{
    ThreadStaticArg* ThArg = (ThreadStaticArg*)rte_malloc("Thread arguments", sizeof(ThreadStaticArg), 0);
    ThArg->class_ptr       = this;
    ThArg->member_arg      = arg;
    int ret                = rte_eal_remote_launch(static_thread_func, (void*)ThArg, lcore_id);
    if (ret != AS_INFRA_RETCODE_ROK)
        rte_free(arg);
    return (AS_INFRA_RETCODE)ret;
}

// Waiting for thread
// \param[in]  seconds   timeout in seconds to wait
// \return AS_INFRA_RETCODE_ROK if thread complete successfully and AS_INFRA_RETCODE_RFAILED if time is out
AS_INFRA_RETCODE DpdkEventDevThread::wait(uint32_t useconds)
{
	callback_ptr_->IDelay_us(useconds);
    if (!run_flag_)
    {
       return AS_INFRA_RETCODE_ROK;
    }
    return AS_INFRA_RETCODE_RFAILED;
}

// thread functions. User can replace ThreadFunc in derived classes
// \param[in]  arg   some argument sending to thread function
// \return some integer value depend of thread specific
int DpdkEventDevThread::static_thread_func(void* arg)
{
    //copying thread arguments
    ThreadStaticArg CurArg = *(ThreadStaticArg*)arg;
    rte_free(arg);
    CurArg.class_ptr->run_flag_      = true;
    CurArg.class_ptr->stop_flag_     = true;
    CurArg.class_ptr->thread_retval_ = CurArg.class_ptr->thread_func(CurArg.member_arg);
    CurArg.class_ptr->run_flag_      = false;
    return CurArg.class_ptr->thread_retval_;
}
int DpdkEventDevThread::thread_func(void* arg)
{
	uint32_t ret= 0;
	TaskAllocStruct task_data;
	//callback_ptr_->IDelay_us(50); //Timer test
	task_data.cookie= 10;
	uint64_t timing_duration = 0;
	timespec tik;
	timespec tok;
	while(1)
	{
#ifdef TRACE_THREAD_MANAGER
		clock_gettime(CLOCK_MONOTONIC, &tik);
#endif
		int32_t i;
		callback_ptr_->ISyncTime();
		if(stop_flag_== true){
			run_flag_ = false;
			callback_ptr_->IDelay_us(10);
		}
		else
			{
			run_flag_ = true;
			//Check temporary queues
			for(i= TASK_PRIO_0; i < NUM_TASK_PRIO; i++)
			{
				if(temporary_q_[i] != NULL)
				{

					while(temporary_q_[i]->PopFIFO_MT((void*)&task_data, sizeof(task_data)) != NULL)
					{

						callback_ptr_->ITraceData(thread_trace_id_, __LINE__, task_data.cookie);
						thread_prof_.Start();
						task_data.data_engine_p->run(task_data.cookie);
						thread_prof_.Stop();
					}
				}
			}
			if(permanent_q_ != NULL)
			{
				if(permanent_q_->PopFIFO_MT((void*)&task_data, sizeof(task_data)) != NULL)
				{
#ifdef TRACE_THREAD_MANAGER
					callback_ptr_->ITraceData(thread_trace_id_, __LINE__, task_data.cookie);
#endif
#ifdef PROFILE_THREAD_MANAGER
					thread_prof_.Start();
#endif
					task_data.data_engine_p->run(task_data.cookie);
#ifdef PROFILE_THREAD_MANAGER
					thread_prof_.Stop();
#endif
					permanent_q_->PushFIFO_MT((void*)&task_data, sizeof(task_data));
#ifdef TRACE_THREAD_MANAGER
					clock_gettime(CLOCK_MONOTONIC, &tok);
					timing_duration = DiffBetweenTimespec(tok, tik);
					callback_ptr_->ITraceData(thread_trace_id_, __LINE__, timing_duration);
#endif
				}
			}
		}
	}

	return 0;
}


