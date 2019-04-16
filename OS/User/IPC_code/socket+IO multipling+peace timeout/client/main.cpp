#include "client.h"
using namespace std;

int main(){
	Client* client = new Client(8888, "10.214.149.119");
	int res = client->run();
	delete client;
	return 0;
}
