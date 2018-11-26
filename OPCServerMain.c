#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include "open62541.h"


#define PC
//#define RASPBERRYPI

#ifdef RASPBERRYPI
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
    
#define MPU_ACCEL_XOUT1 0x3b
#define MPU_ACCEL_XOUT2 0x3c
#define MPU_ACCEL_YOUT1 0x3d
#define MPU_ACCEL_YOUT2 0x3e
#define MPU_ACCEL_ZOUT1 0x3f
#define MPU_ACCEL_ZOUT2 0x40

#define MPU_GYRO_XOUT1 0x43
#define MPU_GYRO_XOUT2 0x44
#define MPU_GYRO_YOUT1 0x45
#define MPU_GYRO_YOUT2 0x46
#define MPU_GYRO_ZOUT1 0x47
#define MPU_GYRO_ZOUT2 0x48

#define MPU_TEMP1 0x41
#define MPU_TEMP2 0x42

#define MPU_POWER1 0x6b
#define MPU_POWER2 0x6c
    
#endif

//-----------------*****************************--------------------------------
/*Global declarations*/
UA_Boolean running = true;
UA_Server *server=NULL;
UA_NodeId accelXNodeId;
UA_NodeId accelYNodeId;
UA_NodeId accelZNodeId;
pthread_t threadID;
 



 
//-----------------*****************************--------------------------------
 //Signal Handler
 void signalHandler(int sig) {  running = false;}
 
 //UA_Write to UINT16 variables
 UA_StatusCode writeVariable_UINT16(UA_Server *server,UA_NodeId nodeID,UA_UInt16 value); 
 
#ifdef PC
 //Random number generator
  unsigned int randRange(unsigned int n,unsigned int seedVal);
#endif
 
 //ACcelerometer data acquisition
 void acquireAccelerometerData(void);
 
 
 
 
 
 
//-----------------*****************************--------------------------------
int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler); /* catch ctrl-c */   
    
    /* Create a server listening on port 4840 */
    UA_ServerConfig *config = UA_ServerConfig_new_default();
    server = UA_Server_new(config);    
    

    /*Accelerometer x axis */
    UA_VariableAttributes accelXAttr = UA_VariableAttributes_default;
    accelXAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Accelerometer_X");
    UA_UInt16 accelXVal = 0;
    UA_Variant_setScalar(&accelXAttr.value, &accelXVal, &UA_TYPES[UA_TYPES_UINT16]);

   
    accelXNodeId = UA_NODEID_STRING(1, "Accelerometer_X");
    UA_NodeId accelXParentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId accelXParentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_QualifiedName accelXBrowseName = UA_QUALIFIEDNAME(1, "Accelerometer_X");

    UA_Server_addVariableNode(server, accelXNodeId, accelXParentNodeId, accelXParentReferenceNodeId,
                              accelXBrowseName, UA_NODEID_NULL, accelXAttr, NULL, NULL);   
    //--------------------------------------------------------------------------    
    
    /*Accelerometer y axis */
    UA_VariableAttributes accelYAttr = UA_VariableAttributes_default;
    accelYAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Accelerometer_Y");
    UA_UInt16 accelYVal = 0;
    UA_Variant_setScalar(&accelYAttr.value, &accelYVal, &UA_TYPES[UA_TYPES_UINT16]);

   
    accelYNodeId = UA_NODEID_STRING(1, "Accelerometer_Y");
    UA_NodeId accelYParentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId accelYParentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_QualifiedName accelYBrowseName = UA_QUALIFIEDNAME(1, "Accelerometer_Y");

    UA_Server_addVariableNode(server, accelYNodeId, accelYParentNodeId, accelYParentReferenceNodeId,
                              accelYBrowseName, UA_NODEID_NULL, accelYAttr, NULL, NULL);   
    //--------------------------------------------------------------------------    
    
    
   /*Accelerometer z axis */
    UA_VariableAttributes accelZAttr = UA_VariableAttributes_default;
    accelZAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Accelerometer_Z");
    UA_UInt16 accelZVal = 0;
    UA_Variant_setScalar(&accelZAttr.value, &accelZVal, &UA_TYPES[UA_TYPES_UINT16]);

   
    accelZNodeId = UA_NODEID_STRING(1, "Accelerometer_Z");
    UA_NodeId accelZParentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId accelZParentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_QualifiedName accelZBrowseName = UA_QUALIFIEDNAME(1, "Accelerometer_Z");

    UA_Server_addVariableNode(server, accelZNodeId, accelZParentNodeId, accelZParentReferenceNodeId,
                              accelZBrowseName, UA_NODEID_NULL, accelZAttr, NULL, NULL);
    //--------------------------------------------------------------------------    
    
    
    //create a thread which acquires  accelerometer data
   if(pthread_create(&threadID, NULL, (void*)acquireAccelerometerData, NULL)) 
   {
       printf("Error creating thread\n");
       return -1;
   }   
    
    //-------------------------------------------------------------------------
    /* Run the server loop */
    UA_StatusCode status = UA_Server_run(server, &running);
    
    //Kill the thread which acquires the accelerometer data
    //to-do
    //pthread_kill(threadID);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return status;
}
 
    
    
   

