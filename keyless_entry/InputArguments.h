
#ifndef INPUTARGUMENTS_H_
#define INPUTARGUMENTS_H_

#include "common_structures.h"
#include <string>

using namespace std;

class InputArguments {
	static char *transport_profile;
	static char *address_url;

	static int decode(uint index, char **argument, InputData &data);
	static int decodeFile(string &lineText, InputData &data);

public:

	static bool inputSpecified;

	InputArguments();
	virtual ~InputArguments();

	static void cleanup();

	static void interpretArguments(int argc, char **argv, InputData &data);
	static bool interpretFromFile(const char *filename, InputData &data);
	static bool interpretFromDefaultConfigFile(char *programPath, InputData &data,  const char *filename = "default_config.txt");
};

#endif /* INPUTARGUMENTS_H_ */
