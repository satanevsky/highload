component_lib=libHelloFastCGI.so
client_helloworld=client_helloworld.o

component_lib:
	g++ HelloFastCGI.cpp -O2 -fPIC -shared -o ${component_lib} -lfastcgi-daemon2 -lcassandra  

client_helloworld:
	g++ cassandra_client_helloworld.cpp -lcassandra -O2 -o ${client_helloworld}
