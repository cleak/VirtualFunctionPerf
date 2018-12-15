#include <iostream>
#include <fstream>
#include <sstream>

#include "PerfTimer.h"

using namespace std;

int g_updateCount = 0;
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

class GameObject {
public:
	virtual void Update() {}

	virtual ~GameObject() {}
};

class Incrementer : public GameObject {
public:

	int myNum = 1;

	Incrementer() {
		myNum = rand();
	}

	virtual ~Incrementer() {}

	virtual void Update() override{
		g_updateCount += myNum;
	}
};

class IncrementerDirect {
public:
	
	volatile void* fakeVTable = nullptr;
	int myNum = 1;

	IncrementerDirect() {
		myNum = rand();
	}

	__declspec(noinline) void Update() {
	//void Update() {
		g_updateCount += myNum;
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
	GameObject** objs = new GameObject*[objCount];
	if (!g_useIncrementers) {
		// No-op to prevent compiler optimization
		for (int i = 0; i < objCount; ++i) {
			objs[i] = new GameObject();
		}
	}
	else {
		for (int i = 0; i < objCount; ++i) {
			objs[i] = new Incrementer();
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
			objs[i]->Update();
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
	GameObject** objs = new GameObject*[objCount];
	if (!g_useIncrementers) {
		// No-op to prevent compiler optimization
		for (int i = 0; i < objCount; ++i) {
			objs[i] = new GameObject();
		}
	} else {
		for (int i = 0; i < objCount; ++i) {
			objs[i] = new Incrementer();
		}
	}

	VoidMemberFn* updateFn = (VoidMemberFn*)(GetVTable(dynamic_cast<GameObject*>(objs[0])) + 0);

	double runTime = 0;
	PerfTimer timer;
	for (int j = 0; j < runCount; ++j) {
		if (clearCacheBeforeRun) {
			FlushCache();
		}

		timer.Start();
		for (int i = 0; i < objCount; ++i) {
			//objs[i]->Update();
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
			objs[i]->Update();
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
