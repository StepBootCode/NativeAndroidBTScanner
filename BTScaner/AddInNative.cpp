#include "AddInNative.h"
#include "ConversionWchar.h"
#include "wchar.h"
#include <chrono>
#include <thread>
#include "../jni/jnienv.h"
#include "../include/IAndroidComponentHelper.h"

static const wchar_t *g_PropNames[] = 
{
	L"DeviceInfo"
};

static const wchar_t* g_PropNamesRu[] =
{
	L"ОписаниеУстройства",
};

static const wchar_t *g_MethodNames[] =
{
	L"GetBluetoothDevicesList",
	L"GetDescription",
	L"GetLastError",
	L"GetParameters",
	L"SetParameter",
	L"Enabled",
	L"InitDevice",
	L"StopDevice",
	L"Toast"
};

static const wchar_t *g_MethodNamesRu[] =
{
	L"СписокУстройств",
	L"ПолучитьОписание",
	L"ПолучитьОшибку",
	L"ПолучитьПараметры",
	L"УстановитьПараметр",
	L"Доступно",
	L"ИнициализироватьУстройство",
	L"ОстановитьУстройство",
	L"ВсплывающееСообщение"
};

static const wchar_t g_ComponentNameAddIn[] = L"BTScaner";
static WcharWrapper s_ComponentClass(g_ComponentNameAddIn);

const long g_VersionAddIn = 1000;  // This 1.0 version
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new AddInNative();
		return (long)*pInterface;
	}
	return 0;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	g_capabilities = capabilities;
	return eAppCapabilitiesLast;
}

long DestroyObject(IComponentBase** pInterface)
{
	if (!*pInterface)
		return -1;

	delete *pInterface;
	*pInterface = 0;
	return 0;
}

const WCHAR_T* GetClassNames()
{
	return s_ComponentClass;
}

AddInNative::AddInNative() : m_iConnect(nullptr), m_iMemory(nullptr)
{
}

AddInNative::~AddInNative()
{
}

// IInitDoneBase
bool AddInNative::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBaseEx*)pConnection;
	if (m_iConnect)
	{
		jvBTScaner.Initialize(m_iConnect);
	}
	return m_iConnect != nullptr;
}

bool AddInNative::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	if (m_iMemory)
	{
		return true;
	}
	return m_iMemory != nullptr;
}

long AddInNative::GetInfo()
{
	return g_VersionAddIn;
}

void AddInNative::Done()
{
	m_iConnect = nullptr;
	m_iMemory = nullptr;
}

// ILanguageExtenderBase
bool AddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	const wchar_t *wsExtension = g_ComponentNameAddIn;
	uint32_t iActualSize = static_cast<uint32_t>(::wcslen(wsExtension) + 1);

	if (m_iMemory)
	{
		if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
		{
			convToShortWchar(wsExtensionName, wsExtension, iActualSize);
			return true;
		}
	}

	return false;
}

long AddInNative::GetNProps()
{
	// You may delete next lines and add your own implementation code here
	return ePropLast;
}

long AddInNative::FindProp(const WCHAR_T* wsPropName)
{
	long plPropNum = -1;
	wchar_t* propName = 0;
	convFromShortWchar(&propName, wsPropName);

	plPropNum = findName(g_PropNames, propName, ePropLast);

	if (plPropNum == -1)
		plPropNum = findName(g_PropNamesRu, propName, ePropLast);

	delete[] propName;
	return plPropNum;
}


const WCHAR_T* AddInNative::GetPropName(long lPropNum, long lPropAlias)
{
	if (lPropNum >= ePropLast)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsPropName = NULL;

	switch (lPropAlias)
	{
	case 0: // First language (english)
		wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
		break;
	case 1: // Second language (local)
		wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
		break;
	default:
		return 0;
	}

	uint32_t iActualSize = static_cast<uint32_t>(wcslen(wsCurrentName) + 1);

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}


