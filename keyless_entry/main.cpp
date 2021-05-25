#include <iostream>
#include <thread>
#include <chrono>

#include "InputArguments.h"
#include "Publisher.h"
#include "UserInput.h"

using namespace std;

int main(int argc, char **argv) {

	InputData data;
	char tp[] = {"http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp"};
	data.transportProfile = UA_STRING(tp);
	data.serverPort = 4840;
	char url[] = {"opc.udp://224.0.0.22:4840/"};
    data.networkAddressUrl = {UA_STRING_NULL, UA_STRING(url)};

	InputArguments::interpretArguments(argc, argv, data);

//    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"URL: " UA_PRINTF_STRING_FORMAT"\n", UA_PRINTF_STRING_DATA(data.networkAddressUrl.url));

    cout << "\nParameters set:" << endl;
    cout << "\tserver port:\t" << data.serverPort << endl;
    cout << "\taddress url:\t" << data.networkAddressUrl.url.data << "\n\n";
    cout << "\ttransport profile:\t" << data.transportProfile.data << "\n\n";


    Publisher pub(&data);

	UA_NodeId connId = pub.addPubSubConnection();

	UA_NodeId wg[WRITERGROUP_NUMBER];

	UA_WriterGroupConfig wgConfig[WRITERGROUP_NUMBER];

	for(UA_Byte i = 0; i < WRITERGROUP_NUMBER; i++) {
		wg[i] = pub.addWriterGroup(1000, connId);
		memset(&wgConfig[i], 0, sizeof(UA_WriterGroupConfig));
		UA_Server_getWriterGroupConfig(pub.server, wg[i], &wgConfig[i]);
		UA_Server_setWriterGroupDisabled(pub.server, wg[i]);
	}
	pub.addVariableToPublish(VAR_TYPE_DOUBLE, wg[0], ID_DOUBLE);
	pub.addVariableToPublish(VAR_TYPE_FLOAT, wg[1], ID_FLOAT);
	pub.addVariableToPublish(VAR_TYPE_BOOLEAN, wg[2], ID_BOOLEAN);
	pub.addVariableToPublish(VAR_TYPE_BYTE, wg[3], ID_BYTE);
	pub.addVariableToPublish(VAR_TYPE_INT32, wg[4], ID_INT32);
	pub.addVariableToPublish(VAR_TYPE_UINT32, wg[5], ID_UINT32);

	Datetime:
	memset(&wg[6], 0, sizeof(UA_WriterGroupConfig));
	pub.publishDateTime(1000, &wg[6], connId);

    UA_Server_setWriterGroupDisabled(pub.server, wg[6]);

	using namespace std;
	thread t(UserInput::handlingWriterGroup, &pub, wg, wgConfig);
	t.detach();

	int retval = pub.run();

	InputArguments::cleanup();

    return retval;
}
