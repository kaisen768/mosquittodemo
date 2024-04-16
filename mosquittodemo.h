#ifndef _MOSQUITTODEMO_H_
#define _MOSQUITTODEMO_H_

#include <string>
#include "mosquitto.h"

#define USE_DEFAULT_CLIENTID 1

enum MosquittoDemoState {
    MOSQUITTODEMO_STATE_IDLE = 0,
    MOSQUITTODEMO_STATE_STARTING,
    MOSQUITTODEMO_STATE_CONNECTED,
    MOSQUITTODEMO_STATE_CLOSING
};

class MosquittoDemo
{
public:
    MosquittoDemo();
    ~MosquittoDemo();

    int Start();
    void Stop();

    MosquittoDemoState GetState();

private:
    bool PublishData(const std::string &topic, const std::string &data);

private:
    struct mosquitto *mosq_;
    MosquittoDemoState state_;
    std::string host_;
    int port_;
    std::string username_;
    std::string password_;
};

#endif // _MOSQUITTODEMO_H_