bool AddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	switch (lPropNum)
	{
	case ePropDevice:
	{

		IAndroidComponentHelper* helper = (IAndroidComponentHelper*)m_iConnect->GetInterface(eIAndroidComponentHelper);
		pvarPropVal->vt = VTYPE_EMPTY;
		if (helper)
		{
			WCHAR_T* className = nullptr;
			convToShortWchar(&className, L"android.os.Build");
			jclass ccloc = helper->FindClass(className);
			delete[] className;
			className = nullptr;
			std::wstring wData{};
			if (ccloc)
			{
				JNIEnv* env = getJniEnv();
				jfieldID fldID = env->GetStaticFieldID(ccloc, "MANUFACTURER", "Ljava/lang/String;");
				jstring	jManufacturer = (jstring)env->GetStaticObjectField(ccloc, fldID);
				std::wstring wManufacturer = ToWStringJni(env, jManufacturer);
				env->DeleteLocalRef(jManufacturer);
				fldID = env->GetStaticFieldID(ccloc, "MODEL", "Ljava/lang/String;");
				jstring	jModel = static_cast<jstring>(env->GetStaticObjectField(ccloc, fldID));
				std::wstring wModel = ToWStringJni(env, jModel);
				env->DeleteLocalRef(jModel);
				if (wManufacturer.length())
					wData = wManufacturer + L": " + wModel;
				else
					wData = wModel;
				env->DeleteLocalRef(ccloc);
			}
			if (wData.length())
				ToV8String(wData.c_str(), pvarPropVal);
		}
		return true;
	}
	default:
		return false;
	}
}


bool AddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{
	switch (lPropNum)
	{
	default:
		return false;
	}
}

bool AddInNative::IsPropReadable(const long lPropNum)
{
	return true;
}

bool AddInNative::IsPropWritable(const long lPropNum)
{
	switch (lPropNum)
	{
	default:
		return false;
	}
}

long AddInNative::GetNMethods()
{
	return eMethLast;
}

long AddInNative::FindMethod(const WCHAR_T* wsMethodName)
{
	long plMethodNum = -1;
	wchar_t* name = 0;
	convFromShortWchar(&name, wsMethodName);

	plMethodNum = findName(g_MethodNames, name, eMethLast);

	if (plMethodNum == -1)
		plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

	delete[] name;

	return plMethodNum;
}

const WCHAR_T* AddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{
	if (lMethodNum >= eMethLast)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsMethodName = NULL;

	switch (lMethodAlias)
	{
	case 0: // First language (english)
		wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
		break;
	case 1: // Second language (local)
		wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
		break;
	default:
		return 0;
	}

	uint32_t iActualSize = static_cast<uint32_t>(wcslen(wsCurrentName) + 1);

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}

long AddInNative::GetNParams(const long lMethodNum)
{
	switch (lMethodNum)
	{
	case eMethInitDevice:
	case eMethGetDescription:
	case eMethGetLastError:
	case eMethGetParameters:
	case eMethToast:
		return 1;
	case eMethSetParameter:
		return 2;
	default:
		return 0;
	}
}

bool AddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,	tVariant *pvarParamDefValue)
{
	switch (lMethodNum)
	{
		default:
			return false;
	}
}


bool AddInNative::HasRetVal(const long lMethodNum)
{
	switch (lMethodNum)
	{
		case eMethGetDescription:
		case eMethGetLastError:
		case eMethGetParameters:
		case eMethSetParameter:
		case eMethBluetoothDevicesList:
		case eMethEnabled:
			return true;

		default:
			return false;
	}
}

