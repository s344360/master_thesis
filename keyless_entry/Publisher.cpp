

#include "Publisher.h"

#include <iostream>

using namespace std;

UA_Boolean Publisher::running = true;

Publisher::Publisher() {}
Publisher::Publisher(InputData *data) {
{
		this->data = data;
		this->configureServer(data->serverPort);
		this->initPublishedDataSet();
	}
}

Publisher::~Publisher() {
}


void Publisher::configureServer(UA_UInt16 serverPort) {
	server = UA_Server_new();
	UA_ServerConfig *config = UA_Server_getConfig(server);
	UA_ServerConfig_setMinimal(config, serverPort, NULL);

	 /* Details about the connection configuration and handling are located in
	  * the pubsub connection tutorial */
	config->pubsubTransportLayers =
		(UA_PubSubTransportLayer *) UA_calloc(2, sizeof(UA_PubSubTransportLayer));
	if(!config->pubsubTransportLayers) {
		UA_Server_delete(server);
		this->isServerConfigured = false;
	}
	    config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
	    config->pubsubTransportLayersSize++;

	#ifdef UA_ENABLE_PUBSUB_ETH_UADP
	    config->pubsubTransportLayers[1] = UA_PubSubTransportLayerEthernet();
	    config->pubsubTransportLayersSize++;
	#endif

	this->isServerConfigured = true;
}

UA_NodeId Publisher::addPubSubConnection(UA_Int32 pubId) {
	/* Details about the connection configuration and handling are located
	 * in the pubsub connection tutorial */
	UA_NodeId connectionIdent;
	UA_PubSubConnectionConfig connectionConfig;
	memset(&connectionConfig, 0, sizeof(connectionConfig));
	char name[] = {"UADP Connection 1"};
	connectionConfig.name = UA_STRING(name);
	connectionConfig.transportProfileUri = data->transportProfile;
	connectionConfig.enabled = UA_TRUE;
	UA_Variant_setScalar(&connectionConfig.address, &data->networkAddressUrl,
						 &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
	/* Changed to static publisherId from random generation to identify
	 * the publisher on Subscriber side */
	connectionConfig.publisherId.numeric = pubId;
	if (UA_Server_addPubSubConnection(server, &connectionConfig, &connectionIdent)
			== UA_STATUSCODE_GOOD) {
	    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"\ton " UA_PRINTF_STRING_FORMAT, UA_PRINTF_STRING_DATA(this->data->networkAddressUrl.url));
	} else {
	    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"ADD PUBSUB CHANNEL ON " UA_PRINTF_STRING_FORMAT " FAILED!", UA_PRINTF_STRING_DATA(this->data->networkAddressUrl.url));
	}
	return connectionIdent;
}

