#include "BufferingAgent.h"


namespace pelican {
namespace ampp {

BufferingAgent::BufferingAgent(AbstractDataClient& client)
    : QThread()
    , _max_queue_length(3)
    , _halt(false)
    , _client(client)
{
    // create some objects to fill
    for(int i=0; i < _max_queue_length; ++i) {
        _buffer_objects.push_back(DataBlobHash);
    }
    // assign then to the buffer locking manager
    _buffer.reset(&_buffer_objects);
}

BufferingAgent::~BufferingAgent()
{
    _halt = false;
}

void BufferingAgent::run() {
    _halt = false;
    while(1) {
        if(_halt) return;
        DataBlobHash& hash = *(_buffer.next()); // blocks until ready
        if(_halt) return;
        _client.getData(hash);
        _queue.push_back(hash);
    }
}

void BufferingAgent::getData(DataBlobHash& hash) {
    // spin until we have data
    do{}
    while(_queue.empty());

    hash.swap(_queue.front()); // TODO verify this is doing what we think its doing
    _buffer.unlock(&(_queue.front()));
    _queue.pop_front();
}

} // namespace ampp
} // namespace pelican
