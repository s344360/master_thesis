
#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pubsub_ethernet.h>
#include <open62541/plugin/pubsub_udp.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <signal.h>
#include "common_structures.h"

#define WRITERGROUP_NUMBER 7

#define VAR_TYPE_DOUBLE			&UA_TYPES[UA_TYPES_DOUBLE]
#define VAR_TYPE_FLOAT			&UA_TYPES[UA_TYPES_FLOAT]
#define VAR_TYPE_BOOLEAN		&UA_TYPES[UA_TYPES_BOOLEAN]
#define VAR_TYPE_BYTE			&UA_TYPES[UA_TYPES_BYTE]
#define VAR_TYPE_INT32			&UA_TYPES[UA_TYPES_INT32]
#define VAR_TYPE_UINT32			&UA_TYPES[UA_TYPES_UINT32]

#define ID_DOUBLE				UA_NODEID_NUMERIC(1, 1111)
#define ID_FLOAT				UA_NODEID_NUMERIC(1, 2111)
#define ID_BOOLEAN				UA_NODEID_NUMERIC(1, 3111)
#define ID_BYTE					UA_NODEID_NUMERIC(1, 4111)
#define ID_INT32				UA_NODEID_NUMERIC(1, 5111)
#define ID_UINT32				UA_NODEID_NUMERIC(1, 6111)


class Publisher {
private:
	InputData *data;

//	UA_String *transportProfile;
//	UA_NetworkAddressUrlDataType *networkAddressUrl;

	UA_PublishedDataSetConfig publishedDataSetConfig;

	UA_Boolean isServerConfigured;
	UA_Duration interval;

	static UA_Boolean running;

public:
	UA_Server *server;

    Publisher();

	Publisher(InputData *data);

	virtual ~Publisher();

	void configureServer(UA_UInt16 serverPort = 4840);

	UA_NodeId addPubSubConnection(UA_Int32 pubId = 2234);

	UA_NodeId addWriterGroup(UA_Duration publishInterval, UA_NodeId &connectionIdent, UA_Int16 writerGroupId = 100);

	void addDataSetWriter(UA_NodeId publishedDataSetIdent, UA_NodeId &writerGroupIdent, UA_Int16  dataSetWriterId = 62541);

	void initPublishedDataSet();

	UA_NodeId addPublishedDataSet();

	void addDataSetField(UA_NodeId &publishedDataSetIdent, const UA_NodeId &varNodeId);

	void addVariableNode(UA_NodeId variableNodeId, const UA_DataType *variableType);

	void addVariableToPublish(const UA_DataType *variableType, UA_NodeId &writerGroupIdent, const UA_NodeId &varNodeId);

	void writeValueToVariable(const UA_DataType *variableType,
				void *value, const UA_NodeId &varNodeId);

	void writeValueDouble(UA_Double val, const UA_NodeId &varNodeId);

	void startDemo();

	void disablePublishing(UA_NodeId *writerGroupIdent);

	void enablePublishing(UA_NodeId *writerGroupIdent);

	void setChannelPort(UA_UInt16 port);

	void setUrl(char *url);

	int run();

	static bool isRunning();

	static void stopRunning();

	static void stopHandler(int sign);

	void publishDateTime(UA_UInt16 interval, UA_NodeId *writerGroup, UA_NodeId &connId);
};

#endif /* PUBLISHER_H_ */
