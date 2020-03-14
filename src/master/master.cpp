#include"master.h"

#include<glog/logging.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace ::apache::thrift::concurrency;

Master::Master(int port, uint32_t num_workers, uint32_t num_servers)
   : port_(port), num_workers_(num_workers), num_servers_(num_servers){}

//TODO:提供异常处理机制
void Master::start_serve(){
  shared_ptr<MLtaskHandler> handler(make_shared<MLtaskHandler>(num_workers_, num_servers_));
  shared_ptr<TProcessor> processor(make_shared<MLtaskProcessor>(handler));
  shared_ptr<TServerTransport> server_transport(make_shared<TServerSocket>(port_));
  shared_ptr<TTransportFactory> transport_factory(make_shared<TBufferedTransportFactory>());
  shared_ptr<TProtocolFactory> protocol_factory(make_shared<TBinaryProtocolFactory>());
  //线程池
  //TODO
  //不要写magic number统一放在一个头文件
  const int num_threads = 4;
  shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(num_threads);
  shared_ptr<ThreadFactory> threadFactory = shared_ptr<ThreadFactory>(new ThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();
  task_server_ = make_shared<TThreadPoolServer>(processor, server_transport, transport_factory, protocol_factory, threadManager);
  LOG(INFO) << "Master start serve at port: " << port_;
  task_server_->serve();
}
