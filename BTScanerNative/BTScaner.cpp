#include <wchar.h>
#include <thread>
#include "BTScaner.h"
#include "ConversionWchar.h"
#include "AddInNative.h"

static const wchar_t g_EventSource[] = L"ru.bootcode.btscaner";
static WcharWrapper s_EventSource(g_EventSource);

static const wchar_t g_EventBarCode[] = L"barcode";
static WcharWrapper s_EventBarCode(g_EventBarCode);

static const wchar_t g_EventCancel[] = L"cancel";
static WcharWrapper s_EventCancel(g_EventCancel);
extern "C"
JNIEXPORT void JNICALL
Java_ru_bootcode_btscaner_BTScaner_OnBarcode(JNIEnv *env, jclass clazz, jlong p_object, jstring s_barcode) {
	
	// The object corresponds to the one passed to initialize the Activity, see the line in the Initialize procedure
	// jobject objloc = env->NewObject(cc, methID, activity, (jlong)this);
	// (jlong)this - will be returned to us as an object(i.e.a reference to an object of our class BTScaner)
	
	//Def: IAddInDefBaseEx* pAddIn = (IAddInDefBaseEx*)p_object;
	BTScaner* scaner = (BTScaner*)p_object;

	if (scaner != nullptr) {
		WCHAR_T* wcBarcode = nullptr;
		jstring2v8string(env, scaner->m_iMemory, s_barcode, &wcBarcode);

		scaner->m_iConnect->ExternalEvent(s_EventSource, s_EventBarCode, wcBarcode);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_ru_bootcode_btscaner_BTScaner_OnCancel(JNIEnv *env, jclass clazz,
															 jlong p_object) {
	BTScaner* scaner = (BTScaner*)p_object;

	if (scaner != nullptr)
		scaner->m_iConnect->ExternalEvent(s_EventSource, s_EventCancel, nullptr);
}


BTScaner::BTScaner() : cc(nullptr), obj(nullptr)
{
}

BTScaner::~BTScaner()
{
	if (obj)
	{
		JNIEnv* env = getJniEnv();
		jmethodID methID = env->GetMethodID(cc, "getParameters", "()Ljava/lang/String;");

		jstring res = (jstring)env->CallObjectMethod(obj, methID);
		
		m_pwstrParameters = 0;
		if (res)
		{
			JNIEnv* env = getJniEnv();
			int* lSize = nullptr;
			*lSize = env->GetStringLength(res);
			const WCHAR_T* pjstr = env->GetStringChars(res, NULL);
			m_iMemory->AllocMemory((void**)&m_pwstrDescription, (*lSize + 1) * sizeof(WCHAR_T));
			for (auto i = 0; i < *lSize; ++i)
				m_pwstrDescription[i] = pjstr[i];
			m_pwstrDescription[*lSize] = 0;
			env->ReleaseStringChars(res, pjstr);
		}

		wchar_t description[] =
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?><DriverDescription Name=\"Bluetooth сканер штрихкода\" Description=\"Bluetooth сканер штрихкода\" EquipmentType=\"BTСканерШтрихкода\" IntegrationComponent=\"false\" MainDriverInstalled=\"false\" DriverVersion=\"1.0.0.0\" IntegrationComponentVersion=\"1.0\" DownloadURL=\"\" LogIsEnabled=\"false\" LogPath = \"\"/>";
		m_pwstrDescription = 0;
		convToShortWchar(&m_pwstrDescription, description);


		//wchar_t parameters[] =
		//	L"<Settings><Group Caption=\"Параметры подключения\"><Parameter Name=\"MACAddress\" Caption=\"MAC адрес\" TypeValue=\"String\" DefaultValue=\"\"><ChoiceList><Item Value=\"broadcast\">broadcast event</Item><Item Value=\"clipboard\">clipboard</Item></ChoiceList></Parameter></Group></Settings>";
		//m_pwstrParameters = 0;
		//convToShortWchar(&m_pwstrParameters, parameters);

		StopDevice(); 

		env->DeleteGlobalRef(obj);
		env->DeleteGlobalRef(cc);
	}
}

void BTScaner::Initialize(IAddInDefBaseEx* cnn)
{
	m_iConnect = cnn;
	if (!obj)
	{
		IAndroidComponentHelper* helper = (IAndroidComponentHelper*)cnn->GetInterface(eIAndroidComponentHelper);
		if (helper)
		{
			WCHAR_T* className = nullptr;
			convToShortWchar(&className, L"ru.bootcode.btscaner.BTScaner");
			jclass ccloc = helper->FindClass(className);
			delete[] className;
			className = nullptr;
			if (ccloc)
			{
				JNIEnv* env = getJniEnv();
				cc = static_cast<jclass>(env->NewGlobalRef(ccloc));
				env->DeleteLocalRef(ccloc);

				jobject activity = helper->GetActivity();
				jmethodID methID = env->GetMethodID(cc, "<init>", "(Landroid/app/Activity;J)V");
				jobject objloc = env->NewObject(cc, methID, activity, (jlong)this);  //cnn
				obj = static_cast<jobject>(env->NewGlobalRef(objloc));
				env->DeleteLocalRef(objloc);
				methID = env->GetMethodID(cc, "show", "()V");
				env->CallVoidMethod(obj, methID);
				env->DeleteLocalRef(activity);
			}
		}
	}
}

void BTScaner::InitDevice(tVariant* paParams, IMemoryManager* iMemory)
{
	// Examples:
	// (FI)V → void func(float, int)
	// (FF)V → void func(float, float)
	// (FI)Z → boolean func(float, int)
	// ()Z → boolean func()
	// (Ljava/lang/String;)Z → boolean func(String)

	m_iMemory = iMemory;

	if (obj)
	{
		JNIEnv* env = getJniEnv();

		jstring jMAC = m_pwstrMACAddress != NULL ?
			env->NewString(m_pwstrMACAddress, getLenShortWcharStr(m_pwstrMACAddress)) :
			NULL;

		//jstring jMAC = env->NewString(paParams[0].pwstrVal, paParams[0].wstrLen);

		jmethodID methID = env->GetMethodID(cc, "initDevice", "(Ljava/lang/String;)V");
		env->CallVoidMethod(obj, methID, jMAC);
	}
}

void BTScaner::StopDevice() const
{
	if (obj)
	{
		JNIEnv* env = getJniEnv();
		jmethodID methID = env->GetMethodID(cc, "stopDevice", "()V");
		env->CallVoidMethod(obj, methID);
	}
}

bool BTScaner::Enabled(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) const
{
	if (obj)
	{
		JNIEnv* env = getJniEnv();
		jmethodID methID = env->GetMethodID(cc, "enabled", "()Z");

		jboolean res = (jboolean)env->CallBooleanMethod(obj, methID);

		return (bool)(res == JNI_TRUE);
	}
	return false;
}

jstring BTScaner::GetBluetoothDevicesList(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) const
{

	if (obj)
	{
		JNIEnv* env = getJniEnv();
		jmethodID methID = env->GetMethodID(cc, "getBluetoothDevicesList", "()Ljava/lang/String;");

		jstring res = (jstring)env->CallObjectMethod(obj, methID);
		return res;
	}
	jstring defres = nullptr;
	return defres;
}


WCHAR_T* BTScaner::GetDescription()
{
	return m_pwstrDescription;
}

uint32_t BTScaner::GetLastErrorCode()
{
	return 0L;
}

WCHAR_T* BTScaner::GetLastErrorDesc()
{
	return m_pwstrLastErrorDesc;
}

WCHAR_T* BTScaner::GetParameters()
{
	return m_pwstrParameters;
}

bool BTScaner::SetParameter(wchar_t* name, wchar_t* value)
{
	if (!name || !value)
		return false;

	if (wcscmp(name, L"MACAddress") == 0)
	{
		if (m_pwstrMACAddress) {
			delete m_pwstrMACAddress;
			m_pwstrMACAddress = 0;
		}

		convToShortWchar(&m_pwstrMACAddress, value);
	}
	else
	{
		return false;
	}

	return true;
}

void BTScaner::setCC(jclass _cc) {
	cc = _cc;
}

void BTScaner::setOBJ(jobject _obj) {
	obj = _obj;
}