//-----------------*****************************--------------------------------
UA_StatusCode writeVariable_UINT16(UA_Server *server,UA_NodeId nodeID,UA_UInt16 value) 
{
       /* Write a different integer value */
    UA_UInt16 tempVal = value;
    
    UA_Variant tempVar;
    UA_Variant_init(&tempVar);
    UA_Variant_setScalar(&tempVar, &tempVal, &UA_TYPES[UA_TYPES_UINT16]);
    UA_StatusCode retval=UA_Server_writeValue(server, nodeID, tempVar);
    return retval;
}


//-----------------*****************************--------------------------------
unsigned int prevValue=0;
 unsigned int randRange(unsigned int n,unsigned int seedVal)
{
   unsigned int limit;
   unsigned int r;
   srand(seedVal);
   //limit = RAND_MAX - (RAND_MAX % n);
   //while((r = rand()) >= limit);
   r= rand();
   r=r % n;
   return r;
}


 //-----------------*****************************--------------------------------
void acquireAccelerometerData(void)
{
    
#ifdef RASPBERRYPI
    int fd;
    char *fileName = "/dev/i2c-1";
    int  address = 0x68;
	
    if ((fd = open(fileName, O_RDWR)) < 0) {
        printf("Failed to open i2c port\n");
        exit(1);
    }
	
    if (ioctl(fd, I2C_SLAVE, address) < 0) {
        printf("Unable to get bus access to talk to slave\n");
        exit(1);
    }
    
    int8_t power = i2c_smbus_read_byte_data(fd, MPU_POWER1);
    i2c_smbus_write_byte_data(fd, MPU_POWER1, ~(1 << 6) & power);

    while (1) 
    {
        int16_t temp = i2c_smbus_read_byte_data(fd, MPU_TEMP1) << 8 |
                        i2c_smbus_read_byte_data(fd, MPU_TEMP2);

        int16_t xaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT1) << 8 |
                         i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT2);
        int16_t yaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT1) << 8 |
                         i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT2);
        int16_t zaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT1) << 8 |
                         i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT2);

        int16_t xgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT1) << 8 |
                        i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT2);
        int16_t ygyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT1) << 8 |
                        i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT2);
        int16_t zgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT1) << 8 |
                        i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT2);

        writeVariable_UINT16(server,accelXNodeId,xaccel);
        writeVariable_UINT16(server,accelYNodeId,yaccel);
        writeVariable_UINT16(server,accelZNodeId,zaccel);
        
