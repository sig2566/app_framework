/******************************************************************
 * I_cpp_module.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include <time.h>

#ifndef API_I_CPP_MODULE_H_
#define API_I_CPP_MODULE_H_

/******************************************************************
 *@brief	This function advance timestamp with value of nano-seconds
 *@brief	in format of timespec (old format - used by build oran packets to advance time between packets/symbols)
 *@brief
 *@brief	Input:	original timespec
 *@brief					 advance time in nano-seconds
 *@brief	Output:	new timespec
 *@brief
 *******************************************************************/

static timespec  Preceeding(timespec t1,uint32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec+(uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
/******************************************************************
 *@brief	This function move back timestamp with value of nano-seconds
 *@brief	in format of timespec (old format - used by build oran packets to advance time between packets/symbols)
 *@brief
 *@brief	Input:	original timespec
 *@brief					 advance time in nano-seconds
 *@brief	Output:	new timespec
 *@brief
 *******************************************************************/
static timespec  Decreasing(timespec t1,int32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec - (uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
/******************************************************************
 *@brief	This function decrease time of  timeval with value of micro-seconds
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in micro-seconds
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimevalUsec(timeval t1,uint32_t usec_value)
{
	timeval new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * MILLION + (uint64_t)t1.tv_usec-(uint64_t)usec_value;
	new_t.tv_usec = t64 %MILLION;
	new_t.tv_sec=(t64-t64 %MILLION)/MILLION;
	return new_t;
}

/******************************************************************
 *@brief	This function decrease time of  timeval with value of seconds
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in seconds
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimevalSec(timeval t1,uint32_t sec_value)
{
	timeval new_t=t1;
	uint64_t t64;
	if(t1.tv_sec>=sec_value)
		{new_t.tv_sec=new_t.tv_sec-sec_value;}
	else
	{
		printf("Time can't be negative!!\n");
		ASSERT(0);
	}
	return new_t;
}

/******************************************************************
 *@brief	This function decrease time of timeval with value of another timeval
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in timeval
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimeval(timeval orig,timeval timeval_value)
{
	timeval timeval_result;
	timeval_result=DecreasingTimevalUsec(orig,timeval_value.tv_usec);
	timeval_result=DecreasingTimevalSec(timeval_result,timeval_value.tv_sec);
	return timeval_result;
}

/******************************************************************
 *@brief	This function increase time of timeval with value of another timeval
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 advance time in timeval
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval PreceedingTimevalUsec(timeval t1,uint32_t usec_value)
{
	timeval new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * MILLION + (uint64_t)t1.tv_usec+(uint64_t)usec_value;
	new_t.tv_usec = t64 %MILLION;
	new_t.tv_sec=(t64-t64 %MILLION)/MILLION;
	return new_t;
}


//This function substract the time by ns_value
static timespec  ReverseTime(timespec t1,uint32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec-(uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
static __syscall_slong_t DiffBetweenTimespec  (timespec t1,timespec t2)
{
	timespec new_t;
	uint64_t t64_1,t64_2;
	__syscall_slong_t t64_3;
	t64_1= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
	t64_2= (uint64_t)t2.tv_sec * BILLION + (uint64_t)t2.tv_nsec;
	t64_3=t64_1-t64_2;
	return t64_3;
}

namespace ns_5g_phy
{
	enum EJob_type
	{
		NO_JOB,
		GEN_JOB //GO!!
	};


	/******************************************************************************************//*!
	 *@class JobReqBase_t
	 *@brief The purpose of this class is :
	 *@brief It is example of JOB request class, which is used to send request between components.
	 *@brief This structure might be used as base class for job request types.
	 *********************************************************************************************/
	struct JobReqBase_t
	{
		uint32_t 		req_id;
		EJob_type	job_cmd;
		void_t  		*in_buf;
		void_t  		*out_buf;
		IModuleControlAPI *call_back_p;

		void Reset()
		{
			req_id = -1;
			job_cmd = NO_JOB;
			in_buf = NULL;
			out_buf = NULL;
			call_back_p = NULL;
		}

	};

    /******************************************************************************************//*!
     *@class JobRspBase_t
     *@brief The purpose of this class is :
     *@brief It is example of JOB response class, which is used to send response after job end to master component.
     *@brief This structure might be used as base class for job response types.
     *********************************************************************************************/

	struct JobRspBase_t
	{
		uint32_t req_id;
		EResultT res;
	};

   /******************************************************************************************//*!
    *@class CProfileCnt
    *@brief The purpose of this class is :
    *@brief This class implements the profiler counter.
    *********************************************************************************************/
	enum EProfileEval_type
	{
		TIME_EVAL,
		CYCLE_EVAL
	};

	class CProfileCnt
	{

		ProfilerCntD prof_cnt_;
		uint64_t acc_cnt_; //Support multiple start stop during one measurement
		uint32_t cnt_id_;
		uint32_t max_calls_;
		uint64_t start_val_;
		uint64_t delta_;
		uint32_t	prof_id = -1;
		CProfileCnt  *next_prof_p;
		EProfileEval_type 	evalType_;
		IModuleControlCallBackAPI *callback_ptr_ = NULL;


	public:
	CProfileCnt()
	{
		cnt_id_ =0;
		prof_cnt_.Reset();
		max_calls_= 500;
		acc_cnt_ = 0;
		start_val_ = 0;
		delta_ = 0;
		next_prof_p = NULL;
		evalType_ = TIME_EVAL;//TIME_EVAL;//CYCLE_EVAL;
	}
	CProfileCnt* GetNextProf()
	{
	    return next_prof_p;
	}
	void ConnectProf(CProfileCnt *prof_addr)
	{
	    next_prof_p = prof_addr;
	}

	void SetMaxCalls(uint32_t val)
	{
		max_calls_= val;
	}
	void Init(char *name, IModuleControlCallBackAPI *callback_ptr)
	{
	    ProfilerCntD* read_p;
	    struct timespec t1, t0;
	    callback_ptr_ = callback_ptr;
		callback_ptr->IRegistryProfileEntry(this, name, &prof_id);
		prof_cnt_.Reset();
		Reset();
		if(evalType_ == TIME_EVAL)
		{
			int8_t number_delta_calls_iterations = 10;
			delta_ = 0;
			for(int8_t i=0; i<number_delta_calls_iterations; i++)
			{
				clock_gettime(CLOCK_MONOTONIC, &t0);
				clock_gettime(CLOCK_MONOTONIC, &t1);
				delta_ += (((uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec) - ((uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec));
			}
			delta_ /= number_delta_calls_iterations; //average amount of clocks for calling gettime

		}
		else
		{
			// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
			delta_ 		= tsc();
			start_val_	= tsc();
			delta_ 		= tsc();
			start_val_	= tsc();
			delta_ = start_val_ - delta_;
			start_val_ = 0;

		}

	}
	void Update()
	{
	    ProfilerCntD tmp;
	    if(prof_cnt_.meas_num_ == 0)
	        return;
		prof_cnt_.average_cnt_ = prof_cnt_.average_cnt_/prof_cnt_.meas_num_;

		callback_ptr_->ISaveProfileInfo(prof_id, &prof_cnt_);
		prof_cnt_.Reset();
	}
	void Start()
	{
		ASSERT(start_val_== 0);
		//__sync_synchronize();
		struct timespec t1, t0;
        prof_cnt_.last_cnt_ =1; //Informs that the Start function was called

        if(evalType_ == TIME_EVAL)
		{
        	//clock_gettime(CLOCK_MONOTONIC, &t0);
			clock_gettime(CLOCK_MONOTONIC, &t1);
			//delta_ = (uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec;
			start_val_ = (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
		}
		else
		{
			// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
			start_val_	= tsc();
		}
		//__sync_synchronize();
	}

#if defined(__x86_64__) || defined(__amd64__)
inline	uint64_t tsc(void)
	{
	    uint32_t lo, hi;
	    asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	    return ((uint64_t)hi << 32UL) | (uint32_t)lo;
	}
#elif defined(__aarch64__)
inline	uint64_t tsc(void)
	{

 /*
// this code should work on our ARM processor since it's ver8 architecture, but it does not recognize "mrc" assembly command for some reason
#if (__ARM_ARCH >= 6)
		uint32_t pmccntr;
		uint32_t pmuseren;
		uint32_t pmcntenset;
		  // Read the user mode perf monitor counter access permissions.
		  asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
		  if (pmuseren & 1) {  // Allows reading perfmon counters for user mode code.
		    asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
		    if (pmcntenset & 0x80000000ul) {  // Is it counting?
		      asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
		      // The counter is set up to count every 64th cycle
		      return static_cast<uint64_t>(pmccntr) * 64;  // Should optimize to << 6
		    }
		  }
#endif
 */

		  // System timer of ARMv8 runs at a different frequency than the CPU's.
		  // The frequency is fixed, typically in the range 1-50MHz.  It can be
		  // read at CNTFRQ special register.  We assume the OS has set up
		  // the virtual timer properly.

		int64_t virtual_timer_value;
		asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
		return (uint32_t)virtual_timer_value;
	}
#endif


	void StopContinue()
	{
		struct timespec t1;
		uint64_t t64;
		if(evalType_ == TIME_EVAL)
		{
			clock_gettime(CLOCK_MONOTONIC, &t1);
			t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
		}
		else
		{
			t64 = tsc();
		}
		acc_cnt_ += t64 - start_val_;
		start_val_ = 0;

	}
	void ForceStop()
	{
	    Update();
	    Reset();
	}
	void Stop(uint64_t *prof_valp = NULL)
	{
		//__sync_synchronize();
		struct timespec t1;
		volatile uint64_t t64;
		if(prof_valp==NULL)
		{
			//Calculate time profiling between calling start and stop functions
			if(evalType_ == TIME_EVAL)
			{
				clock_gettime(CLOCK_MONOTONIC, &t1);
				t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
			}
			else
			{
				t64 = tsc();
			}

			int64_t tmp_val = t64 - start_val_ + acc_cnt_ - delta_;// check if calling to both, the tested code and gettime function, took less time than delta_ time (it can happen in case of caching issues)
			if(tmp_val < 0)
			{
				prof_cnt_.last_cnt_ = 20; //add artificial small number of nsecs (20nsec is about 30 cycles for 1.6 Ghz CPU clock)
			}
			else
			{
				prof_cnt_.last_cnt_= tmp_val;
			}
		}
		else
		{
			//Calculate profiling of any values
			prof_cnt_.last_cnt_= *prof_valp;
		}
		//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_ - delta_;
		//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_; // force delta_ = 0 since there is sometime values of the counter less than delta and then we get negative measurement
		//ASSERT(delta_ < 10000L)
		//ASSERT((t64 - start_val_) < 100000000000L)
		//ASSERT(acc_cnt_ == 0)
		//ASSERT(prof_cnt_.last_cnt_ < 100000000000L)
#ifdef yafit
		if(prof_cnt_.last_cnt_ > 100000000000L)
		{
			printf("t64=%llu, start_val_=%llu, acc_cnt_=%llu, delta_=%llu\n", t64, start_val_, acc_cnt_, delta_);
		}
#endif
		acc_cnt_ = 0;
		if(prof_cnt_.last_cnt_ > prof_cnt_.max_cnt_)
		{
			prof_cnt_.max_cnt_ = prof_cnt_.last_cnt_;
			prof_cnt_.max_cnt_time_ = start_val_;
		}
		prof_cnt_.average_cnt_ += prof_cnt_.last_cnt_;
		start_val_ = 0;
		prof_cnt_.meas_num_++;
		if((max_calls_ !=0) &&(  max_calls_ <= prof_cnt_.meas_num_))
		{
			Update();
		}
		//__sync_synchronize();
	}


	void Reset()
	{
	    prof_cnt_.Reset();
		start_val_ = 0;
		acc_cnt_ = 0;
	}

	};
} //namespace 
#define LOG(severity, ... )    {\
	char buf[100];\
	sprintf(buf,__VA_ARGS__);\
	callback_ptr_->ILogData(severity, buf);\
	}
#define LOG_module(severity, ... )    {\
	char buf[100];\
	sprintf(buf,__VA_ARGS__);\
	callback_framework_class_->ILogData(severity, buf);\
	}

#define DEFINE_MEMAREA(size, inc_size, name, Attrib, area_ptr ) {\
	char tmp_name[]= name;\
	CMemArea tmp_area;\
	area_ptr = &tmp_area;\
	tmp_area.Setup(size, inc_size, tmp_name, E_READ, Attrib );\
	EResultT res=  callback_ptr_->IMemAreaDefine(&area_ptr);\
	ASSERT(res == ns_5g_phy::E_OK);\
	}

#define MEMAREA_CONNECT(name, area_ptr, access_t) {\
	char tmp_name[]= name;\
	EResultT res=  callback_ptr_->IMemAreaMount(area_ptr, tmp_name , access_t);\
	ASSERT(res == ns_5g_phy::E_OK);\
	}

#define INIT_PROF_CNT(name, call_back_ptr, prof_cnt){\
	    char tmp_name[]= name;\
	    prof_cnt.Init(tmp_name, call_back_ptr);\
	    }

#endif
