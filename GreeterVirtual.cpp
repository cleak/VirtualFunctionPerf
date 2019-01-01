#include <iostream>

using namespace std;

class GenericGreeter {
 public:
	virtual void Greet(const char* name) {
		cout << "Hi " << name << "." << endl;
	}
};

class FriendlyGreeter : public GenericGreeter {
 public:
	virtual void Greet(const char* name) {
		cout << "Hello " << name << "! It's a pleasure to meet you!" << endl;
	}
};

int main() {
	FriendlyGreeter* friendlyGreeter = new FriendlyGreeter;
	GenericGreeter* genericGreeter = (GenericGreeter*)friendlyGreeter;

	friendlyGreeter->Greet("Bob");
	genericGreeter->Greet("Alice");

	delete friendlyGreeter;

	return 0;
}
