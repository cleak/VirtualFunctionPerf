#include <iostream>
#include <fstream>
#include <sstream>

#include "PerfTimer.h"

using namespace std;

static int g_update_count = 0;
volatile int* g_cahceUnfriendlyBlock = nullptr;

void FlushCache() {
  constexpr int kBlockSize = 1024 * 1024 * 4;
  if (!g_cahceUnfriendlyBlock) {
    g_cahceUnfriendlyBlock = new int[kBlockSize];
  }

  for (int i = 0; i < kBlockSize; ++i) {
    g_cahceUnfriendlyBlock[i] = rand();
  }
}

class IncrementerBase {
 public:
  IncrementerBase() {
    my_num = rand();
  }
  virtual ~IncrementerBase() {}
  virtual void Increment() {}
  int my_num;
};

class IncrementerVirtual : public IncrementerBase {
 public:
  IncrementerVirtual() { }
  virtual ~IncrementerVirtual() {}

  virtual void Increment() override {
    g_update_count += my_num;
  }
};

class IncrementerDirect : public IncrementerBase {
 public:
  IncrementerDirect() {}
  virtual ~IncrementerDirect() {}

  // Preventing inling to force a fair comparison.
  __declspec(noinline) void IncrementDirect() {
    g_update_count += my_num;
  }
};

volatile bool g_useIncrementers = true;

typedef void(*VoidMemberFn)(void*);

// Retrieves a pointer to the given object's VTable.
template <typename T>
void** GetVTable(T* obj) {
  return *((void***)obj);
}

// Runs and records timing for virtual function calls, optionally clearing the cache before each run.
double RunVirtualCalls(int objCount, int runCount, bool clearCacheBeforeRun) {
  IncrementerBase** objs = new IncrementerBase*[objCount];
  if (!g_useIncrementers) {
    // No-op to prevent compiler optimization
    for (int i = 0; i < objCount; ++i) {
      objs[i] = new IncrementerBase();
    }
  }
  else {
    for (int i = 0; i < objCount; ++i) {
      objs[i] = new IncrementerVirtual();
    }
  }

  double runTime = 0;
  PerfTimer timer;
  for (int j = 0; j < runCount; ++j) {
    if (clearCacheBeforeRun) {
      FlushCache();
    }

    timer.Start();
    for (int i = 0; i < objCount; ++i) {
      objs[i]->Increment();
    }
    runTime += timer.Stop();
  }

  // Cleanup
  for (int i = 0; i < objCount; ++i) {
    delete objs[i];
  }
  delete[] objs;

  return runTime;
}

// Runs and records timing for virtual function calls, optionally clearing the cache before each run.
double RunVirtualDirectCalls(int objCount, int runCount, bool clearCacheBeforeRun) {
  IncrementerBase** objs = new IncrementerBase*[objCount];
  if (!g_useIncrementers) {
    // No-op to prevent compiler optimization
    for (int i = 0; i < objCount; ++i) {
      objs[i] = new IncrementerBase();
    }
  } else {
    for (int i = 0; i < objCount; ++i) {
      objs[i] = new IncrementerVirtual();
    }
  }

  VoidMemberFn* updateFn = (VoidMemberFn*)(GetVTable(dynamic_cast<IncrementerBase*>(objs[0])) + 0);

  double runTime = 0;
  PerfTimer timer;
  for (int j = 0; j < runCount; ++j) {
    if (clearCacheBeforeRun) {
      FlushCache();
    }

    timer.Start();
    for (int i = 0; i < objCount; ++i) {
      //objs[i]->Increment();
      (*updateFn)(objs[i]);
    }
    runTime += timer.Stop();
  }

  // Cleanup
  for (int i = 0; i < objCount; ++i) {
    delete objs[i];
  }
  delete[] objs;

  return runTime;
}

// Runs and records timing for virtual direct calls, optionally clearing the cache before each run.
double RunDirectCalls(int objCount, int runCount, bool clearCacheBeforeRun) {
  IncrementerDirect** objs = new IncrementerDirect*[objCount];

  for (int i = 0; i < objCount; ++i) {
    objs[i] = new IncrementerDirect();
  }

  double runTime = 0;
  PerfTimer timer;

  for (int j = 0; j < runCount; ++j) {
    if (clearCacheBeforeRun) {
      FlushCache();
    }

    timer.Start();
    for (int i = 0; i < objCount; ++i) {
      objs[i]->IncrementDirect();
    }
    runTime += timer.Stop();
  }

  // Cleanup
  for (int i = 0; i < objCount; ++i) {
    delete objs[i];
  }
  delete[] objs;

  return runTime;
}

int main(int argc, char** argv) {
#ifdef _DEBUG
  constexpr int kNumRuns = 16;
#else
  //constexpr int kNumRuns = 64;
  constexpr int kNumRuns = 32;
#endif

  constexpr int kObjStepSize = 256;
  constexpr int kMaxObjs = kObjStepSize * 4 * 64;

  //constexpr int kObjStepSize = 1024 * 8;
  //constexpr int kMaxObjs = kObjStepSize * 4;

  ofstream fout("results.csv");

  for (int n = kObjStepSize; n <= kMaxObjs; n += kObjStepSize) {
    stringstream ss;
    ss << n << ", ";
    ss << RunVirtualCalls(n, kNumRuns, false) * 1000.0 << ", ";
    ss << RunDirectCalls(n, kNumRuns, false) * 1000.0 << ", ";
    ss << RunVirtualCalls(n, kNumRuns, true) * 1000.0 << ", ";
    ss << RunDirectCalls(n, kNumRuns, true) * 1000.0 << ", ";
    ss << RunVirtualDirectCalls(n, kNumRuns, true) * 1000.0;

    cout << ss.str() << endl;
    fout << ss.str() << endl;
  }

  fout.close();

  system("PAUSE");
  return 0;
}
