#include <iostream>

using namespace std;

class GenericGreeter {
 public:
  virtual void Greet(const char* name) {
    cout << "Hi " << name << "." << endl;
  }

  virtual void Dismiss(const char* name) {
    cout << "Bye " << name << "." << endl;
  }
};

class FriendlyGreeter : public GenericGreeter {
 public:
  virtual void Greet(const char* name) {
    cout << "Hello " << name << "! It's a pleasure to meet you!" << endl;
  }

  virtual void Dismiss(const char* name) {
    cout << "Farewell " << name << "! Until later!" << endl;
  }
};

// Manually define what the VTable looks like.
typedef void (GreetFn)(void*, const char* name);
struct GenericGreeter_VTable {
  GreetFn* greet;
  GreetFn* dismiss;
};

GenericGreeter_VTable* GetVTable(GenericGreeter* obj) {
  GenericGreeter_VTable** vtable_ptr = (GenericGreeter_VTable**)obj;
  return *(vtable_ptr);
}

int main() {
  FriendlyGreeter* friendly_greeter = new FriendlyGreeter;
  GenericGreeter* generic_greeter = (GenericGreeter*)friendly_greeter;

  GetVTable(friendly_greeter)->greet(friendly_greeter, "Bob");
  GetVTable(friendly_greeter)->dismiss(friendly_greeter, "Bob");

  GetVTable(generic_greeter)->greet(generic_greeter, "Alice");
  GetVTable(generic_greeter)->dismiss(generic_greeter, "Alice");

  delete friendly_greeter;

  return 0;
}
