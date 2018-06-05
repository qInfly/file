#include <iomanip>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <errno.h>
#include <netinet/in.h>   //互联网地址族
#include <sys/types.h>    //是Unix/Linux系统的基本系统数据类型的头文件
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "MQTTClient.h"

#define ADDRESS     "qinzining.quarkioe.cn"
#define CLIENTID    "123123123123"
#define MAXBUF 1024
 
void publish(MQTTClient client, char* topic, char* payload) 
	{
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	pubmsg.payload = payload;
	pubmsg.payloadlen = strlen((char*)pubmsg.payload);
	pubmsg.qos = 2;
	pubmsg.retained = 0;
	MQTTClient_deliveryToken token;
	MQTTClient_publishMessage(client, topic, &pubmsg, &token);
	MQTTClient_waitForCompletion(client, token, 1000L);
	printf("Message '%s' with delivery token %d delivered\n", payload, token);
	}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) 
	{
    void* payload = message->payload;
    printf("Received operation %s\n", payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
	}

int main(int argc, char* argv[]) 
	{
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = "qinzining/qinzining";
    conn_opts.password = "hello@123";

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) 
	{
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    	}
    	//create device
    	publish(client, "s/us", "100,MQTT,c8y_MQTTDevice");
    	//set hardware information
    	publish(client, "s/us", "110,123456789,,Rev0.1");
	publish(client, "s/us", "111,1234567890,100,54353");
	publish(client, "s/us", "112,50.323423,6.423423");
    //listen for operation
    MQTTClient_subscribe(client, "s/ds", 0);

    for (;;) 
	{
        //send temperature measurement
        publish(client, "s/us", "211,25");
        sleep(3);
    	}
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}

