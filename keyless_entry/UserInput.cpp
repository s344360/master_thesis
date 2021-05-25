
#include <open62541/server_pubsub.h>
#include "UserInput.h"
#include <limits>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

Publisher *UserInput::pub;
UA_NodeId UserInput::wg[WRITERGROUP_NUMBER];
UA_WriterGroupConfig UserInput::wgConfig[WRITERGROUP_NUMBER];

UserInput::UserInput() {
	// TODO Auto-generated constructor stub

}

UserInput::~UserInput() {
	// TODO Auto-generated destructor stub
}

int UserInput::disable(char *p) {
	if (strcmp(p, "date") == 0) {
		cout << "Disable publishing datetime" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[6]);
		return 0;
	}
	else if (strcmp(p, "double") == 0) {
		cout << "Disable publishing double" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[0]);
		return 0;
	}
	else if (strcmp(p, "float") == 0) {
		cout << "Disable publishing float" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[1]);
		return 0;
	}
	else if (strcmp(p, "bool") == 0) {
		cout << "Disable publishing boolean" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[2]);
		return 0;
	}
	else if (strcmp(p, "byte") == 0) {
		cout << "Disable publishing byte" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[3]);
		return 0;
	}
	else if (strcmp(p, "int") == 0) {
		cout << "Disable publishing int" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[4]);
		return 0;
	}
	else if (strcmp(p, "uint") == 0) {
		cout << "Disable publishing unsigned int" << endl;
		UA_Server_setWriterGroupDisabled(pub->server, wg[5]);
		return 0;
	}
	else if (strcmp(p, "all") == 0) {
		cout << "Disable all publishing" << endl;
		for(UA_Byte i = 0; i < WRITERGROUP_NUMBER; i++) {
			UA_Server_setWriterGroupDisabled(pub->server, wg[i]);
		}
		return 1;
	}
	return 1;
}

int UserInput::enable(char *p) {
	if (strcmp(p, "date") == 0) {
		cout << "Enable publishing datetime" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[6]);
		return 0;
	}
	else if (strcmp(p, "double") == 0) {
		cout << "Enable publishing double" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[0]);
	}
	else if (strcmp(p, "float") == 0) {
		cout << "Enable publishing float" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[1]);
		return 0;
	}
	else if (strcmp(p, "bool") == 0) {
		cout << "Enable publishing boolean" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[2]);
		return 0;
	}
	else if (strcmp(p, "byte") == 0) {
		cout << "Enable publishing byte" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[3]);
		return 0;
	}
	else if (strcmp(p, "int") == 0) {
		cout << "Enable publishing int" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[4]);
		return 0;
	}
	else if (strcmp(p, "uint") == 0) {
		cout << "Enable publishing unsigned int" << endl;
		UA_Server_setWriterGroupOperational(pub->server, wg[5]);
		return 0;
	}
	else if (strcmp(p, "all") == 0) {
		cout << "Enable all publishing" << endl;
		for(UA_Byte i = 0; i < WRITERGROUP_NUMBER; i++) {
			UA_Server_setWriterGroupOperational(pub->server, wg[i]);
		}
		return 1;
	}
	return 1;
}

int UserInput::set(char *p, char *val) {
	if(strcmp(p, "double") == 0) {
		UA_Double v = atof(val);
		cout << "Double set to " << val << endl;
		pub->writeValueToVariable(VAR_TYPE_DOUBLE, &v, ID_DOUBLE);
		return 0;
	}
	else if(strcmp(p, "float") == 0) {
		UA_Float v = atof(val);
		cout << "Float set to " << val << endl;
		pub->writeValueToVariable(VAR_TYPE_FLOAT, &v, ID_FLOAT);
		return 0;
	}
	else if(strcmp(p, "bool") == 0) {
		static UA_Boolean v = false;
		if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0) {
			cout << "Boolean set to true" << endl;
			v = true;
		}
		else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0) {
			cout << "Boolean set to false" << endl;
			v = false;
		}
		else {
			cout << "Invalid boolean value!" << endl;
		}
		pub->writeValueToVariable(VAR_TYPE_BOOLEAN, &v, ID_BOOLEAN);

        UA_Server_setWriterGroupOperational(pub->server, wg[2]);
        //sleep(5);
        UA_Server_setWriterGroupDisabled(pub->server, wg[2]);


		return 0;
	}
	else if(strcmp(p, "byte") == 0) {
		UA_Byte v = atoi(val);
		cout << "Byte set to " << v << endl;
		pub->writeValueToVariable(VAR_TYPE_BYTE, &v, ID_BYTE);
		return 0;
	}
	else if(strcmp(p, "int") == 0) {
		UA_Int32 v = atoi(val);
		cout << "Int set to " << v << endl;
		pub->writeValueToVariable(VAR_TYPE_INT32, &v, ID_INT32);
		return 0;
	}
	else if(strcmp(p, "uint") == 0) {
		UA_UInt32 v = atoi(val);
		cout << "Usigned int set to " << v << endl;
		pub->writeValueToVariable(VAR_TYPE_UINT32, &v, ID_UINT32);
		return 0;
	}
	else if(strcmp(p, "all") == 0) {
		cout << "There is no option 'all' for command 'set'" << endl;
		return 0;
	}
	return 1;
}

