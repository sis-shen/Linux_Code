#include "Threadpool.hpp"
#include "Task.hpp"

using namespace std;


int main()
{
    srand(time(0));

    ThreadPool<Task> *tp = new ThreadPool<Task>(5);
    tp->Start();
    while(true)
    {
        //1.构建任务
        sleep(1);
        Task t;
        //2.交给线程池处理
        tp->Push(t);
        string tstr = t.PrintTask();
        // cout<<"main thread make a task.."<<tstr<<endl;
        t.PrintTask();
    }
    return 0;
}