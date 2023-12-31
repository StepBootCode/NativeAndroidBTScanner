#pragma once

#include "../include/AddInDefBase.h"
#include "../include/IAndroidComponentHelper.h"
#include "../jni/jnienv.h"
#include "../include/IMemoryManager.h"
#include <string>

class BTScaner
{
private:

	jclass cc;
	jobject obj;

public:

	BTScaner();
	~BTScaner();

	IAddInDefBaseEx *m_iConnect;
	IMemoryManager	*m_iMemory;

	void setCC(jclass _cc);
	void setOBJ(jobject _obj);

	void Initialize(IAddInDefBaseEx* cnn);

	void StopDevice() const;
	void InitDevice(tVariant* paParams, IMemoryManager* iMemory);

	bool Enabled(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) const;
	jstring GetBluetoothDevicesList(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) const;
};