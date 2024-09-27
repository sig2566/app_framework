/******************************************************************
 * i_cpp_sys_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef I_COMP_API_H_
#define I_COMP_API_H_
//#include <asm/cachectl.h>
//#include <asm/unistd.h>
#include "rt_debug_defs.h"
#include "rt_debug_api.h"
#include "rt_debug_cpp_defs.h"

//	Use this macro in order to ignore compiler alignment of data structures
//	that are not aligned:
#define PACK_STRUCT __attribute__((packed))


#include "i_osa_api.h"
//API definitions for 5G simulator modules
#define MDO_NAME_SIZE    (80)
namespace ai_framework_proj
{
	enum EResultT
	{
		E_OK,
		E_FAIL
	};

	enum EAccessT
	{
		E_READ,
		E_WRITE
	};
	enum ESeverityT
	{
		E_CRITICAL,
		E_HIGH,
		E_MEDIUM,
		E_LOW,
		E_BP,
		E_INFO
	};
	enum ECallMethodT
	{
		E_PERIODIC,
		E_ONCE
	};

	enum EDebugOperationT
	{
		E_VERSION,
		E_PROFILER,
		E_MAX_OP
	};

	struct TraceDataT
	{
		uint32_t trace_id;
		uint32_t val0, val1, val2, val3;
	};
	enum EInterfaceT
	{
		E_CONTROL_API,
		E_DEBUG_API
	};

	enum ECallReasonT
	{
		E_WAKEUP,
		E_DEBUG,
		E_DIRECT,
		E_RESPONSE,
		E_MAX_CALL_REASON
	};

	//These enums are used for IConfigure method
	enum EConfigId
	{
	    e_GENERAL,
	    e_GET_API,
		e_GET_TEST_MODULE_API,
	    e_TEST_CONFIG,
l,		e_PUT_INIT_DATA
	};

	enum EMemAllocBit
	{
		e_DEFINE_WITHOUT_ALLOCATE_BIT_NUM,
		e_SHR_CONSUMER,
		e_SHR_PRODUCER,
		e_NOT_IN_USE_3,
		e_NOT_IN_USE_4,
		e_NOT_IN_USE_5,
		e_NOT_IN_USE_6,
		e_NOT_IN_USE_7,
		e_NOT_IN_USE_8,
		e_NOT_IN_USE_9,
		e_NOT_IN_USE_10,
		e_NOT_IN_USE_11,
		e_NOT_IN_USE_12,
		e_NOT_IN_USE_13,
		e_NOT_IN_USE_14,
		e_NOT_IN_USE_15,
		e_NOT_IN_USE_16,
		e_NOT_IN_USE_17,
		e_NOT_IN_USE_18,
		e_NOT_IN_USE_19,
		e_NOT_IN_USE_20,
		e_NOT_IN_USE_21,
		e_NOT_IN_USE_22,
		e_NOT_IN_USE_23,
		e_NOT_IN_USE_24,
		e_NOT_IN_USE_25,
		e_NOT_IN_USE_26,
		e_NOT_IN_USE_27,
		e_NOT_IN_USE_28,
		e_NOT_IN_USE_29,
		e_NOT_IN_USE_30,
		e_NOT_IN_USE_31
	};



#if		(ARCH == INTEL)

	#define FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)		//nothing to do on Intel CPU
	#define	INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)	//nothing to do on Intel CPU

#elif 	(ARCH == ARM)

	#define FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr) 	__builtin___clear_cache(chunk_start_ptr, chunk_end_ptr)
	#define	INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)	//should find-out how to perform that on ARM processor
#else

	#error	No valid architecture detected !!!

#endif

	class ITarget;
	class RSE_ControlAPI;
	class IModuleControlAPI;
	class CProfileCnt;

	/*******************************************************************************************//*!
	*@class CMemArea
	*@brief The purpose of this class is :
	*@brief Memory management. The class provides the following services:
	*@brief Explicit memory allocation by Framework.
	*@brief Inter-module thread safe data transfer.
	*@brief FIFO support
	*@brief Dual Buffer support
	*@brief Dynamic memory allocation of memory chunks with the same size.
	***********************************************************************************************/
	class CMemArea
	{
	protected:
	  void *allocated_ptr_; // for saving the original allocated pointer (in case of release required)
	  void *ptr_producer_;
	  void *ptr_consumer_;
	  uint32_t number_chunks_;
	  uint32_t single_chunk_size_;
	  volatile __align(SYS_CACHE_ALIGNMENT) uint32_t wr_ind_; // make write pointer be aligned to the cache-line size of the processor
	  volatile __align(SYS_CACHE_ALIGNMENT) uint32_t rd_ind_; // make read pointer be aligned to the cache-line size of the processor
	  __align(SYS_CACHE_ALIGNMENT) EAccessT access_;
	  uint32_t attrib_;
	  OSA_spinlock_t in_lock_;
	  OSA_spinlock_t out_lock_;
	public:
	  char	name_[MDO_NAME_SIZE];
	  CMemArea()
	  {
		  Init();
	  }
	  void Setup(uint32_t size, uint32_t inc_size, char *name, EAccessT access= E_READ, uint32_t attrtib=0 )
	  {
		  rd_ind_ = 0;
		  wr_ind_ = 1;
		  number_chunks_= size;
		  single_chunk_size_= ALIGN_SIZE(inc_size, SYS_CACHE_ALIGNMENT);
		  access_ = access;
		  attrib_ = attrtib;
		  strcpy(name_, name);
		  OSA_spinlock_create(&in_lock_);
		  OSA_spinlock_create(&out_lock_);
	  }
	  void MemAlloc(void *mem_buf = NULL)
	  {
		  uint64_t pointer_int;
		  uint32_t total_size= single_chunk_size_*number_chunks_;
		  total_size = ADD_SIZE_FOR_MEM_ALIGN(total_size, SYS_CACHE_ALIGNMENT);
		  ASSERT(total_size >0);

		  if(mem_buf== NULL)
		  {
			  if(!(attrib_>>e_DEFINE_WITHOUT_ALLOCATE_BIT_NUM)&0x1)
			  {
				  allocated_ptr_= new char [total_size];
				  ASSERT(allocated_ptr_!=NULL);
				  pointer_int = ALIGN_POINTER((uint64_t)allocated_ptr_, SYS_CACHE_ALIGNMENT); // align the memory pointer to the Cache line size
				  ptr_producer_ = ptr_consumer_= (void*)pointer_int;
			  }
		  }
		  else
		  {
			  bool consumer = (attrib_>> e_SHR_CONSUMER) && 1;
			  bool producer = (attrib_>> e_SHR_PRODUCER) && 1;

			  ASSERT(ALIGN_64BIT((uint64_t)mem_buf)==(uint64_t)mem_buf); //Check that buffer address is aligned to 64 bytes (cache line size)

			  if((producer== false) && (consumer== false))
			  {
				  //No care producer/consumer means both
				  producer= true;
				  consumer= true;
			  }

			  if(producer)
			  {
				  ptr_producer_ = mem_buf;
			  }
			  if(consumer)
			  {
				  ptr_consumer_= mem_buf;
			  }

		  }

	  }
	  uint32_t GetAttrib()
	  {
		  return attrib_;
	  }
	  char* GetName()
	  {
		  return name_;
	  }
	  void SetAccess(EAccessT access)
	  {
		  access_ = access;
	  }
	  EAccessT GetAccess()
	  {
		  return access_;
	  }
	  void Init()
	  {
		  ptr_producer_= NULL;
		  ptr_consumer_= NULL;
	  }

	  uint32_t GetDataSize()
	  {
		 return single_chunk_size_;
	  }


	  uint32_t GetTotalDataSize()
	  {
		 return single_chunk_size_*number_chunks_;
	  }

	  // Methods below support access to the memory area as double buffer (used for situation when one thread update data and many other threads read updated data)
	  //***********************************************************************************************************************************************************
	  void *GetReadBufP()
	  {

		  return (void*)((char*)ptr_consumer_ + single_chunk_size_ * rd_ind_);
	  }

	  void *GetWriteBufP()
	  {
		 return (void*)((char*)ptr_producer_ + single_chunk_size_ * wr_ind_);;
	  }

	  void DualBufSwap()
	  {
		  PushFIFO();
		  PopFIFO();
	  }
	  //*******************************************************************************************************

	  // Methods below allow access to the memory area as FIFO (one direction data exchange between two threads)
	  //********************************************************************************************************
	  void *AllocFIFO()
	  {
		  return GetWriteBufP();
	  }
	  void *PushFIFO()
	  {
		  uint32_t tmp = (wr_ind_+1)%number_chunks_;
		  if(tmp == rd_ind_)
		  {
			  return NULL;
		  }

		  wr_ind_ = tmp;
		  void* chunk_start_ptr = AllocFIFO();
		  uint64_t chunk_end = (uint64_t)chunk_start_ptr+single_chunk_size_-1;
		  void* chunk_end_ptr = (void*)chunk_end;
		  //flush the cache lines of the new pushed chunk
		  FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr);
		  return chunk_start_ptr;
	  }
	  void *PopFIFO()
	  {
		  uint32_t tmp = (rd_ind_+1)%number_chunks_;
		  if(tmp == wr_ind_)
		  {
			  return NULL;
		  }
		  rd_ind_ = tmp;

		  void* chunk_start_ptr = GetReadBufP();
		  uint64_t chunk_end_ptr = (uint64_t)chunk_start_ptr+single_chunk_size_-1;
		  //void* chunk_end_ptr = (void*)chunk_end;
		  //invalidate the cache lines of the new chunk poped
		  INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr);
		  return chunk_start_ptr;

	  }
	  //**********************************************************************************************************
	  //The methods below allow use the memory area as dynamic heap
	  void *AllocPoolChunk()
	  {
		  void *ptr;
	      //spinlock_push check/allocate
	      OSA_spinlock_lock(&out_lock_);

		  wr_ind_ = (wr_ind_+1)%number_chunks_;
		  ptr = AllocFIFO();
		  OSA_spinlock_unlock(&out_lock_);
		  return ptr;
	  }

	  void ReleaseChunk(void *ptr)
	  {

	  }

	  void *GetPrevChunk(uint32_t prev_idx)
	  {
	      uint32_t prev_abs_indx;
		  bool consumer = (attrib_>> e_SHR_CONSUMER) && 1;
		  bool producer = (attrib_>> e_SHR_PRODUCER) && 1;
		  char *ptr= NULL;
		  if((producer== false) && (consumer== false))
		  {
			  //No care producer/consumer means both
			  producer= true;
			  consumer= true;
		  }

		  if(producer)
		  {
			  ptr= (char*)ptr_producer_ ;
		  }
		  else
		  {
			  ptr =(char*)ptr_consumer_;
		  }

	      ASSERT(prev_idx < number_chunks_);
	      prev_abs_indx = (number_chunks_ + wr_ind_ - prev_idx) % single_chunk_size_;
	      return (void*)(ptr + single_chunk_size_ * prev_abs_indx);;
	  }
	  //************************************************************************************************************
	  //   Methods below is used for multi-threaded FIFO, where number threads may run simultaneously on producer and consumer side.
	  //************************************************************************************************************
	  EResultT PushFIFO_MT(void *data_p, uint32_t data_size)
      {
	      //spinlock_push check/allocate
	      OSA_spinlock_lock(&in_lock_);
	      EResultT res = E_OK;
          uint32_t tmp = (wr_ind_+1)%number_chunks_;
          if(tmp == rd_ind_)
          {
              res = E_FAIL;
          }
          else
          {
              void *dest = GetWriteBufP();
              ASSERT(single_chunk_size_ >= data_size);
              memcpy(dest, data_p, data_size);
              wr_ind_ = tmp;
          }
          //Release spinlock_push
          OSA_spinlock_unlock(&in_lock_);
          return res;
      }
      void *PopFIFO_MT(void *data_p, uint32_t data_size)
      {
          // Add spinlock_pop check/allocate
          OSA_spinlock_lock(&out_lock_);
          uint32_t tmp = (rd_ind_+1)%number_chunks_;
          void *res = NULL;
          if(tmp != wr_ind_)
          {
        	  rd_ind_ = tmp;
              res = GetReadBufP();
              ASSERT(single_chunk_size_ >= data_size);
              memcpy(data_p, res, data_size);
          }
          //Release spinlock_pop
          OSA_spinlock_unlock(&out_lock_);
          return res;
      }


	} ;

	/*******************************************************************************************//*!
	*@class StaticMemArea template
	*@brief The purpose of this class is :
	*@brief Memory management. The class provides the following services:
	*@brief Static memory allocation by Framework.
	*@brief Inter-module thread safe data transfer.
	*@brief FIFO support
	*@brief Dual Buffer support
	*@brief Dynamic memory allocation of memory chunks with the same size.
	***********************************************************************************************///
	template<class T, int32_t CHUNKS_NUM>class StaticMemArea : public CMemArea
	{
		__align(SYS_CACHE_ALIGNMENT) T buf_[CHUNKS_NUM];
	public:
		  void *GetReadBufP()
		  {

			  return (void*)((char*)buf_ + single_chunk_size_ * rd_ind_);
		  }

		  void *GetWriteBufP()
		  {
			 return (void*)((char*)buf_ + single_chunk_size_ * wr_ind_);;
		  }
		  void Setup(uint32_t size, uint32_t inc_size, char *name, EAccessT access= E_READ, uint32_t attrib=0 )
		  {
			  CMemArea::Setup(CHUNKS_NUM, sizeof(T), name, access, attrib);
		  }
		  void MemAlloc(void *mem_buf = NULL)
		  {
			  CMemArea::MemAlloc((void*)buf_);
		  }

		  T* GetEntry(uint32_t num)
		  {
			  ASSERT(num<CHUNKS_NUM);
			  return &buf_[num];
		  }
		  EResultT PushFIFO_MT(void *data_p, uint32_t data_size)
	      {
		      //spinlock_push check/allocate
		      OSA_spinlock_lock(&in_lock_);
		      EResultT res = E_OK;
	          uint32_t tmp = (wr_ind_+1)%number_chunks_;
	          if(tmp == rd_ind_)
	          {
	              res = E_FAIL;
	          }
	          else
	          {
	              void *dest = GetWriteBufP();
	              ASSERT(single_chunk_size_ >= data_size);
	              memcpy(dest, data_p, data_size);
	              wr_ind_ = tmp;
	          }
	          //Release spinlock_push
	          OSA_spinlock_unlock(&in_lock_);
	          return res;
	      }
	      void *PopFIFO_MT(void *data_p, uint32_t data_size)
	      {
	          // Add spinlock_pop check/allocate
	          OSA_spinlock_lock(&out_lock_);
	          uint32_t tmp = (rd_ind_+1)%number_chunks_;
	          void *res = NULL;
	          if(tmp != wr_ind_)
	          {
	        	  rd_ind_ = tmp;
	              res = GetReadBufP();
	              ASSERT(single_chunk_size_ >= data_size);
	              memcpy(data_p, res, data_size);
	          }
	          //Release spinlock_pop
	          OSA_spinlock_unlock(&out_lock_);
	          return res;
	      }
		  void *AllocPoolChunk()
		  {
			  void *ptr;
		      //spinlock_push check/allocate
		      OSA_spinlock_lock(&out_lock_);

			  wr_ind_ = (wr_ind_+1)%number_chunks_;
			  ptr = GetWriteBufP();
			  OSA_spinlock_unlock(&out_lock_);
			  return ptr;
		  }


	};
	typedef CMemArea* CMemAreaP;
	struct CModuleInfo
	{
		IModuleControlAPI*  module_p_;
		char				module_name_[MDO_NAME_SIZE];
		void Init()
		{
			module_name_[0]= 0;
			module_p_ = NULL;
		}
	};


	//Module APIs
	/*******************************************************************************************//*!
	*@class IModuleControlCallBackAPI
	*@brief The purpose of this class is :
	*@brief API for asynchronous services, provided by the framework to the 5G moduler
	***********************************************************************************************/
	class IModuleControlCallBackAPI
	{
	public:
		/**!
		* @Function: EResultT IMemAreaDefine(CMemAreaP *mearea_ptr);
		* @Purpose:
		* Define and allocate FIFO Memory area object. It creates it in RSE with specific name and returns the FIFO producer pointer.
		* @Parameters:
		* mearea_ptr - any control value.
		**/
		virtual EResultT IMemAreaDefine(CMemAreaP *mearea_ptr) = 0;
		virtual EResultT IRegistryTraceEntry(char *format_str, uint32_t *id) = 0;
		virtual EResultT IRegistryProfileEntry(CProfileCnt *ptr, const char *name, uint32_t *prof_id) = 0;
		virtual EResultT ILogData(ESeverityT severity, char *str) = 0;
		virtual EResultT ITraceData(uint32_t id, uint32_t line_num, uint64_t val0= 0, uint64_t val1=0, uint64_t val2=0, uint64_t val3=0) = 0;
		virtual EResultT IStopRequest(ESeverityT severity) = 0;
		virtual EResultT IGetModule(const char mod_name[], IModuleControlAPI **mod_ptr) = 0;

		/**!
		* @Function: EResultT IMemAreaMount(CMemAreaP *mearea_ptr, char area_name[], EAccessT ac_type) ;
		* @Purpose:
		* Connection
		* @Parameters:
		* param - any control value.
		**/
		virtual EResultT IMemAreaMount(CMemAreaP *mearea_ptr, char area_name[], EAccessT ac_type) = 0;
		virtual EResultT IDelay_us(uint32_t usecs)= 0;
		virtual EResultT IAllocateEventCnt(const char *cnt_name, volatile int64_t **cnt_ptr)= 0;
		virtual EResultT ISaveProfileInfo(uint32_t prof_id, ProfilePoint *data) = 0;
	};

	/*******************************************************************************************//*!
	*@class IModuleControlAPI
	*@brief The purpose of this class is :
	*@brief API for access to the module
	***********************************************************************************************/
	class IModuleControlAPI
	{
	public:
		/**!
		* @Function: EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info)
		* @Purpose: Module data initialization. Initialization includes the following major stages:
		* Call Initialization for CBaseModule class: CBaseModule::IInit(callback_ptr, target_api, init_info);
		* Allocates memories and objects
		* Allocates producer side CMemArea FIFOs if the there are data transfer with other modules.
		* It handles  the following parameters:
		* @Parameters:
		* callback_ptr - pointer to IModuleControlCallBackAPI callback_ptr. This pointer allows access to RSE services
		* target_api - pointer to RSE control
		* init_info - char* array of some initial information. NULL if there is not such information.
		**/
		virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api, const char *init_info) = 0;
		/**!
		* @Function: EResultT IColdStart();
		* @Purpose: The following tasks my be done if necessary.
		* Connection with other modules via CMemArea FIFOs.
		* Import and export of pointers of APIs
		* Initialization of the cold start data
		* @Parameters: None
		**/
		virtual EResultT IColdStart() = 0;
		/**!
		* @Function: EResultT IWarmStart();
		* @Purpose: The following tasks my be done if necessary.
		* Connection with other modules via CMemArea FIFOs.
		* Preparing threads, mutexes, semaphores, etc.
		* Initialization of the warm start data
		* @Parameters: None
		**/
		virtual EResultT IWarmStart() = 0;
		/**!
		* @Function: EResultT IHotStart();
		* @Purpose: The following tasks my be done if necessary.
		* Preparing threads, mutexes, semaphores, etc.
		* Initialization of the hot start data
		* @Parameters: None
		**/
		virtual EResultT IHotStart() = 0;
		/**!
		* @Function: EResultT IStop();
		* @Purpose:
		* Informing the module that running was stop.
		* All modules threads should be stopped.
		* @Parameters: None
		**/
		virtual EResultT IStop(ESeverityT severity) = 0;
		/**!
		* @Function: EResultT ICall();
		* @Purpose:
		* Call the module if it runs in the steps mode.
		* @Parameters:
		* param - any control value.
		**/
		virtual EResultT ICall(uint32_t param) = 0;

		/**!
		* @Function: EResultT IConfigure(EConfigId id, void *in, void **out);
		* @Purpose:
		*Making additional configuration to the module.
		* @Parameters:
		* id - type of configuration: There are the following configuration types:
		* e_GENERAL, - General configuration information
	    * e_GET_API, - Export specific API to other module. "in" parameter points to string with the API name, "out" is the returned API value.
		* e_GET_TEST_MODULE_API, - Export specific test API if necessary.
	    * e_TEST_CONFIG, - Configure the test running.
		* e_PUT_INIT_DATA - Send additional initialization data if necessary.
		* in - Specific receiving data
		* out - Specific data to send out
		**/
		virtual EResultT IConfigure(EConfigId id, void *in, void **out) = 0;

		/**!
		* @Function: EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info)
		* @Purpose:
		* Get module version information.
		* @Parameters:
		* version information..
		**/
		virtual EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info) = 0;
	};

	//Front End APIs
	/*******************************************************************************************//*!
	*@class RSE_ControlCallBackAPI
	*@brief The purpose of this class is :
	*@brief API RSE callback API
	***********************************************************************************************/
	class IRSE_ControlCallBackAPI
	{
	public:
		virtual EResultT IStopRequest(ESeverityT severity) = 0;
		virtual EResultT I_TTI_evt() = 0;
	};

	/*******************************************************************************************//*!
	*@class IRSE_DebugAPI
	*@brief The purpose of this class is :
	*@brief Debugger API to the running support component. That API allows the system debugging from
	*@brief the externas application (front end).
	***********************************************************************************************/
	class IRSE_DebugAPI	{
	public:
		virtual EResultT  	IDebugInit(const char* add_info) = 0;
		virtual EResultT  	IGetMemAreasNum(uint32_t *areas_num) = 0;
		virtual CMemArea*  	IGetMemArea(uint32_t area_num) = 0;
		virtual EResultT 	IGetModulesNum(uint32_t *modules_num) = 0;
		virtual CModuleInfo IGetModule(uint32_t module_num) = 0;
		virtual EResultT 	ISetLogSeverity(ESeverityT severity) = 0;
		virtual EResultT 	IProfilerSave(const char* file_name) = 0;
		virtual EResultT 	ITraceSave(const char* file_name) = 0;
		virtual EResultT 	ICall(uint32_t param) = 0;
		virtual EResultT 	ISetBP(const char *data, uint32_t *id) = 0;
		virtual EResultT 	IClearBP(uint32_t id) = 0;
		virtual EResultT 	ISendCLI(const char* command_str, char **respond) = 0;
		virtual CMemArea** 	IGetProfilingData(uint32_t *nuentries_)= 0;
		virtual CMemArea**	IGetLogData(uint32_t *nuentries_) = 0;
		virtual void 		Delay_us(uint32_t usecs) = 0;
		virtual void 		AddMemArea(CMemAreaP *mearea_p_) = 0;

	};


	/*******************************************************************************************//*!
	*@class IRSE_ControlAPI
	*@brief The purpose of this class is :
	*@brief Running and control 5G enodev using external program (front end)
	***********************************************************************************************/
	class IRSE_ControlAPI
	{
	public:
		virtual EResultT IInit(	IRSE_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info) = 0;
		virtual EResultT IColdStart() = 0;
		virtual EResultT IWarmStart() = 0;
		virtual EResultT IHotStart() = 0;
		virtual EResultT IStop(ESeverityT severity) = 0;
		virtual EResultT IConfigure(EConfigId id, void *in, void **out) = 0;
		virtual EResultT IGetInfo(const char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info) = 0;
		virtual EResultT IExitReq(ESeverityT severity) = 0;
	};

	//General factory API
	/*******************************************************************************************//*!
	*@class ITarget
	*@brief The purpose of this class is :
	*@brief Factory API using for connection to the classes implemented inside the library.
	***********************************************************************************************/
	class ITarget
	{
	public:
		virtual EResultT IGetTarget(EInterfaceT api_type, uint32_t id, void **gl_api_ptr) = 0;
		virtual EResultT IGetVersionInfo(char** module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char** add_info) = 0;
	};
}; //Namespace definition
//API function to connect with ITarget API
typedef uint32_t (*IGetConnectAPI_t)(void **target_ptr);
extern "C" uint32_t IGetConnectAPI(void **target_ptr);
#endif /* I_COMP_API_H_ */