UA_NodeId Publisher::addWriterGroup(UA_Duration publishInterval, UA_NodeId &connectionIdent, UA_Int16 writerGroupId) {
	/* Now we create a new WriterGroupConfig and add the group to the existing
	 * PubSubConnection. */
	UA_NodeId writerGroupIdent;
	UA_WriterGroupConfig writerGroupConfig;
	memset(&writerGroupConfig, 0, sizeof(UA_WriterGroupConfig));
	char name[] = "Demo WriterGroup";
	writerGroupConfig.name = UA_STRING(name);
	writerGroupConfig.publishingInterval = publishInterval;
	writerGroupConfig.enabled = UA_FALSE;
	writerGroupConfig.writerGroupId = writerGroupId;
	writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
	writerGroupConfig.messageSettings.encoding             = UA_EXTENSIONOBJECT_DECODED;
	writerGroupConfig.messageSettings.content.decoded.type = &UA_TYPES[UA_TYPES_UADPWRITERGROUPMESSAGEDATATYPE];
	/* The configuration flags for the messages are encapsulated inside the
	 * message- and transport settings extension objects. These extension
	 * objects are defined by the standard. e.g.
	 * UadpWriterGroupMessageDataType */
	UA_UadpWriterGroupMessageDataType *writerGroupMessage  = UA_UadpWriterGroupMessageDataType_new();
	/* Change message settings of writerGroup to send PublisherId,
	 * WriterGroupId in GroupHeader and DataSetWriterId in PayloadHeader
	 * of NetworkMessage */
	writerGroupMessage->networkMessageContentMask          = (UA_UadpNetworkMessageContentMask)(UA_UADPNETWORKMESSAGECONTENTMASK_PUBLISHERID |
															  (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_GROUPHEADER |
															  (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_WRITERGROUPID |
															  (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_PAYLOADHEADER);
	writerGroupConfig.messageSettings.content.decoded.data = writerGroupMessage;
	UA_Server_addWriterGroup(server, connectionIdent, &writerGroupConfig, &writerGroupIdent);
	UA_Server_setWriterGroupOperational(server, writerGroupIdent);
	UA_UadpWriterGroupMessageDataType_delete(writerGroupMessage);
	return writerGroupIdent;
}

void Publisher::initPublishedDataSet() {
	/* The PublishedDataSetConfig contains all necessary public
	 * informations for the creation of a new PublishedDataSet */
	memset(&publishedDataSetConfig, 0, sizeof(UA_PublishedDataSetConfig));
	publishedDataSetConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
	char name[] = {"Demo PDS"};
	publishedDataSetConfig.name = UA_STRING(name);
}

UA_NodeId Publisher::addPublishedDataSet() {
	UA_NodeId publishedDataSetIdent;
	UA_Server_addPublishedDataSet(server, &publishedDataSetConfig, &publishedDataSetIdent);
	return publishedDataSetIdent;
}

void Publisher::addDataSetField(UA_NodeId &publishedDataSetIdent, const UA_NodeId &varNodeId) {
	/* Add a field to the previous created PublishedDataSet */
	    UA_NodeId dataSetFieldIdent;
	    UA_DataSetFieldConfig dataSetFieldConfig;
	    memset(&dataSetFieldConfig, 0, sizeof(UA_DataSetFieldConfig));
	    dataSetFieldConfig.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
	    char name[] = {"Server localtime"};
	    dataSetFieldConfig.field.variable.fieldNameAlias = UA_STRING(name);
	    dataSetFieldConfig.field.variable.promotedField = UA_FALSE;
	    dataSetFieldConfig.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
		dataSetFieldConfig.field.variable.publishParameters.publishedVariable =	varNodeId;
	    UA_Server_addDataSetField(server, publishedDataSetIdent,
	                              &dataSetFieldConfig, &dataSetFieldIdent);

}

void Publisher::addDataSetWriter(UA_NodeId publishedDataSetIdent, UA_NodeId &writerGroupIdent, UA_Int16 dataSetWriterId) {
	/* We need now a DataSetWriter within the WriterGroup. This means we must
	 * create a new DataSetWriterConfig and add call the addWriterGroup function. */
	    UA_NodeId dataSetWriterIdent;
	    UA_DataSetWriterConfig dataSetWriterConfig;
	    memset(&dataSetWriterConfig, 0, sizeof(UA_DataSetWriterConfig));
	    char name[] = {"Demo DataSetWriter"};
	    dataSetWriterConfig.name = UA_STRING(name);
	    dataSetWriterConfig.dataSetWriterId = dataSetWriterId;
	    dataSetWriterConfig.keyFrameCount = 10;
	    UA_Server_addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent,
	                               &dataSetWriterConfig, &dataSetWriterIdent);

}

void Publisher::addVariableNode(UA_NodeId variableNodeId, const UA_DataType *variableType) {
	UA_VariableAttributes publisherAttr = UA_VariableAttributes_default;
	char name[] = {"Publisher Counter"};
	char text[] = {"en-US"};
	publisherAttr.displayName = UA_LOCALIZEDTEXT(text, name);
	publisherAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;


	if (variableType == &UA_TYPES[UA_TYPES_BYTE]) {
		UA_Byte publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_BYTE]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_SBYTE]) {
		UA_SByte publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_SBYTE]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),


									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_INT16]) {
		UA_Int16 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_INT16]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_INT32]) {
		UA_Int32 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_INT32]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_INT64]) {
		UA_Int64 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_INT64]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_UINT16]) {
		UA_UInt16 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_UINT16]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_UINT32]) {
		UA_UInt32 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_UINT32]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_UINT64]) {
		UA_UInt64 publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_UINT64]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_FLOAT]) {
		UA_Float publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_FLOAT]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_DOUBLE]) {
		UA_Double publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_DOUBLE]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else if (variableType == &UA_TYPES[UA_TYPES_BOOLEAN]) {
		UA_Boolean publishValue = true;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_BOOLEAN]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}
	else {
		UA_Double publishValue = 0;
		UA_Variant_setScalar(&publisherAttr.value, &publishValue, &UA_TYPES[UA_TYPES_DOUBLE]);
		UA_Server_addVariableNode(server, variableNodeId,
									  UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
									  UA_QUALIFIEDNAME(1, name),
									  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
									  publisherAttr, NULL, NULL);
	}

}

