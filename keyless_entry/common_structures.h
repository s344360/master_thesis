
#ifndef COMMON_STRUCTURES_H_
#define COMMON_STRUCTURES_H_



#include <open62541/types.h>
#include <open62541/types_generated.h>

typedef struct{
	UA_String transportProfile;
	UA_NetworkAddressUrlDataType networkAddressUrl;
	UA_UInt16 serverPort;
}  InputData;


#endif /* COMMON_STRUCTURES_H_ */






