bool AddInNative::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
		case eMethInitDevice:
		{
			jvBTScaner.InitDevice(paParams, m_iMemory);
			return true;
		}
		case eMethStopDevice: {
			jvBTScaner.StopDevice();
			return true;
		}
		case eMethToast: {

			IAndroidComponentHelper* helper = (IAndroidComponentHelper*)m_iConnect->GetInterface(eIAndroidComponentHelper);
			if (helper)
			{
				WCHAR_T* className = nullptr;
				convToShortWchar(&className, L"ru.bootcode.btscaner.ShowToast");
				jclass ccloc = helper->FindClass(className);
				delete[] className;
				className = nullptr;
				if (ccloc)
				{
					JNIEnv* env = getJniEnv();
					jclass cc = static_cast<jclass>(env->NewGlobalRef(ccloc));
					env->DeleteLocalRef(ccloc);

					jobject activity = helper->GetActivity();
					jstring message = env->NewString(paParams[0].pwstrVal, paParams[0].wstrLen);

					jmethodID methID = env->GetMethodID(cc, "<init>", "(Landroid/app/Activity;Ljava/lang/String;)V");
					jobject objloc = env->NewObject(cc, methID, activity, message);  //cnn
					jobject obj = static_cast<jobject>(env->NewGlobalRef(objloc));
					
					env->DeleteLocalRef(objloc);
					methID = env->GetMethodID(cc, "toast", "()V");
					env->CallVoidMethod(obj, methID);
					env->DeleteLocalRef(activity);
				}
			}

			return true;
		}

		default:
			return false;
	}
}

bool AddInNative::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{

	switch (lMethodNum)
	{
		case eMethBluetoothDevicesList:
		{
			jstring res = jvBTScaner.GetBluetoothDevicesList(pvarRetValue, paParams, lSizeArray);

			JNIEnv* env = getJniEnv();

			TV_VT(pvarRetValue) = VTYPE_PWSTR;
			pvarRetValue->wstrLen = jstring2v8string(env, m_iMemory, res, &(pvarRetValue->pwstrVal)) / sizeof(uint16_t) - 1;		
		
			return true;

		}
		case eMethEnabled:
		{
			bool res = jvBTScaner.Enabled(pvarRetValue, paParams, lSizeArray);

			JNIEnv* env = getJniEnv();

			TV_VT(pvarRetValue) = VTYPE_BOOL;
			pvarRetValue->bVal = res;

			return true;

		}
		case eMethGetDescription:
			if (m_iMemory)
			{
				WCHAR_T* pwstrDesc = jvBTScaner.GetDescription();
				uint32_t  iActualSize = getLenShortWcharStr(pwstrDesc) + 1;
				tVariant& pParam0 = paParams[0];
				if (m_iMemory->AllocMemory((void**)&pParam0.pwstrVal, iActualSize * sizeof(WCHAR_T)))
				{
					memcpy(pParam0.pwstrVal, pwstrDesc, iActualSize * sizeof(WCHAR_T));
					pParam0.wstrLen = iActualSize - 1;
					TV_VT(&pParam0) = VTYPE_PWSTR;
				}
			}

			TV_VT(pvarRetValue) = VTYPE_BOOL;
			TV_BOOL(pvarRetValue) = true;
			return true;
		case eMethGetLastError:
			if (m_iMemory)
			{
				tVariant& pParam0 = paParams[0];

				WCHAR_T* pwstrLastError = jvBTScaner.GetLastErrorDesc();
				if (pwstrLastError)
				{
					uint32_t iActualSize = getLenShortWcharStr(pwstrLastError) + 1;
					if (m_iMemory->AllocMemory((void**)&pParam0.pwstrVal, iActualSize * sizeof(WCHAR_T)))
					{
						memcpy(pParam0.pwstrVal, pwstrLastError, iActualSize * sizeof(WCHAR_T));
						pParam0.wstrLen = iActualSize - 1;
						TV_VT(&pParam0) = VTYPE_PWSTR;
					}
				}
			}

			TV_VT(pvarRetValue) = VTYPE_I4;
			TV_I4(pvarRetValue) = jvBTScaner.GetLastErrorCode();
			return true;
		case eMethGetParameters:
		{
			IAndroidComponentHelper* helper = (IAndroidComponentHelper*)m_iConnect->GetInterface(eIAndroidComponentHelper);
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
					jclass cc = static_cast<jclass>(env->NewGlobalRef(ccloc));
					env->DeleteLocalRef(ccloc);

					jobject activity = helper->GetActivity();
					jmethodID methID = env->GetMethodID(cc, "<init>", "(Landroid/app/Activity;J)V");
					jobject objloc = env->NewObject(cc, methID, activity, (jlong)this);  //cnn
					jobject obj = static_cast<jobject>(env->NewGlobalRef(objloc));
					env->DeleteLocalRef(objloc);
					methID = env->GetMethodID(cc, "show", "()V");
					env->CallVoidMethod(obj, methID);
					
					methID = env->GetMethodID(cc, "getParameters", "()Ljava/lang/String;");
					jstring  res = (jstring)env->CallObjectMethod(obj, methID);
					env->DeleteLocalRef(activity);

					TV_VT(pvarRetValue) = VTYPE_PWSTR;
					pvarRetValue->wstrLen = jstring2v8string(env, m_iMemory, res, &(pvarRetValue->pwstrVal)) / sizeof(uint16_t) - 1;
					
					env->DeleteLocalRef(activity);
				}
			}
			TV_VT(pvarRetValue) = VTYPE_BOOL;
			TV_BOOL(pvarRetValue) = true;
			return true;
		}
		case eMethSetParameter:
		{
			wchar_t* pwstrParam0 = 0;
			wchar_t* pwstrParam1 = 0;

			tVariant& pParam0 = paParams[0];
			if (pParam0.wstrLen > 0)
				convFromShortWchar(&pwstrParam0, pParam0.pwstrVal);

			tVariant& pParam1 = paParams[1];
			if (pParam1.wstrLen > 0)
				convFromShortWchar(&pwstrParam1, pParam1.pwstrVal);

			jvBTScaner.SetParameter(pwstrParam0, pwstrParam1);

			if (pwstrParam0)
				delete pwstrParam0;
			if (pwstrParam1)
				delete pwstrParam1;

			TV_VT(pvarRetValue) = VTYPE_BOOL;
			TV_BOOL(pvarRetValue) = true;
			return true;
		}

		default:
			return false;
	}

	return false;
}

