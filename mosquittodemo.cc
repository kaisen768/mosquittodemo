#include "mosquittodemo.h"

#if !USE_DEFAULT_CLIENTID
    #include <ctime>
#endif

#define BROKER_HOST "59.61.82.170"
#define BROKER_PORT 5660
#define BROKER_CONNECT_USERNAME "admin"
#define BROKER_CONNECT_PASSWORD "123456"

MosquittoDemo::MosquittoDemo()
    : mosq_(nullptr)
    , state_(MOSQUITTODEMO_STATE_IDLE)
    , host_(BROKER_HOST)
    , port_(BROKER_PORT)
    , username_(BROKER_CONNECT_USERNAME)
    , password_(BROKER_CONNECT_PASSWORD)

{
}

MosquittoDemo::~MosquittoDemo()
{
}

static void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    int rc;
    MosquittoDemo *moquittodemo = (MosquittoDemo *)obj;

    if (reason_code != 0) {
        fprintf(stderr, "mosquitto connect error: %s\n", mosquitto_connack_string(reason_code));
        return;
    }

    /* Making subscriptions in the on_connect() */
    // for (size_t id = 0; id < TOPIC_ID_SIZE, id++) {
    //     const char *topic = ...;

    //     rc = mosquitto_subscribe(mosq, nullptr, topic, 1);
    //     if (rc != MOSQ_ERR_SUCCESS) {
    //         fprintf(stderr, "topic \"%s\" subscribe failed\n", topic);
    //     } else {
    //         fprintf(stderr, "topic \"%s\" subscribe success\n", topic);
    //     }
    // }
}

static void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code)
{
    MosquittoDemo *moquittodemo = (MosquittoDemo *)obj;

    fprintf(stderr, "mosquitto disconnect: %s\n", mosquitto_connack_string(reason_code));

    // for (size_t id = 0; id < TOPIC_ID_SIZE, id++) {
    //     const char *topic = ...;

    //     mosquitto_unsubscribe(mosq, nullptr, topic);
    // }
}

static void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
}

static void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    int i;
    bool have_subscription = false;

    for(i = 0; i < qos_count; i++) {
        // printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
        if (granted_qos[i] <= 4) {
            have_subscription = true;
        }
    }

    if(have_subscription == false){
        fprintf(stderr, "Error: All subscriptions rejected.\n");
        // mosquitto_disconnect(mosq);
    }
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("message received: topic:%s qos:%d payload:%s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int MosquittoDemo::Start()
{
    int rc;

    if (state_ != MOSQUITTODEMO_STATE_IDLE) {
        return -1;
    }

    state_ = MOSQUITTODEMO_STATE_STARTING;

    /* Required before calling other mosquitto functions */
    rc = mosquitto_lib_init();
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "mosquitto lib init failed!\n");
        return -1;
    }

    #if USE_DEFAULT_CLIENTID
        const char *id = nullptr;
    #else
        std::string client_id = GenerateClientId();
        const char *id = client_id.c_str();
    #endif

    /* Create a new client instance */
    mosq_ = mosquitto_new(id, true, this);
    if (mosq_ == nullptr) {
        fprintf(stderr, "mosquitto new session failed.\n");
        goto error0;
    }

    /* Set connet's username and password */
    rc = mosquitto_username_pw_set(mosq_, username_.c_str(), password_.c_str());
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "mosquitto username and password setting is failed, error: %s\n", mosquitto_strerror(rc));
        goto error1;
    }

    /* Configure callbacks. This should be done before connecting ideally. */
    mosquitto_connect_callback_set(mosq_, on_connect);
    mosquitto_disconnect_callback_set(mosq_, on_disconnect);
    mosquitto_publish_callback_set(mosq_, on_publish);
    mosquitto_subscribe_callback_set(mosq_, on_subscribe);
    mosquitto_message_callback_set(mosq_, on_message);

    /* Connect to broker */
    rc = mosquitto_connect(mosq_, host_.c_str(), port_, 30);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "connect to mqtt broker error: %s\n", mosquitto_strerror(rc));
        goto error1;
    }

    rc = mosquitto_loop_start(mosq_);
	if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "mosquitto loop start failed, error: %s", mosquitto_strerror(rc));
		goto error1;
	}

    state_ = MOSQUITTODEMO_STATE_CONNECTED;

    return 0;

error1:
    mosquitto_destroy(mosq_);
error0:
    mosquitto_lib_cleanup();
    state_ = MOSQUITTODEMO_STATE_IDLE;
    return -1;
}

void MosquittoDemo::Stop()
{
    if (state_ != MOSQUITTODEMO_STATE_CONNECTED)
        return;

    state_ = MOSQUITTODEMO_STATE_CLOSING;

    mosquitto_disconnect(mosq_);

    if (mosq_ != nullptr) {
        mosquitto_loop_stop(mosq_, false);
        mosquitto_destroy(mosq_);
    }

    mosquitto_lib_cleanup();

    state_ = MOSQUITTODEMO_STATE_IDLE;
}

MosquittoDemoState MosquittoDemo::GetState()
{
    return state_;
}

bool MosquittoDemo::PublishData(const std::string &_topic, const std::string &data)
{
    int rc;
    int mid;
    const char *topic;
    int payloadlen;
    const char *payload;

    mid = 0;
    topic = _topic.c_str();
    payloadlen = data.length();
    payload = data.c_str();

    rc = mosquitto_publish(mosq_, &mid, topic, payloadlen, payload, 2, false);
    if(rc != MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
        return false;
    } else {
        fprintf(stderr, "message send: %s\n", topic);
    }

    return true;
}
