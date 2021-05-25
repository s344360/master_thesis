

#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pubsub.h>
#include <open62541/plugin/pubsub_udp.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include "Publisher.h"
#include "common_structures.h"
#include <thread>
#include "InputArguments.h"
#include "UserInput.h"
#include <iostream>
#include "ua_pubsub_networkmessage.h"
#include <signal.h>


using namespace std;


Publisher pub;
UA_NodeId wg[WRITERGROUP_NUMBER];
UA_WriterGroupConfig wgConfig[WRITERGROUP_NUMBER];

static void runThis() {

    InputData data;
    char tp[] = {"http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp"};
    data.transportProfile = UA_STRING(tp);
    data.serverPort = 4892;
    char url2[] = {"opc.udp://224.0.0.22:4842/"};//send 1erst msg to BCM
    data.networkAddressUrl = {UA_STRING_NULL, UA_STRING(url2)};


    pub = Publisher(&data);


    UA_NodeId connId = pub.addPubSubConnection();


    wg[0] = pub.addWriterGroup(1000, connId);
    memset(&wgConfig[0], 0, sizeof(UA_WriterGroupConfig));
    UA_Server_getWriterGroupConfig(pub.server, wg[0], &wgConfig[0]);
    UA_Server_setWriterGroupDisabled(pub.server, wg[0]);
    pub.addVariableToPublish(VAR_TYPE_BYTE, wg[0], ID_BYTE);

    //Startwert
    UA_Double val= 123.344;
    pub.writeValueToVariable(VAR_TYPE_DOUBLE, &val, ID_DOUBLE);

    //Create new channel
    pub.setChannelPort(4842);//send 2nd msg to BCM
    UA_NodeId connectionIdent_2 = pub.addPubSubConnection(/*Default publihserID 2234 */);

    wg[1] = pub.addWriterGroup(1000, connectionIdent_2);
    memset(&wgConfig[1], 0, sizeof(UA_WriterGroupConfig));
    UA_Server_getWriterGroupConfig(pub.server, wg[1], &wgConfig[1]);
    UA_Server_setWriterGroupDisabled(pub.server, wg[1]);
    pub.addVariableToPublish(&UA_TYPES[UA_TYPES_UINT64], wg[1], UA_NODEID_NUMERIC(1, 5555));


    thread t(UserInput::handlingWriterGroup, &pub, wg, wgConfig);//userinput
    t.detach();

    pub.run();

}


UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "received ctrl-c");
    running = false;
}

