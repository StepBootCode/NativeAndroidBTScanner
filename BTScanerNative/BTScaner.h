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

	WCHAR_T* m_pwstrMACAddress;
	WCHAR_T* m_pwstrDescription;
	WCHAR_T* m_pwstrLastErrorDesc;
	WCHAR_T* m_pwstrParameters;

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

	WCHAR_T* GetDescription();

	uint32_t GetLastErrorCode();
	WCHAR_T* GetLastErrorDesc();

	WCHAR_T* GetParameters();
	bool SetParameter(wchar_t* name, wchar_t* value);

};