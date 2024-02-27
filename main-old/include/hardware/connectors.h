class Connector {
	bool connected;
	int send(void *buf, int size);
	int recv(void *buf, int size);
};