int UserInput::setInterval(char *p, char *val) {
	if (strcmp(p, "date") == 0) {
			wgConfig[6].publishingInterval = atoi(val);//string to int
			cout << "Set publishing interval of datetime to " << wgConfig[6].publishingInterval << "ms" << endl;
			UA_Server_updateWriterGroupConfig(pub->server, wg[6], &wgConfig[6]);
			return 0;
		}
	else if(strcmp(p, "double") == 0) {
		wgConfig[0].publishingInterval = atoi(val);
		cout << "Set publishing interval of double to " << wgConfig[0].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[0], &wgConfig[0]);
		return 0;
	}
	else if(strcmp(p, "float") == 0) {
		wgConfig[1].publishingInterval = atoi(val);
		cout << "Set publishing interval of float to " << wgConfig[1].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[1], &wgConfig[1]);
		return 0;
	}
	else if(strcmp(p, "bool") == 0) {
		wgConfig[2].publishingInterval = atoi(val);
		cout << "Set publishing interval of boolean to " << wgConfig[2].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[2], &wgConfig[2]);
		return 0;
	}
	else if(strcmp(p, "byte") == 0) {
		wgConfig[3].publishingInterval = atoi(val);
		cout << "Set publishing interval of byte to " << wgConfig[3].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[3], &wgConfig[3]);
		return 0;
	}
	else if(strcmp(p, "int") == 0) {
		wgConfig[4].publishingInterval = atoi(val);
		cout << "Set publishing interval of int to " << wgConfig[4].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[4], &wgConfig[4]);
		return 0;
	}
	else if(strcmp(p, "uint") == 0) {
		wgConfig[5].publishingInterval = atoi(val);
		cout << "Set publishing interval of unsigned int to " << wgConfig[5].publishingInterval << "ms" << endl;
		UA_Server_updateWriterGroupConfig(pub->server, wg[5], &wgConfig[5]);
		return 0;
	}
	else if(strcmp(p, "all") == 0) {
		cout << "Set publishing interval of all to " << atoi(val) << "ms" << endl;
		for(UA_Byte i = 0; i < WRITERGROUP_NUMBER; i++) {
			wgConfig[i].publishingInterval = atoi(val);
			UA_Server_updateWriterGroupConfig(pub->server, wg[i], &wgConfig[i]);
		}
		return 1;
	}
	return 1;
}