// ILocaleBase
void AddInNative::SetLocale(const WCHAR_T* loc)
{
}

// Other
void AddInNative::addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code)
{
	if (m_iConnect)
	{
		WCHAR_T *err = 0;
		WCHAR_T *descr = 0;

		convToShortWchar(&err, source);
		convToShortWchar(&descr, descriptor);

		m_iConnect->AddError(wcode, err, descr, code);

		delete[] descr;
		delete[] err;
	}
}

long AddInNative::findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const
{
	long ret = -1;
	for (uint32_t i = 0; i < size; i++)
	{
		if (!wcscmp(names[i], name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}

void AddInNative::ToV8String(const wchar_t* wstr, tVariant* par)
{
	if (wstr)
	{
		int len = wcslen(wstr);
		m_iMemory->AllocMemory((void**)&par->pwstrVal, (len + 1) * sizeof(WCHAR_T));
		convToShortWchar(&par->pwstrVal, wstr);
		par->vt = VTYPE_PWSTR;
		par->wstrLen = len;
	}
	else
		par->vt = VTYPE_EMPTY;
}

bool AddInNative::isNumericParameter(tVariant* par)
{
	return par->vt == VTYPE_I4 || par->vt == VTYPE_UI4 || par->vt == VTYPE_R8;
}

long AddInNative::numericValue(tVariant* par)
{
	long ret = 0;
	switch (par->vt)
	{
	case VTYPE_I4:
		ret = par->lVal;
		break;
	case VTYPE_UI4:
		ret = par->ulVal;
		break;
	case VTYPE_R8:
		ret = par->dblVal;
		break;
	}
	return ret;
}