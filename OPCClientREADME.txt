Command compile the client 

gcc -std=c99 -o opcuaclient main.c open62541.c -lpthread


Running the client

./opcuaclient opc.tcp://IPADDRESS_OF_SERVER:4840