//        printf("temp: %f\n", (float)temp / 340.0f + 36.53);
//        printf("accel x,y,z: %d, %d, %d\n", (int)xaccel, (int)yaccel, (int)zaccel);
//        printf("gyro x,y,z: %d, %d, %d\n\n", (int)xgyro, (int)ygyro, (int)zgyro);
        sleep(1);
    }
    
#endif
    
#ifdef PC
    while(1)
    {
        UA_UInt16 x=randRange(32600,prevValue);
        writeVariable_UINT16(server,accelXNodeId,x);
        prevValue=x;
        
        UA_UInt16 y=randRange(32600,prevValue);
        writeVariable_UINT16(server,accelYNodeId,y);
        prevValue=y;
        
        UA_UInt16 z=randRange(32600,prevValue);
        writeVariable_UINT16(server,accelZNodeId,z);
        prevValue=z;
    }
#endif    
    
    return;
}







//-----------------*****************************--------------------------------
//        /* Add a variable node */
//    /* 1) Define the node attributes */
//    UA_VariableAttributes attr = UA_VariableAttributes_default;
//    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Accelerometer_X");
//    UA_Int32 myInteger = 69;
//    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
//
//    /* 2) Define where the node shall be added with which browsename */
//    UA_NodeId newNodeId = UA_NODEID_STRING(1, "Accelerometer_X");
//    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
//    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
//    UA_NodeId variableType = UA_NODEID_NULL; /* take the default variable type */
//    UA_QualifiedName browseName = UA_QUALIFIEDNAME(1, "Accelerometer_X");
    
///* 3) Add the node */
//    UA_Server_addVariableNode(server, newNodeId, parentNodeId, parentReferenceNodeId,
//                              browseName, variableType, attr, NULL, NULL);






//    //Creating Accelerometer object and its components
//        //Accelerometer object
//        UA_NodeId AccelID; /* get the nodeid assigned by the server */
//        UA_ObjectAttributes accelObjAttr = UA_ObjectAttributes_default;
//        accelObjAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Accelerometer");
//        UA_Server_addObjectNode(server, UA_NODEID_NULL,
//                                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
//                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
//                                UA_QUALIFIEDNAME(1, "Accelerometer"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
//                                accelObjAttr, NULL, &AccelID);
//
//        //Accelerometer objects components X axis
//        UA_VariableAttributes accelCompXAttr = UA_VariableAttributes_default;
//        UA_UInt16 accelXAxis = 0;
//        UA_Variant_setScalar(&accelCompXAttr.value, &accelXAxis, &UA_TYPES[UA_TYPES_UINT16]);
//        accelCompXAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Xaxis");
//        UA_Server_addVariableNode(server, UA_NODEID_NULL, AccelID,
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                                  UA_QUALIFIEDNAME(1, "Xaxis"),
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), accelCompXAttr, NULL, NULL);
//
//        //Accelerometer objects components Y axis 
//        UA_VariableAttributes accelCompYAttr = UA_VariableAttributes_default;
//        UA_UInt16 accelYAxis = 0;
//        UA_Variant_setScalar(&accelCompYAttr.value, &accelYAxis, &UA_TYPES[UA_TYPES_UINT16]);
//        accelCompYAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Yaxis");
//        UA_Server_addVariableNode(server, UA_NODEID_NULL, AccelID,
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                                  UA_QUALIFIEDNAME(1, "Yaxis"),
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), accelCompYAttr, NULL, NULL);
//
//        //Accelerometer objects components Z axis
//        UA_VariableAttributes accelZAxis = UA_VariableAttributes_default;
//        UA_UInt16 accelZAxis = 0;
//        UA_Variant_setScalar(&accelZAxis.value, &accelZAxis, &UA_TYPES[UA_TYPES_UINT16]);
//        accelZAxis.displayName = UA_LOCALIZEDTEXT("en-US", "Zaxis");
//        UA_Server_addVariableNode(server, UA_NODEID_NULL, AccelID,
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                                  UA_QUALIFIEDNAME(1, "Zaxis"),
//                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), accelZAxis, NULL, NULL);