void Publisher::addVariableToPublish(const UA_DataType *variableType,
		UA_NodeId &writerGroupIdent, const UA_NodeId &varNodeId) {

	this->addVariableNode(varNodeId, variableType);
	UA_NodeId publishedDataSetIdent_1 = this->addPublishedDataSet();
	this->addDataSetField(publishedDataSetIdent_1, varNodeId);
	this->addDataSetWriter(publishedDataSetIdent_1, writerGroupIdent /*, dataSetWriterId default 62541*/);
}


void Publisher::writeValueToVariable(const UA_DataType *variableType,
		void *value, const UA_NodeId &varNodeId) {

	UA_Variant myVar;
	UA_Variant_init(&myVar);
	UA_Variant_setScalar(&myVar, value, variableType);
	UA_Server_writeValue(server, varNodeId, myVar);
}


void Publisher::writeValueDouble(UA_Double val, const UA_NodeId &varNodeId) {
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_DOUBLE], &val, varNodeId);
}

void Publisher::setChannelPort(UA_UInt16 port) {
	char p[4];
	sprintf(p, "%d", port);
	UA_String s = this->data->networkAddressUrl.url;
	UA_Byte l = s.length;
	UA_Byte k = 5;
	for (int i = 2; i < 6; i++) {
		s.data[l-i] = p[k-i];
	}
}

void Publisher::setUrl(char *url) {
	this->data->networkAddressUrl.url = UA_STRING(url);
	free(url);
}


