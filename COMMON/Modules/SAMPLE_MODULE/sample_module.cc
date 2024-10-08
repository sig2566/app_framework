
#include "sample_module.h"

EResultT CSAMPLE_MODULE::IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
{

    /********************************************************************************************/
    // MANDATORY code
    // Initialization of the pointers to the framework services.
	CBaseModule::IInit(callback_ptr, target_api, init_info);
	/*********************************************************************************************/

    uint32_t inc_size= 100; //Size of FIFO element
    uint32_t size= 10; //Number of FIFO elements

    //Example of definition and allocation of memory areas
    DEFINE_MEMAREA(size, inc_size, "TEST_MEM_AREA_2", 0, in_ptr);
    DEFINE_MEMAREA(size, inc_size, "TEST_DATA_POOL", 0, mem_pool_);
    DEFINE_MEMAREA(size, sizeof(JobReqBase_t), "TEST_JOB_REQ", 0, job_req_);
    DEFINE_MEMAREA(size, sizeof(JobRspBase_t), "TEST_JOB_RSP", 0, job_rsp_);

    //Example of log function
    LOG(E_MEDIUM, "%s init passed", mod_name_);

    //Example of initialization of the profiler counter
    call_prof_.Init(MOD_NAME, "EXAMPLE_PROF"); //INIT Profiler counter
    return E_OK;
}
EResultT CSAMPLE_MODULE::IColdStart()
	{
		const char mod_name[]= "TEST_MODULE";

		// Example connection with memory areas
		MEMAREA_CONNECT("TEST_MEM_AREA_2", &out_ptr, E_READ);	// out_ptr -> TEST_MEM_AREA_2, used for data read
		MEMAREA_CONNECT("TEST_MEM_AREA_1", &in_ptr, E_WRITE);	// in_ptr -> TEST_MEM_AREA_1, used for data write
		MEMAREA_CONNECT("TEST_DATA_POOL", &mem_pool_, E_READ);
		MEMAREA_CONNECT("TEST_JOB_REQ", &job_req_, E_WRITE);
		MEMAREA_CONNECT("TEST_JOB_RSP", &job_rsp_, E_READ);

		//Example of getting pointer to other component API
		ASSERT(callback_ptr_->IGetModule(mod_name, &test_module_p_) == E_OK);
		

		return E_OK;
	}
EResultT CSAMPLE_MODULE::ICall(uint32_t param)
	{
		static JobReqBase_t *req;
		if((param == (uint32_t)E_WAKEUP) || (param == (uint32_t)E_DEBUG))
		{
		    //Example of use profiler between to processing points
			struct timespec t1, t2;
			uint32_t num= 0xff, val=0, i;
			req= NULL;
			//Check profiler couner & loG
			call_prof_.Start();		// init activate profiler
			clock_gettime(2, &t1);		// t1 = profiler counter
			for(i=0; i< num; i++)
			{
				uint32_t tmp= i+1;
				val= val + tmp;
			}
			call_prof_.Stop();			// finish profiling measure interval
			clock_gettime(2, &t2);		// t2 = profiler counter
			LOG(E_MEDIUM, "Result of Phibonachi digit calculation is %d s1= %ld s2=%ld",val, t1.tv_nsec, t2.tv_nsec);

			// Example access to the memory areas as to the FIFO
			char *in_p= (char*)in_ptr->PopFIFO();
			if(in_p != NULL)
			{
				char *out_p= (char*)out_ptr->AllocFIFO();
				ASSERT(out_p);
				
				memcpy(out_p, in_p, in_ptr->GetDataSize());
				out_ptr->PushFIFO();
			}

			//Example sending Job to the TEST_MODULE
			if(param == (uint32_t)E_DEBUG)
			{
			    //Example of prepare new job request
				req = (JobReqBase_t*)job_req_->AllocFIFO();
				uint32_t tmp;
				ASSERT(req != NULL);
				req->call_back_p= (IModuleControlAPI*)this;
				req->req_id = 1;
				//Example of dynamic memory allocation of data junks
				req->in_buf = mem_pool_->AllocPoolChunk();
				req->out_buf = mem_pool_->AllocPoolChunk();
				*(uint32_t*)req->in_buf = -1;
				*(uint32_t*)req->out_buf = 0;
				req->job_cmd = GEN_JOB;
				job_req_->PushFIFO();

				//Example of call the other module
				test_module_p_->ICall((uint32_t)E_DIRECT);
				LOG(E_MEDIUM, "%s sent job to other module", mod_name_);

			}

		}

		//Example of handle response from other module
		if(param == (uint32_t)E_RESPONSE)
		{
			JobRspBase_t *resp = (JobRspBase_t *)job_rsp_->PopFIFO();
			ASSERT(resp != NULL);

			if((resp->req_id == 1) && (resp->res == E_OK))
			{
				if(*(uint32_t*)req->out_buf == -1)
				{
					LOG(E_MEDIUM, "Response ok, test passed");
					return E_OK;
				}
			}
			LOG(E_MEDIUM, "Response not ok, test failed");
		}
		return E_OK;
	}
EResultT CSAMPLE_MODULE::IConfigure(EConfigId id, void *in, void **out)
	{
		if(id == e_GET_API){
			*out = (void*)static_cast<sample_module_api*>(this);
		}
		char str[]= "Call passed";
		callback_ptr_->ILogData(E_LOW, str);
		return E_OK;
	}

uint32_t CSAMPLE_MODULE::calc_sum_uint_16()
{
	uint32_t num=6;
	int i;
	uint32_t sum=0;
	const int16_t arr_p[] = {1,2,3,4,5,6};
	for (i=0; i< num; i++)
	{
		sum += arr_p[i];
	}
	return sum;
}

CSAMPLE_MODULE::CSAMPLE_MODULE(const char *mod_name)
	{

		in_ptr = NULL;
		out_ptr = NULL;
		test_module_p_ = NULL;
		mem_pool_ = NULL;
		job_req_ = NULL;
		job_rsp_ = NULL;
	}

	CSAMPLE_MODULE::CSAMPLE_MODULE()
	{

		const_data_module_p_ = NULL;
		in_ptr = NULL;
		out_ptr = NULL;
		test_module_p_ = NULL;
		mem_pool_ = NULL;
		job_req_ = NULL;
		job_rsp_ = NULL;


	}



class CSAMPLE_MODULE     *g_module_ptr = NULL;

//API function to connect with ITarget API
extern "C" uint32_t API_CONNECT_FUNC(void **target_ptr)
{
	g_module_ptr = new CSAMPLE_MODULE();
	*target_ptr= static_cast<IModuleControlAPI*>(g_module_ptr);
	return 0;
}
