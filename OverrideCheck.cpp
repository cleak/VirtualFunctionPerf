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
  virtual void LateUpdate() {}
  virtual void PhysicsUpdate() {}
  virtual void DrawImmediate() {}
};

struct SceneObject_VTable {
  void* dtor;
  UpdateFn* update;
  UpdateFn* late_update;
  UpdateFn* physics_update;
  UpdateFn* draw_immediate;
};

SceneObject_VTable* GetVtable(SceneObject* obj) {
  SceneObject_VTable** vtable_ptr = (SceneObject_VTable**)obj;
  return *vtable_ptr;
}

class SceneManager {
 public:
  SceneManager() {
    null_vtable_ = GetVtable(&null_object_);
  }

  void Add(SceneObject* obj) {
    SceneObject_VTable* vtable = GetVtable(obj);

    if (vtable->update != null_vtable_->update) {
      update_fns_.insert({ vtable->update, obj });
    }

    if (vtable->late_update != null_vtable_->late_update) {
      late_update_fns_.insert({ vtable->late_update, obj });
    }

    if (vtable->physics_update != null_vtable_->physics_update) {
      physics_update_fns_.insert({ vtable->physics_update, obj });
    }

    if (vtable->draw_immediate != null_vtable_->draw_immediate) {
      draw_immediate_fns_.insert({ vtable->draw_immediate, obj });
    }
  }

  void UpdateAll() {
    for (auto& fn : update_fns_) {
      fn.first(fn.second);
    }
  }

  void LateUpdateAll() {
    for (auto& fn : late_update_fns_) {
      fn.first(fn.second);
    }
  }

  void PhysicsUpdateAll() {
    for (auto& fn : physics_update_fns_) {
      fn.first(fn.second);
    }
  }

  void DrawImmediateAll() {
    for (auto& fn : draw_immediate_fns_) {
      fn.first(fn.second);
    }
  }

 private:
  SceneObject null_object_;
  SceneObject_VTable* null_vtable_;

  std::set<SceneObject*> objects_;

  std::set<std::pair<UpdateFn*, void*>, CompareUpdateFns> update_fns_;
  std::set<std::pair<UpdateFn*, void*>, CompareUpdateFns> late_update_fns_;
  std::set<std::pair<UpdateFn*, void*>, CompareUpdateFns> physics_update_fns_;
  std::set<std::pair<UpdateFn*, void*>, CompareUpdateFns> draw_immediate_fns_;
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