static UA_StatusCode
subscriberListen(UA_PubSubChannel *psc) {
    UA_ByteString buffer;
    UA_StatusCode retval = UA_ByteString_allocBuffer(&buffer, 512);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Message buffer allocation failed!");
        return retval;
    }

    /* Receive the message. Blocks for 100ms */
    retval = psc->receive(psc, &buffer, NULL, 100);
    if(retval != UA_STATUSCODE_GOOD || buffer.length == 0) {
        /* Workaround!! Reset buffer length. Receive can set the length to zero.
         * Then the buffer is not deleted because no memory allocation is
         * assumed.
         * TODO: Return an error code in 'receive' instead of setting the buf
         * length to zero. */
        buffer.length = 512;
        UA_ByteString_clear(&buffer);
        return UA_STATUSCODE_GOOD;
    }

    /* Decode the message */
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Message length: %lu", (unsigned long) buffer.length);
    UA_NetworkMessage networkMessage;
    memset(&networkMessage, 0, sizeof(UA_NetworkMessage));
    size_t currentPosition = 0;
    UA_NetworkMessage_decodeBinary(&buffer, &currentPosition, &networkMessage);
    UA_ByteString_clear(&buffer);

    /* Is this the correct message type? */
    if(networkMessage.networkMessageType != UA_NETWORKMESSAGE_DATASET)
        goto cleanup;

    /* At least one DataSetMessage in the NetworkMessage? */
    if(networkMessage.payloadHeaderEnabled &&
       networkMessage.payloadHeader.dataSetPayloadHeader.count < 1)
        goto cleanup;

    /* Is this a KeyFrame-DataSetMessage? */
    for(size_t j = 0; j < networkMessage.payloadHeader.dataSetPayloadHeader.count; j++) {
        UA_DataSetMessage *dsm = &networkMessage.payload.dataSetPayload.dataSetMessages[j];
        if(dsm->header.dataSetMessageType != UA_DATASETMESSAGE_DATAKEYFRAME)
            continue;

        /* Loop over the fields and print well-known content types */
        for(int i = 0; i < dsm->data.keyFrameData.fieldCount; i++) {
            const UA_DataType *currentType = dsm->data.keyFrameData.dataSetFields[i].value.type;
            if(currentType == &UA_TYPES[UA_TYPES_BYTE]) {
                UA_Byte value = *(UA_Byte *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Byte] \tReceived data: %d", value);


            } else if (currentType == &UA_TYPES[UA_TYPES_SBYTE]) {
                UA_SByte value = *(UA_SByte *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [SByte] \tReceived data: %d", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_INT16]) {
                UA_Int16 value = *(UA_Int16 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Int16] \tReceived data: %d", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_INT32]) {
                UA_Int32 value = *(UA_Int32 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Int32] \tReceived data: %d", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_INT64]) {
                UA_Int64 value = *(UA_Int64 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Int64] \tReceived data: %ld", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_UINT16]) {
                UA_UInt16 value = *(UA_UInt16 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [UInt16] \tReceived data: %u", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_UINT32]) {
                UA_UInt32 value = *(UA_UInt32 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [UInt32] \tReceived data: %u", value);

                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"Send created Challenge and Signature");

            } else if (currentType == &UA_TYPES[UA_TYPES_UINT64]) {
                UA_UInt64 value = *(UA_UInt64 *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [UInt64] \tReceived data: %lu", value);

                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"Send LF Data");

                static UA_UInt64 uInt64Val = 0;
                uInt64Val += 1000;
                pub.writeValueToVariable(&UA_TYPES[UA_TYPES_UINT64], &uInt64Val, UA_NODEID_NUMERIC(1, 5555));

                UA_Server_setWriterGroupOperational(pub.server, wg[1]);// Do Publish

                //sleep(1);
                UA_Server_setWriterGroupDisabled(pub.server, wg[1]);//Stop Publish
                printf("UID_KEY UINT64");




            } else if (currentType == &UA_TYPES[UA_TYPES_FLOAT]) {
                UA_Float value = *(UA_Float *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Float] \tReceived data: %f", value);

            } else if (currentType == &UA_TYPES[UA_TYPES_DOUBLE]) {
                UA_Double value = *(UA_Double *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Double] \tReceived data: %f", value);
            } else if (currentType == &UA_TYPES[UA_TYPES_BOOLEAN]) {
                UA_Boolean value = *(UA_Boolean *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [Boolean] \tReceived data: %s", value ? "true" : "false");

                if(value){
                    static UA_Double doubleValue= 0;
                    doubleValue += 100;
                    static UA_UInt64 uInt64Val = 0;
                    uInt64Val += 1000;
                    // Change Value for Publish
                    pub.writeValueToVariable(VAR_TYPE_BYTE, &doubleValue, ID_BYTE);
                    //pub.writeValueToVariable(&UA_TYPES[UA_TYPES_UINT64], &uInt64Val, UA_NODEID_NUMERIC(1, 5555));

                    UA_Server_setWriterGroupOperational(pub.server, wg[0]);// Do Publish
                    //UA_Server_setWriterGroupOperational(pub.server, wg[1]);// Do Publish
                    //sleep(1);
                    UA_Server_setWriterGroupDisabled(pub.server, wg[0]);//Stop Publish
//                    sleep(1);
//                    UA_Server_setWriterGroupDisabled(pub.server, wg[1]);//Stop Publish


                }
            } else if (currentType == &UA_TYPES[UA_TYPES_DATETIME]) {
                UA_DateTime value = *(UA_DateTime *)dsm->data.keyFrameData.dataSetFields[i].value.data;
                UA_DateTimeStruct receivedTime = UA_DateTime_toStruct(value);
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Message content: [DateTime] \t"
                            "Received data: %02i-%02i-%02i %02i:%02i:%02i",
                            receivedTime.year, receivedTime.month, receivedTime.day,
                            receivedTime.hour, receivedTime.min, receivedTime.sec);
            }
        }
    }

    cleanup:
    UA_NetworkMessage_clear(&networkMessage);
    return retval;
}


int main(int argc, char **argv) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    // Publsher Thread
    thread t(runThis);
    t.detach();

    sleep(2);

    UA_PubSubTransportLayer udpLayer = UA_PubSubTransportLayerUDPMP();

    UA_PubSubConnectionConfig connectionConfig;
    memset(&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING("UADP Connection 1");
    connectionConfig.transportProfileUri =
            UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
    connectionConfig.enabled = UA_TRUE;

    char url[]="opc.udp://224.0.0.22:4843/";
    UA_NetworkAddressUrlDataType networkAddressUrl =
            {UA_STRING_NULL , UA_STRING(url)};

    if (argc > 1) {
        if (strncmp(argv[1], "opc.udp://", 10) == 0) {
            networkAddressUrl.url = UA_STRING(argv[1]);
        }
        else if (strcmp(argv[1], "-cp") == 0) {
            UA_Int16 port = atoi(argv[2]);
            //		cout << "channel port from input: " << argument[index] << endl;
            if(port > 9999 || port < 1000) {
                printf("\n\tWarning: Invalid channel port input! Default port set!\n");
            }
            else {
                UA_Byte l = networkAddressUrl.url.length;
                UA_Byte i = 1;
                UA_Byte k = 4;
                if(networkAddressUrl.url.data[l-1] == '/' ) {
                    i++;
                    k++;
                }
                while ( i < k + 1) {
                    networkAddressUrl.url.data[l-i] = argv[2][k-i];
                    i++;
                }
            }
        }
    }
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"URL: " UA_PRINTF_STRING_FORMAT, UA_PRINTF_STRING_DATA(networkAddressUrl.url));
    UA_Variant_setScalar(&connectionConfig.address, &networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);


    UA_PubSubChannel *psc =
            udpLayer.createPubSubChannel(&connectionConfig);
    psc->regist(psc, NULL, NULL);

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    while(running && retval == UA_STATUSCODE_GOOD)
        retval = subscriberListen(psc);

    psc->close(psc);


    InputArguments::cleanup();

    return 0;
}



