void Publisher::startDemo() {
    // Adds PubSubChannel on default port "opc.udp://224.0.0.22:4840/":
	UA_NodeId connectionIdent_1 = this->addPubSubConnection(/*publisherId default 2234*/);

    UA_NodeId writerGroup_0 = this->addWriterGroup(/*interval:*/1000, connectionIdent_1 /*, writerGroupId default 100*/);

    UA_NodeId publishedDataSetIdent = this->addPublishedDataSet();
	this->addDataSetField(publishedDataSetIdent, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME));
	this->addDataSetWriter(publishedDataSetIdent, writerGroup_0 /*, dataSetWriterId default 62541*/);


	UA_NodeId writerGroup_1 = this->addWriterGroup(/*interval:*/5000, connectionIdent_1 /*, writerGroupId default 100*/);

	this->addVariableToPublish(&UA_TYPES[UA_TYPES_BYTE], writerGroup_1, UA_NODEID_NUMERIC(1, 1111));
	this->addVariableToPublish(&UA_TYPES[UA_TYPES_FLOAT], writerGroup_1, UA_NODEID_NUMERIC(1, 2222));
	this->addVariableToPublish(&UA_TYPES[UA_TYPES_BOOLEAN], writerGroup_1, UA_NODEID_NUMERIC(1, 1234));

	UA_Byte byteVal = 127;
	UA_SByte sbyteVal = -128;
	UA_Boolean boolVal = true;
	UA_Float floatVal = 127.1234;
	UA_Double doubleVal = 0.987989;
	UA_Int16 int16Val = 3651;
	UA_Int32 int32Val = INT32_MIN;
	UA_Int64 int64Val = INT64_MAX;
	UA_UInt16 uInt16Val = 9864;
	UA_UInt32 uInt32Val = 1519849;
	UA_UInt64 uInt64Val = 546546;

	this->writeValueToVariable(&UA_TYPES[UA_TYPES_BYTE], &byteVal, UA_NODEID_NUMERIC(1, 1111));
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_FLOAT], &floatVal, UA_NODEID_NUMERIC(1, 2222));
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_BOOLEAN], &boolVal, UA_NODEID_NUMERIC(1, 1234));


	// Add another PubSubChannel on different port
    this->setChannelPort(4860);
    UA_NodeId connectionIdent_2 = this->addPubSubConnection(/*publisherId default 2234*/);

	UA_NodeId writerGroup_2 = this->addWriterGroup(/*interval:*/2000, connectionIdent_2 /*, writerGroupId default 100*/);

	this->addVariableToPublish(&UA_TYPES[UA_TYPES_SBYTE], writerGroup_2, UA_NODEID_NUMERIC(0, 1598));
	this->addVariableToPublish(&UA_TYPES[UA_TYPES_UINT16], writerGroup_2, UA_NODEID_NUMERIC(1, 3333));
	this->addVariableToPublish(&UA_TYPES[UA_TYPES_UINT32], writerGroup_2, UA_NODEID_NUMERIC(1, 4444));
	this->addVariableToPublish(&UA_TYPES[UA_TYPES_UINT64], writerGroup_2, UA_NODEID_NUMERIC(1, 5555));

	this->writeValueToVariable(&UA_TYPES[UA_TYPES_SBYTE], &sbyteVal, UA_NODEID_NUMERIC(0, 1598));
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_UINT16], &uInt16Val, UA_NODEID_NUMERIC(1, 3333));
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_UINT32], &uInt32Val, UA_NODEID_NUMERIC(1, 4444));
	this->writeValueToVariable(&UA_TYPES[UA_TYPES_UINT64], &uInt64Val, UA_NODEID_NUMERIC(1, 5555));

	// Add another PubSubChannel
    this->setChannelPort(4870);
    UA_NodeId connectionIdent_3 = this->addPubSubConnection(/*publisherId default 2234*/);

    UA_NodeId writerGroup_3 = this->addWriterGroup(/*interval:*/1000, connectionIdent_3 /*, writerGroupId default 100*/);

    this->addVariableToPublish(&UA_TYPES[UA_TYPES_INT16], writerGroup_3, UA_NODEID_NUMERIC(1, 6666));
    this->addVariableToPublish(&UA_TYPES[UA_TYPES_INT32], writerGroup_3, UA_NODEID_NUMERIC(1, 7777));
    this->addVariableToPublish(&UA_TYPES[UA_TYPES_INT64], writerGroup_3, UA_NODEID_NUMERIC(1, 8888));
    this->addVariableToPublish(&UA_TYPES[UA_TYPES_DOUBLE], writerGroup_3, UA_NODEID_NUMERIC(1, 9999));

    this->writeValueToVariable(&UA_TYPES[UA_TYPES_INT16], &int16Val, UA_NODEID_NUMERIC(1, 6666));
    this->writeValueToVariable(&UA_TYPES[UA_TYPES_INT32], &int32Val, UA_NODEID_NUMERIC(1, 7777));
    this->writeValueToVariable(&UA_TYPES[UA_TYPES_INT64], &int64Val, UA_NODEID_NUMERIC(1, 8888));
    this->writeValueToVariable(&UA_TYPES[UA_TYPES_DOUBLE], &doubleVal, UA_NODEID_NUMERIC(1, 9999));
}

void Publisher::disablePublishing(UA_NodeId *writerGroupIdent) {
	UA_Server_setWriterGroupDisabled(server, *writerGroupIdent);
}

void Publisher::enablePublishing(UA_NodeId *writerGroupIdent) {
	UA_Server_setWriterGroupOperational(server, *writerGroupIdent);
}


void Publisher::stopHandler(int sign) {
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
	Publisher::running = false;
}

bool Publisher::isRunning() {
	return Publisher::running;
}

void Publisher::stopRunning() {
	Publisher::running = false;
}

int Publisher::run() {
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);

	if(!isServerConfigured)
		return EXIT_FAILURE;

	printf("mach was");
//	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"URL: " UA_PRINTF_STRING_FORMAT"\n", UA_PRINTF_STRING_DATA(networkAddressUrl.url));

	UA_StatusCode retval = UA_Server_run(server, &Publisher::running);

	UA_Server_delete(server);
	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}


void Publisher::publishDateTime(UA_UInt16 interval, UA_NodeId *writerGroup, UA_NodeId &connId) {

	*writerGroup = this->addWriterGroup(/*interval:*/1000, connId /*, writerGroupId default 100*/);

	UA_NodeId publishedDataSetIdent = this->addPublishedDataSet();
	this->addDataSetField(publishedDataSetIdent, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME));
	this->addDataSetWriter(publishedDataSetIdent, *writerGroup /*, dataSetWriterId default 62541*/);
}
