#include <iostream>
#include <set>
#include <utility>

typedef void (UpdateFn)(void*);

struct CompareUpdateFns {
  bool operator()(const std::pair<UpdateFn*, void*>& a,
                  const std::pair<UpdateFn*, void*>& b) const {
    if (a.first == b.first) {
      return a.second < b.second;
    }
    return a.first < b.first;
  }
};

class SceneObject {
 public:
  virtual ~SceneObject() {}
  virtual void Update() {}
};

struct SceneObject_VTable {
  void* dtor;
  UpdateFn* update;
};

UpdateFn* GetUpdateFn(SceneObject* obj) {
  SceneObject_VTable** vtable_ptr = (SceneObject_VTable**)obj;
  return (*vtable_ptr)->update;
}

class SceneManager {
 public:

  void Add(SceneObject* obj) {
    update_fns_.insert({ GetUpdateFn(obj), obj });
  }

  void UpdateAll() {
    for (auto& update : update_fns_) {
      update.first(update.second);
    }
  }

 private:
  std::set<std::pair<UpdateFn*, void*>, CompareUpdateFns> update_fns_;
};

class Obj1 : public SceneObject { 
  void Update() override {
    std::cout << "## Object 1!" << std::endl;
  }
};

class Obj2 : public SceneObject { 
  void Update() override {
    std::cout << "** Object 2!" << std::endl;
  }
};

int main() {
  SceneManager scene_manager;
  Obj1 a;
  Obj2 b;
  Obj1 c;

  scene_manager.Add(&a);
  scene_manager.Add(&b);
  scene_manager.Add(&c);

  scene_manager.UpdateAll();

  return 0;
}
