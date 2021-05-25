
#ifndef USERINPUT_H_
#define USERINPUT_H_

#include "Publisher.h"
#include <string>

#define WRITERGROUP_NUMBER		7

#define HINT_SELECTION		"date double float bool byte int uint"
#define HINT_DISABLE		"disable <selection> or all"
#define HINT_ENABLE			"enable <selection> or all"
#define HINT_INTERVAL		"interval <milliseconds> <selection> or all"
#define HINT_SET			"set <value> <selection>"
#define HINT_GENERAL		"\nNote: It is possible to write multiple commands at once in one line. Commands will be executed from left to right.\n      Also multiple selections are possible."

class UserInput {

public:
	static Publisher *pub;
	static UA_NodeId wg[];
	static UA_WriterGroupConfig wgConfig[];

	UserInput();
	virtual ~UserInput();

	static void handlingWriterGroup(Publisher *pub, UA_NodeId *wg, UA_WriterGroupConfig *wgConfig);
	static void getInput(std::string *s, const char *msg = "Enter something:");
	static UA_Byte getInputByte(const char *msg = "Enter number:");
	static UA_Int16 getInputInt16(const char *msg = "Enter number:");
	static UA_Int32 getInputInt32(const char *msg = "Enter number:");
	static UA_Int64 getInputInt64(const char *msg = "Enter number:");
	static UA_Double getInputDouble(const char *msg = "Enter number:");
	static void setPublishingValue (Publisher *pub, UA_NodeId variableId, const UA_DataType *variableType);
	static void togglePublishing();
	static void clearInput();
	static int disable(char *p);
	static int enable(char *p);
	static int set(char *p, char* val);
	static int setInterval(char *p, char *val);
	static void decode(char *s);
};


#endif /* USERINPUT_H_ */
