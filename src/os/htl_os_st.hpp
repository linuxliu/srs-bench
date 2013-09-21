#ifndef _htl_os_st_hpp
#define _htl_os_st_hpp

/*
#include <htl_os_st.hpp>
*/

#include <map>
#include <string>

#include <st.h>

#include <htl_core_log.hpp>

// the statistic for each st-thread(task)
class StStatistic
{
private:
    int threads, alive;
    int64_t starttime;
    ssize_t nread, nwrite;
    ssize_t tasks, err_tasks, sub_tasks, err_sub_tasks;
public:
    StStatistic();
    virtual ~StStatistic();
public:
    virtual void OnRead(int tid, ssize_t nread);
    virtual void OnWrite(int tid, ssize_t nwrite);
public:
    // when task thread run.
    virtual void OnThreadRun(int tid);
    // when task thread quit.
    virtual void OnThreadQuit(int tid);
    // when task start request url, ie. get m3u8
    virtual void OnTaskStart(int tid, std::string task_url);
    // when task error.
    virtual void OnTaskError(int tid);
    // when task finish request url, ie. finish all ts in m3u8
    virtual void OnTaskEnd(int tid);
    // when sub task start, ie. get ts in m3u8
    virtual void OnSubTaskStart(int tid, std::string sub_task_url);
    // when sub task error.
    virtual void OnSubTaskError(int tid);
    // when sub task finish, ie. finish a ts.
    virtual void OnSubTaskEnd(int tid);
public:
    virtual void DoReport(double sleep_ms);
};

extern StStatistic* statistic;

// abstract task for st, which run in a st-thread.
class StTask
{
private:
    int id;
public:
    StTask();
    virtual ~StTask();
public:
    virtual int GetId();
public:
    /**
    * the framework will start a thread for the task, 
    * then invoke the Process function to do actual transaction.
    */
    virtual int Process() = 0;
};

// the farm for all StTask, to spawn st-thread and process all task.
class StFarm
{
private:
    double report_seconds;
public:
    StFarm();
    virtual ~StFarm();
public:
    virtual int Initialize(double report);
    virtual int Spawn(StTask* task);
    virtual int WaitAll();
private:
    static void* st_thread_function(void* args);
};

// the socket status
enum SocketStatus{
    SocketInit,
    SocketConnected,
    SocketDisconnected,
};

// the socket base on st.
class StSocket
{
private:
    SocketStatus status;
    st_netfd_t sock_nfd;
public:
    StSocket();
    virtual ~StSocket();
public:
    virtual SocketStatus Status();
    virtual int Connect(const char* ip, int port);
    virtual int Read(const void* buf, size_t size, ssize_t* nread);
    virtual int Write(const void* buf, size_t size, ssize_t* nwrite);
    virtual int Close();
};

// common utilities.
class StUtility
{
public:
    static int64_t GetCurrentTime();
    static void InitRandom();
    static st_utime_t BuildRandomMTime(double sleep_seconds, double default_seconds = 0.0);
    static int DnsResolve(std::string host, std::string& ip);
};

// st-thread based log context.
class StLogContext : public LogContext
{
private:
    std::map<st_thread_t, int> cache;
public:
    StLogContext();
    virtual ~StLogContext();
public:
    virtual void SetId(int id);
    virtual int GetId();
};

#endif