void UserInput::decode(char *s) {
	const char *delim = " \t";
	char *ptr = strtok(s, delim);

	while(ptr != NULL)
	{
		if(strcmp(ptr, "disable") == 0) {
			ptr = strtok(NULL, delim);
			if(ptr != NULL) {
				while (ptr != NULL && disable(ptr) == 0) {
					ptr = strtok(NULL, delim);
				}
				continue;
			}
			else {
				cout << "\n   Type: " << HINT_DISABLE << endl;
			}
		}
		else if(strcmp(ptr, "enable") == 0) {
			ptr = strtok(NULL, delim);
			if(ptr != NULL) {
				while (ptr != NULL && enable(ptr) == 0) {
					ptr = strtok(NULL, delim);
				}
				continue;
			}
			else {
				cout << "\n   Type: " << HINT_ENABLE << endl;
			}
		}
		else if(strcmp(ptr, "interval") == 0) {
			ptr = strtok(NULL, delim);
			if(ptr != NULL) {
				char *val = ptr;
				ptr = strtok(NULL, delim);
				if(ptr != NULL) {
					while (ptr != NULL && setInterval(ptr, val) == 0) {
						ptr = strtok(NULL, delim);
					}
					continue;
				}
				else {
					cout << "\n   Type: " << HINT_INTERVAL << endl;
				}
			}
			else {
				cout << "\n   Type: " << HINT_INTERVAL << endl;
			}
		}
		else if(strcmp(ptr, "set") == 0) {
			ptr = strtok(NULL, delim);
			if(ptr != NULL) {
				char *val = ptr;
				ptr = strtok(NULL, delim);
				if(ptr != NULL) {
					while (ptr != NULL && set(ptr, val) == 0) {
						ptr = strtok(NULL, delim);
					}
					continue;
				}
				else {
					cout << "\n   Type: " << HINT_SET << endl;
				}
			}
			else {
				cout << "\n   Type: " << HINT_SET << endl;
			}
		}
		else if(strcmp(ptr, "exit") == 0) {
			exit(EXIT_SUCCESS);
		}
		else if(strcmp(ptr, "help") == 0) {
			cout << "Selections:" << endl;
			cout << "\t" << HINT_SELECTION << endl;
			cout << "Commands:" << endl;
			cout << "1.\t" << HINT_ENABLE << endl;
			cout << "2.\t" << HINT_DISABLE << endl;
			cout << "3.\t" << HINT_INTERVAL << endl;
			cout << "4.\t" << HINT_SET << endl;
			cout << "5.\thelp (shows this dialog)" << endl;
			cout << "6.\texit (terminates program)" << endl;
			cout << HINT_GENERAL << endl;
		}
		ptr = strtok(NULL, delim);
	}
}

void UserInput::handlingWriterGroup(Publisher *pub,
		 UA_NodeId *wg, UA_WriterGroupConfig *wgConfig) {
	UserInput::pub = pub;
	for (UA_Byte i = 0; i < WRITERGROUP_NUMBER; i++) {
		UserInput::wg[i] =  wg[i];
		UserInput::wgConfig[i] = wgConfig[i];
	}

	this_thread::sleep_for(chrono::milliseconds(100));

	string input;

	while(pub->isRunning()) {
		cout << endl << "User Input: ";
		getline(cin, input);
//		cout << "You have entered: " << input << endl;
		char *cmdline = strdup(input.c_str());
		decode(cmdline);
		free(cmdline);
	}
}

void UserInput::clearInput() {
	cin.clear(); //clear bad input flag
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); //discard input
}

void UserInput::getInput(string *s, const char *msg) {
	//executes loop if the input fails (e.g., no characters were read)
	while (cout << endl << msg << " " && !(cin >> *s)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	UserInput::clearInput();
}

UA_Byte UserInput::getInputByte(const char *msg) {
	UA_Byte value;
	while (cout << endl << msg << " " && !(cin >> value)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "Accepted Input: " << value << endl;
	UserInput::clearInput();
	return value;
}

UA_Int16 UserInput::getInputInt16(const char *msg) {
	UA_Int16 value;
	while (cout << endl << msg << " " && !(cin >> value)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "Accepted Input: " << value << endl;
	UserInput::clearInput();
	return value;
}

UA_Int32 UserInput::getInputInt32(const char *msg) {
	UA_Int32 value;
	while (cout << endl << msg << " " && !(cin >> value)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "Accepted Input: " << value << endl;
	UserInput::clearInput();
	return value;
}

UA_Int64 UserInput::getInputInt64(const char *msg) {
	UA_Int64 value;
	while (cout << endl << msg << " " && !(cin >> value)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "Accepted Input: " << value << endl;
	UserInput::clearInput();
	return value;
}

UA_Double UserInput::getInputDouble(const char *msg) {
	UA_Double value;
	while (cout << endl << msg << " " && !(cin >> value)) {
		UserInput::clearInput();
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "Accepted Input: " << value << endl;
	UserInput::clearInput();
	return value;
}


void UserInput::setPublishingValue(Publisher *pub, UA_NodeId variableId, const UA_DataType *variableType) {
	UA_Int64 val = UserInput::getInputInt64();
	pub->writeValueToVariable(variableType, &val, variableId);
//	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"\nInput Listener Finished\n" );
}


void UserInput::togglePublishing() {
	if (UserInput::getInputInt16("Enable publishing: > 0; Disable publishing: <= 0") > 0) {
//		pub->enablePublishing(writerGroup);
		UA_Server_setWriterGroupOperational(pub->server, wg[6]);
	} else {
//		pub->disablePublishing(writerGroup) ;
		UA_Server_setWriterGroupDisabled(pub->server, wg[6]);
	}
//	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"\nInput Listener Finished\n" );
}








































