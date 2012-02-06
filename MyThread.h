#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SLASH '/'
#define BAD_SOCKET -1

class MyThread
{
  private:
    pthread_t the_thread;
  public:
  // the default constructor, does nothing
  MyThread();
  // the destructor, does nothing
  virtual ~MyThread();
  // call this function to create a thread.
  virtual void createThread(void* threadProc, void* param);
  // call this function to exit a thread.
  virtual void exitThread();
  virtual void cancelThread();
};
