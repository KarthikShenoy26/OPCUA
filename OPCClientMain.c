#include <stdio.h>
#include "open62541.h"

int main(int argc, char *argv[])
{
    if(argc<2){
        printf("Error: Need server address as input argument\n");
        return 0;
    }
    
    char * serverAddress=argv[1];
    printf("Server address is : %s\n",serverAddress);
    
    /* Create a client and connect */
    UA_Client *client = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode status = UA_Client_connect(client, serverAddress); //"opc.tcp://localhost:4840"
    if(status != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return status;
    }

    while(1)
    {
    
        if(true)
        {
            /* Read the value attribute of the node. UA_Client_readValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_read. */
            UA_Variant value; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&value);
            status = UA_Client_readValueAttribute(client, UA_NODEID_STRING(1, "Accelerometer_X"), &value);
            if(status == UA_STATUSCODE_GOOD &&
               UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_UINT16])) {
                printf("Accelerometer X value: %i\n", *(UA_UInt16*)value.data);
            }

            /* Clean up */
            UA_Variant_deleteMembers(&value);
        }


        if(true)
        {
            /* Read the value attribute of the node. UA_Client_readValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_read. */
            UA_Variant value; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&value);
            status = UA_Client_readValueAttribute(client, UA_NODEID_STRING(1, "Accelerometer_Y"), &value);
            if(status == UA_STATUSCODE_GOOD &&
               UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_UINT16])) {
                printf("Accelerometer Y value: %i\n", *(UA_UInt16*)value.data);
            }

            /* Clean up */
            UA_Variant_deleteMembers(&value);
        }


         if(true)
        {
            /* Read the value attribute of the node. UA_Client_readValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_read. */
            UA_Variant value; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&value);
            status = UA_Client_readValueAttribute(client, UA_NODEID_STRING(1, "Accelerometer_Z"), &value);
            if(status == UA_STATUSCODE_GOOD &&
               UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_UINT16])) {
                printf("Accelerometer Z value: %i\n", *(UA_UInt16*)value.data);
            }

            /* Clean up */
            UA_Variant_deleteMembers(&value);
        }
    }
    
    UA_Client_delete(client); /* Disconnects the client internally */
    return status;
}
