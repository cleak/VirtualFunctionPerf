#include <iostream>

using namespace std;

// Our Greet function type.
typedef void (GreetFn)(void*, const char* name);
struct GenericGreeter_VTable {
  GreetFn* greet;
};

// Forward declare VTable instances.
extern GenericGreeter_VTable generic_vtable;
extern GenericGreeter_VTable friendly_vtable;

class GenericGreeter {
 public:
  GenericGreeter_VTable* vtable;

  GenericGreeter() {
    vtable = &generic_vtable;
  }

  static void GreetGeneric(void* _this, const char* name) {
    cout << "Hi " << name << "." << endl;
  }
};

class FriendlyGreeter : public GenericGreeter {
 public:
  FriendlyGreeter() {
    vtable = &friendly_vtable;
  }

  static void GreetFriendly(void* _this, const char* name) {
    cout << "Hello " << name << "! It's a pleasure to meet you!" << endl;
  }
};

// Create the static VTable instances
GenericGreeter_VTable generic_vtable = {
  (GreetFn*)&GenericGreeter::GreetGeneric
};

GenericGreeter_VTable friendly_vtable = {
  (GreetFn*)&FriendlyGreeter::GreetFriendly
};

int main() {
  FriendlyGreeter* friendly_greeter = new FriendlyGreeter;
  GenericGreeter* generic_greeter = (GenericGreeter*)friendly_greeter;

  friendly_greeter->vtable->greet(friendly_greeter, "Bob");
  generic_greeter->vtable->greet(generic_greeter, "Alice");

  delete friendly_greeter;

  return 0;
}
