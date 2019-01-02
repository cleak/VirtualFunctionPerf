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
  FriendlyGreeter* friendly_greeter = new FriendlyGreeter;
  GenericGreeter* generic_greeter = (GenericGreeter*)friendly_greeter;

  friendly_greeter->Greet("Bob");
  generic_greeter->Greet("Alice");

  delete friendly_greeter;

  return 0;
}
