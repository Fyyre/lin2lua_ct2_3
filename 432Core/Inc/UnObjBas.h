/*=============================================================================
	UnObjBas.h: Unreal object base class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Core enumerations.
-----------------------------------------------------------------------------*/

//
// Flags for loading objects.
//
enum ELoadFlags
{
	LOAD_None			= 0x0000,	// No flags.
	LOAD_NoFail			= 0x0001,	// Critical error if load fails.
	LOAD_NoWarn			= 0x0002,	// Don't display warning if load fails.
	LOAD_Throw			= 0x0008,	// Throw exceptions upon failure.
	LOAD_Verify			= 0x0010,	// Only verify existance; don't actually load.
	LOAD_AllowDll		= 0x0020,	// Allow plain DLLs.
	LOAD_DisallowFiles  = 0x0040,	// Don't load from file.
	LOAD_NoVerify       = 0x0080,   // Don't verify imports yet.
	LOAD_Forgiving      = 0x1000,   // Forgive missing imports (set them to NULL).
	LOAD_Quiet			= 0x2000,   // No log warnings.
	LOAD_NoRemap        = 0x4000,   // No remapping of packages.
	LOAD_Propagate      = 0,
};

//
// Package flags.
//
enum EPackageFlags
{
	PKG_AllowDownload	= 0x0001,	// Allow downloading package.
	PKG_ClientOptional  = 0x0002,	// Purely optional for clients.
	PKG_ServerSideOnly  = 0x0004,   // Only needed on the server side.
	PKG_BrokenLinks     = 0x0008,   // Loaded from linker with broken import links.
	PKG_Unsecure        = 0x0010,   // Not trusted.
	PKG_Need			= 0x8000,	// Client needs to download this package.
};

//
// Internal enums.
//
enum ENativeConstructor    {EC_NativeConstructor};
enum EStaticConstructor    {EC_StaticConstructor};
enum EInternal             {EC_Internal};
enum ECppProperty          {EC_CppProperty};
enum EInPlaceConstructor   {EC_InPlaceConstructor};

//
// Result of GotoState.
//
enum EGotoState
{
	GOTOSTATE_NotFound		= 0,
	GOTOSTATE_Success		= 1,
	GOTOSTATE_Preempted		= 2,
};

//
// Flags describing a class.
//
enum EClassFlags
{
	// Base flags.
	CLASS_Abstract          = 0x00001,  // Class is abstract and can't be instantiated directly.
	CLASS_Compiled			= 0x00002,  // Script has been compiled successfully.
	CLASS_Config			= 0x00004,  // Load object configuration at construction time.
	CLASS_Transient			= 0x00008,	// This object type can't be saved; null it out at save time.
	CLASS_Parsed            = 0x00010,	// Successfully parsed.
	CLASS_Localized         = 0x00020,  // Class contains localized text.
	CLASS_SafeReplace       = 0x00040,  // Objects of this class can be safely replaced with default or NULL.
	CLASS_RuntimeStatic     = 0x00080,	// Objects of this class are static during gameplay.
	CLASS_NoExport          = 0x00100,  // Don't export to C++ header.
	CLASS_NoUserCreate      = 0x00200,  // Don't allow users to create in the editor.
	CLASS_PerObjectConfig   = 0x00400,  // Handle object configuration on a per-object basis, rather than per-class.
	CLASS_NativeReplication = 0x00800,  // Replication handled in C++.

 	// from UTPT 
	CLASS_EditInlineNew 	 = 0x01000,
	CLASS_CollapseCategories = 0x02000,
	CLASS_ExportStructs	 = 0x04000,

	// Flags to inherit from base class.
	CLASS_Inherit           = CLASS_Transient | CLASS_Config | CLASS_Localized | CLASS_SafeReplace | CLASS_RuntimeStatic | CLASS_PerObjectConfig,
	CLASS_RecompilerClear   = CLASS_Inherit | CLASS_Abstract | CLASS_NoExport | CLASS_NativeReplication,
};

//
// Flags associated with each property in a class, overriding the
// property's default behavior.
//
enum EPropertyFlags
{
	// Regular flags.
	CPF_Edit		 = 0x00000001, // Property is user-settable in the editor.
	CPF_Const		 = 0x00000002, // Actor's property always matches class's default actor property.
	CPF_Input		 = 0x00000004, // Variable is writable by the input system.
	CPF_ExportObject = 0x00000008, // Object can be exported with actor.
	CPF_OptionalParm = 0x00000010, // Optional parameter (if CPF_Param is set).
	CPF_Net			 = 0x00000020, // Property is relevant to network replication.
	CPF_ConstRef     = 0x00000040, // Reference to a constant object.
	CPF_Parm		 = 0x00000080, // Function/When call parameter.
	CPF_OutParm		 = 0x00000100, // Value is copied out after function call.
	CPF_SkipParm	 = 0x00000200, // Property is a short-circuitable evaluation function parm.
	CPF_ReturnParm	 = 0x00000400, // Return value.
	CPF_CoerceParm	 = 0x00000800, // Coerce args into this function parameter.
	CPF_Native       = 0x00001000, // Property is native: C++ code is responsible for serializing it.
	CPF_Transient    = 0x00002000, // Property is transient: shouldn't be saved, zero-filled at load time.
	CPF_Config       = 0x00004000, // Property should be loaded/saved as permanent profile.
	CPF_Localized    = 0x00008000, // Property should be loaded as localizable text.
	CPF_Travel       = 0x00010000, // Property travels across levels/servers.
	CPF_EditConst    = 0x00020000, // Property is uneditable in the editor.
	CPF_GlobalConfig = 0x00040000, // Load config from base class, not subclass.
	CPF_OnDemand     = 0x00100000, // Object or dynamic array loaded on demand only.
	CPF_New			= 0x00200000, // Automatically create inner object.
	CPF_NeedCtorLink 	= 0x00400000, // Fields need construction/destruction.

	//from UTPT
	CPF_EditorData 	 	= 0x02000000,           // property has extra data to use in editor
	CPF_EditInline	 	= 0x04000000,
	CPF_EditInlineUse	= 0x14000000,
	CPF_Deprecated	 	= 0x20000000,

	// Combinations of flags.
	CPF_ParmFlags           = CPF_OptionalParm | CPF_Parm | CPF_OutParm | CPF_SkipParm | CPF_ReturnParm | CPF_CoerceParm,
	CPF_PropagateFromStruct = CPF_Const | CPF_Native | CPF_Transient,
};

//
// Flags describing an object instance.
//
enum EObjectFlags
{
	RF_Transactional    = 0x00000001,   // Object is transactional.
	RF_Unreachable		= 0x00000002,	// Object is not reachable on the object graph.
	RF_Public			= 0x00000004,	// Object is visible outside its package.
	RF_TagImp			= 0x00000008,	// Temporary import tag in load/save.
	RF_TagExp			= 0x00000010,	// Temporary export tag in load/save.
	RF_SourceModified   = 0x00000020,   // Modified relative to source files.
	RF_TagGarbage		= 0x00000040,	// Check during garbage collection.
	//
	//
	RF_NeedLoad			= 0x00000200,   // During load, indicates object needs loading.
	RF_HighlightedName  = 0x00000400,	// A hardcoded name which should be syntax-highlighted.
	RF_EliminateObject  = 0x00000400,   // NULL out references to this during garbage collecion.
	RF_InSingularFunc   = 0x00000800,	// In a singular function.
	RF_RemappedName     = 0x00000800,   // Name is remapped.
	RF_Suppress         = 0x00001000,	//warning: Mirrored in UnName.h. Suppressed log name.
	RF_StateChanged     = 0x00001000,   // Object did a state change.
	RF_InEndState       = 0x00002000,   // Within an EndState call.
	RF_Transient        = 0x00004000,	// Don't save object.
	RF_Preloading       = 0x00008000,   // Data is being preloaded from file.
	RF_LoadForClient	= 0x00010000,	// In-file load for client.
	RF_LoadForServer	= 0x00020000,	// In-file load for client.
	RF_LoadForEdit		= 0x00040000,	// In-file load for client.
	RF_Standalone       = 0x00080000,   // Keep object around for editing even if unreferenced.
	RF_NotForClient		= 0x00100000,	// Don't load this object for the game client.
	RF_NotForServer		= 0x00200000,	// Don't load this object for the game server.
	RF_NotForEdit		= 0x00400000,	// Don't load this object for the editor.
	RF_Destroyed        = 0x00800000,	// Object Destroy has already been called.
	RF_NeedPostLoad		= 0x01000000,   // Object needs to be postloaded.
	RF_HasStack         = 0x02000000,	// Has execution stack.
	RF_Native			= 0x04000000,   // Native (UClass only).
	RF_Marked			= 0x08000000,   // Marked (for debugging).
	RF_ErrorShutdown    = 0x10000000,	// ShutdownAfterError called.
	RF_DebugPostLoad    = 0x20000000,   // For debugging Serialize calls.
	RF_DebugSerialize   = 0x40000000,   // For debugging Serialize calls.
	RF_DebugDestroy     = 0x80000000,   // For debugging Destroy calls.
	RF_ContextFlags		= RF_NotForClient | RF_NotForServer | RF_NotForEdit, // All context flags.
	RF_LoadContextFlags	= RF_LoadForClient | RF_LoadForServer | RF_LoadForEdit, // Flags affecting loading.
	RF_Load  			= RF_ContextFlags | RF_LoadContextFlags | RF_Public | RF_Standalone | RF_Native | RF_SourceModified | RF_Transactional | RF_HasStack, // Flags to load from Unrealfiles.
	RF_Keep             = RF_Native | RF_Marked, // Flags to persist across loads.
	RF_ScriptMask		= RF_Transactional | RF_Public | RF_Transient | RF_NotForClient | RF_NotForServer | RF_NotForEdit // Script-accessible flags.
};

/*----------------------------------------------------------------------------
	Core types.
----------------------------------------------------------------------------*/

struct CORE_API FScriptDelegate
{
public:
   FScriptDelegate();
   struct FScriptDelegate & operator=(struct FScriptDelegate const &);
};

//
// Globally unique identifier.
//
class CORE_API FGuid
{
public:
	DWORD A,B,C,D;
	FGuid()
	{}
	FGuid( DWORD InA, DWORD InB, DWORD InC, DWORD InD )
	: A(InA), B(InB), C(InC), D(InD)
	{}
	friend UBOOL operator==(const FGuid& X, const FGuid& Y)
		{return X.A==Y.A && X.B==Y.B && X.C==Y.C && X.D==Y.D;}
	friend UBOOL operator!=(const FGuid& X, const FGuid& Y)
		{return X.A!=Y.A || X.B!=Y.B || X.C!=Y.C || X.D!=Y.D;}
	friend FArchive& operator<<( FArchive& Ar, FGuid& G )
	{
		guard(FGuid<<);
		return Ar << G.A << G.B << G.C << G.D;
		unguard;
	}
	TCHAR* String() const
	{
		TCHAR* Result = appStaticString1024();
		appSprintf( Result, TEXT("%08X%08X%08X%08X"), A, B, C, D );
		return Result;
	}
};
inline INT CompareGuids( FGuid* A, FGuid* B )
{
	INT Diff;
	Diff = A->A-B->A; if( Diff ) return Diff;
	Diff = A->B-B->B; if( Diff ) return Diff;
	Diff = A->C-B->C; if( Diff ) return Diff;
	Diff = A->D-B->D; if( Diff ) return Diff;
	return 0;
}

//
// COM IUnknown interface.
//
class CORE_API FUnknown
{
public:
	virtual DWORD STDCALL QueryInterface( const FGuid& RefIID, void** InterfacePtr ) {return 0;}
	virtual DWORD STDCALL AddRef() {return 0;}
	virtual DWORD STDCALL Release() {return 0;}
};

//
// Information about a driver class.
//
class CORE_API FRegistryObjectInfo
{
public:
	FString Object;
	FString Class;
	FString MetaClass;
	FString Description;
	FString Autodetect;
	FRegistryObjectInfo()
	: Object(), Class(), MetaClass(), Description(), Autodetect()
	{}
};

//
// Information about a preferences menu item.
//
class CORE_API FPreferencesInfo
{
public:
	FString Caption;
	FString ParentCaption;
	FString Class;
	FName Category;
	UBOOL Immediate;
	FPreferencesInfo()
	: Caption(), ParentCaption(), Class(), Category(NAME_None), Immediate(0)
	{}
};

/*----------------------------------------------------------------------------
	Core macros.
----------------------------------------------------------------------------*/

// Special canonical package for FindObject, ParseObject.
#define ANY_PACKAGE ((UPackage*)-1)

// Define private default constructor.
#define NO_DEFAULT_CONSTRUCTOR(cls) \
	protected: cls() {} public:

// Guard macros.
#define unguardobjSlow		unguardfSlow(( TEXT("(%s)"), GetFullName() ))
#define unguardobj			unguardf(( TEXT("(%s)"), GetFullName() ))

// Verify the a class definition and C++ definition match up.
#define VERIFY_CLASS_OFFSET(Pre,ClassName,Member) \
	{for( TFieldIterator<UProperty> It( FindObjectChecked<UClass>( Pre##ClassName::StaticClass()->GetOuter(), TEXT(#ClassName) ) ); It; ++It ) \
		if( appStricmp(It->GetName(),TEXT(#Member))==0 ) \
			if( It->Offset != STRUCT_OFFSET(Pre##ClassName,Member) ) \
				appErrorf(TEXT("Class %s Member %s problem: Script=%i C++=%i"), TEXT(#ClassName), TEXT(#Member), It->Offset, STRUCT_OFFSET(Pre##ClassName,Member) );}

// Verify that C++ and script code agree on the size of a class.
#define VERIFY_CLASS_SIZE(ClassName) \
	check(sizeof(ClassName)==ClassName::StaticClass()->GetPropertiesSize());

// Declare the base UObject class.

#if !__STATIC_LINK
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
public: \
	/* Identification */ \
	enum {StaticClassFlags=TStaticFlags}; \
	private: static UClass PrivateStaticClass; public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
	static UClass* StaticClass() \
		{ return &PrivateStaticClass; } \
	void* operator new( size_t Size, UObject* Outer=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0 ) \
		{ return StaticAllocateObject( StaticClass(), Outer, Name, SetFlags); } \
	void* operator new( size_t Size, EInternal* Mem ) \
		{ return (void*)Mem; }
#else
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
public: \
	/* Identification */ \
	enum {StaticClassFlags=TStaticFlags}; \
	private: \
	static UClass* PrivateStaticClass; \
	public: \
	typedef TSuperClass Super;\
	typedef TClass ThisClass;\
	static UClass* GetPrivateStaticClass##TClass( TCHAR* Package ); \
	static void InitializePrivateStaticClass##TClass(); \
	static UClass* StaticClass() \
	{ \
		if (!PrivateStaticClass) \
		{ \
			PrivateStaticClass = GetPrivateStaticClass##TClass( TEXT(#TPackage) ); \
			InitializePrivateStaticClass##TClass(); \
		} \
		return PrivateStaticClass; \
	} \
	void* operator new( size_t Size, UObject* Outer=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0 ) \
		{ return StaticAllocateObject( StaticClass(), Outer, Name, SetFlags); } \
	void* operator new( size_t Size, EInternal* Mem ) \
		{ return (void*)Mem; }
#endif

// Declare a concrete class.
#define DECLARE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
	DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
	friend FArchive &operator<<( FArchive& Ar, TClass*& Res ) \
		{ return Ar << *(UObject**)&Res; } \
	virtual ~TClass() \
		{ ConditionalDestroy(); } \
	static void InternalConstructor( void* X ) \
		{ new( (EInternal*)X )TClass(); } \

// Declare an abstract class.
#define DECLARE_ABSTRACT_CLASS( TClass, TSuperClass, TStaticFlags, TPackage ) \
	DECLARE_BASE_CLASS( TClass, TSuperClass, TStaticFlags | CLASS_Abstract, TPackage ) \
	friend FArchive &operator<<( FArchive& Ar, TClass*& Res ) \
		{ return Ar << *(UObject**)&Res; } \
	virtual ~TClass() \
		{ ConditionalDestroy(); } \

// Declare that objects of class being defined reside within objects of the specified class.
#define DECLARE_WITHIN( TWithinClass ) \
	typedef TWithinClass WithinClass; \
	TWithinClass* GetOuter##TWithinClass() const { return (TWithinClass*)GetOuter(); }

// Register a class at startup time.
#if _MSC_VER
	#define IMPLEMENT_CLASS(TClass) \
		UClass TClass::PrivateStaticClass \
		( \
			EC_NativeConstructor, \
			sizeof(TClass), \
			TClass::StaticClassFlags, \
			TClass::Super::StaticClass(), \
			TClass::WithinClass::StaticClass(), \
			FGuid(TClass::GUID1,TClass::GUID2,TClass::GUID3,TClass::GUID4), \
			TEXT(#TClass)+1, \
			GPackage, \
			StaticConfigName(), \
			RF_Public | RF_Standalone | RF_Transient | RF_Native, \
			(void(*)(void*))TClass::InternalConstructor, \
			(void(UObject::*)())TClass::StaticConstructor \
		); \
		extern "C" DLL_EXPORT UClass* autoclass##TClass;\
		DLL_EXPORT UClass* autoclass##TClass = TClass::StaticClass();
#elif !__STATIC_LINK
	#define IMPLEMENT_CLASS(TClass) \
		UClass TClass::PrivateStaticClass \
		( \
			EC_NativeConstructor, \
			sizeof(TClass), \
			TClass::StaticClassFlags, \
			TClass::Super::StaticClass(), \
			TClass::WithinClass::StaticClass(), \
			FGuid(TClass::GUID1,TClass::GUID2,TClass::GUID3,TClass::GUID4), \
			TEXT(#TClass)+1, \
			GPackage, \
			StaticConfigName(), \
			RF_Public | RF_Standalone | RF_Transient | RF_Native, \
			(void(*)(void*))TClass::InternalConstructor, \
			(void(UObject::*)())&TClass::StaticConstructor \
		); \
		DLL_EXPORT {UClass* autoclass##TClass = TClass::StaticClass();}
#else
	#define IMPLEMENT_CLASS(TClass) \
		UClass* TClass::PrivateStaticClass = NULL; \
		UClass* TClass::GetPrivateStaticClass##TClass( TCHAR* Package ) \
		{ \
			UClass* ReturnClass; \
			ReturnClass = ::new UClass \
			( \
				EC_StaticConstructor, \
				sizeof(TClass), \
				StaticClassFlags, \
				FGuid(GUID1,GUID2,GUID3,GUID4), \
				TEXT(#TClass)+1, \
				Package, \
				StaticConfigName(), \
				RF_Public | RF_Standalone | RF_Transient | RF_Native, \
				(void(*)(void*))TClass::InternalConstructor, \
				(void(UObject::*)())&TClass::StaticConstructor \
			); \
			check(ReturnClass); \
			return ReturnClass; \
		} \
		/* Called from ::StaticClass after GetPrivateStaticClass */ \
		void TClass::InitializePrivateStaticClass##TClass() \
		{ \
			/* No recursive ::StaticClass calls allowed. Setup extras. */ \
			if (TClass::Super::StaticClass() != TClass::PrivateStaticClass) \
				TClass::PrivateStaticClass->SuperField = TClass::Super::StaticClass(); \
			else \
				TClass::PrivateStaticClass->SuperField = NULL; \
			TClass::PrivateStaticClass->ClassWithin = TClass::WithinClass::StaticClass(); \
			TClass::PrivateStaticClass->SetClass(UClass::StaticClass()); \
			/* Perform UObject native registration. */ \
			if( TClass::PrivateStaticClass->GetInitialized() && TClass::PrivateStaticClass->GetClass()==TClass::PrivateStaticClass->StaticClass() ) \
				TClass::PrivateStaticClass->Register(); \
		}
#endif

// Define the package of the current DLL being compiled.
#if _MSC_VER
	#define IMPLEMENT_PACKAGE(pkg) \
		extern "C" DLL_EXPORT TCHAR GPackage[]; \
		DLL_EXPORT TCHAR GPackage[] = TEXT(#pkg); \
		IMPLEMENT_PACKAGE_PLATFORM(pkg)
#else
	#define IMPLEMENT_PACKAGE(pkg) \
	extern "C" DLL_EXPORT {TCHAR GPackage[] = TEXT(#pkg);} \
		IMPLEMENT_PACKAGE_PLATFORM(pkg)
#endif

/*-----------------------------------------------------------------------------
	UObject.
-----------------------------------------------------------------------------*/

//
// The base class of all objects.
//
class CORE_API UObject : public FUnknown
{
	typedef UObject WithinClass;
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};
	// Friends.
	friend class FObjectIterator;
	friend class ULinkerLoad;
	friend class ULinkerSave;
	friend class UPackageMap;
	friend class FArchiveTagUsed;
	friend struct FObjectImport;
	friend struct FObjectExport;

public:
	INT			Index;				// Index of object into table.
	UObject*		HashNext;			// Next object in this hash bin.
	FStateFrame*		StateFrame;			// Main script execution stack.
	ULinkerLoad*		_Linker;			// Linker it came from, or NULL if none.
	INT			_LinkerIndex;		// Index of this object in the linker's export map.
	class UObject* Outer;                                                     //CPF_Native
	INT ObjectFlags;                                                          //CPF_Native
	FName* Name;                                                              //CPF_Edit|CPF_Native|CPF_EditConst
	class UClass* Class;                                                      //CPF_Native|CPF_EditConst

	INT        CacheIndex;				//0x0028 LineageII
	INT        HashNextBuffer;			//0x002C LineageII
	INT        IndexBuffer;				//0x0030 LineageII

	virtual DWORD STDCALL QueryInterface(class FGuid const &, void * *);
	virtual DWORD STDCALL AddRef();
	virtual DWORD STDCALL Release();
	virtual ~UObject();
	virtual void ProcessEvent(class UFunction *, void *, void *);
	virtual void ProcessDelegate(class FName, struct FScriptDelegate *, void *, void *);
	virtual void ProcessState(FLOAT);
	virtual INT ProcessRemoteFunction(class UFunction *, void *, struct FFrame *);
	virtual void Modify();
	virtual void PostLoad();
	virtual void Destroy();
	virtual void Serialize(class FArchive &);
	virtual INT IsPendingKill();
	virtual enum EGotoState GotoState(class FName);
	virtual INT GotoLabel(class FName);
	virtual void ScriptInit(class ALevelInfo *);
	virtual void InitExecution();
	virtual void ShutdownAfterError();
	virtual void PostEditChange();
	virtual void PreEditUndo();
	virtual void PostEditUndo();
	virtual void CallFunction(struct FFrame &, void * const, class UFunction *);
	virtual INT ScriptConsoleExec(TCHAR const *, class FOutputDevice &, class UObject *);
	virtual void Register();
	virtual void LanguageChange();
	virtual void Rename(TCHAR const *, class UObject *);
	virtual void NetDirty(class UProperty *);
	virtual void ExecutingBadStateCode(struct FFrame &);
	void AddToRoot();
	static INT CDECL AttemptDelete(class UObject * &, DWORD, INT);
	static void CDECL BeginLoad();
	static void CDECL BindPackage(class UPackage *);
	static void CDECL CheckDanglingOuter(class UObject *);
	static void CDECL CheckDanglingRefs(class UObject *);
	void ClearFlags(DWORD);
	static void CDECL CollectGarbage(DWORD);
	INT ConditionalDestroy();
	void ConditionalPostLoad();
	void ConditionalRegister();
	void ConditionalShutdownAfterError();
	static class UPackage * CDECL CreatePackage(class UObject *, TCHAR const *);
	static void CDECL EndLoad();
	static void CDECL ExitProperties(BYTE *, class UClass *);
	static void CDECL ExportProperties(class FOutputDevice &, class UClass *, BYTE *, INT, class UClass *, BYTE *);
	INT FindArrayProperty(class FString, class FArray * *, INT *);
	INT FindBoolProperty(class FString, INT *);
	INT FindFNameProperty(class FString, class FName *);
	INT FindFloatProperty(class FString, FLOAT *);
	class UFunction * FindFunction(class FName, INT);
	class UFunction * FindFunctionChecked(class FName, INT);
	INT FindIntProperty(class FString, INT *);
	class UField * FindObjectField(class FName, INT);
	INT FindObjectProperty(class FString, class UObject * *);
	class UState * FindState(class FName);
	INT FindStructProperty(class FString, class UStruct * *);
	class UClass * GetClass() const;
	class FName const GetFName() const;
	DWORD GetFlags() const;
	TCHAR const * GetFullName( TCHAR* Str=NULL ) const;
	DWORD GetIndex() const;
	static class UObject * CDECL GetIndexedObject(INT);
	static INT CDECL GetInitialized();
	static TCHAR const * CDECL GetLanguage();
	class ULinkerLoad * GetLinker();
	INT GetLinkerIndex();
	static class TArray<class UObject *> CDECL GetLoaderList();
	TCHAR const * GetName() const;
	static INT CDECL GetObjectHash(class FName, INT);
	class UObject * GetOuter() const;
	static class ULinkerLoad * CDECL GetPackageLinker(class UObject *, TCHAR const *, DWORD, class UPackageMap *, class FGuid *);
	TCHAR const * GetPathName(class UObject *, TCHAR *) const;
	static void CDECL GetPreferences(class TArray<class FPreferencesInfo> &, TCHAR const *, INT);
	static void CDECL GetRegistryObjects(class TArray<class FRegistryObjectInfo> &, class UClass *, class UClass *, INT);
	struct FStateFrame * GetStateFrame();
	static class UPackage * CDECL GetTransientPackage();
	static void CDECL GlobalSetProperty(TCHAR const *, class UClass *, class UProperty *, INT, INT);
	void InitClassDefaultObject(class UClass *, INT);
	static void CDECL InitProperties(BYTE *, INT, class UClass *, BYTE *, INT, class UObject *, class UObject *);
	static void CDECL InternalConstructor(void *);
	INT IsA(class UClass *) const;
	INT IsIn(class UObject *) const;
	INT IsInState(class FName);
	INT IsProbing(class FName);
	static INT CDECL IsReferenced(class UObject * &, DWORD, INT);
	INT IsValid();
	void LoadConfig(INT, class UClass *, TCHAR const *);
	void LoadLocalized();
	static class UObject * CDECL LoadPackage(class UObject *, TCHAR const *, DWORD);
	void ParseParms(TCHAR const *);
	void ProcessInternal(struct FFrame &, void * const);
	static void CDECL ProcessRegistrants();
	void RemoveFromRoot();
	static void CDECL ResetConfig(class UClass *);
	static void CDECL ResetLoaders(class UObject *, INT, INT);
	void SaveConfig(DWORD, TCHAR const *);
	static INT CDECL SavePackage(class UObject *, class UObject *, DWORD, TCHAR const *, class FOutputDevice *, class ULinkerLoad *);
	static void CDECL SerializeRootSet(class FArchive &, DWORD, DWORD);
	void SetClass(class UClass *);
	void SetFlags(DWORD);
	static void CDECL SetLanguage(TCHAR const *);
	//static class UObject * CDECL StaticAllocateObject(class UClass *, class UObject *, class FName, DWORD, class UObject *, class FOutputDevice *, class UObject *, class UObject *);
	static       UObject *       StaticAllocateObject(      UClass * Class, UObject * InOuter=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0, UObject * Template=NULL, FOutputDevice* Error=GError, UObject * Ptr=NULL );
	static class UClass * CDECL StaticClass();
	static TCHAR const * CDECL StaticConfigName();
	static class UObject * CDECL StaticConstructObject(class UClass *, class UObject *, class FName, DWORD, class UObject *, class FOutputDevice *, class UObject *);
	void StaticConstructor();
	static INT CDECL StaticExec(TCHAR const *, class FOutputDevice &);
	static void CDECL StaticExit();
	static class UObject * CDECL StaticFindObject(class UClass *, class UObject *, TCHAR const *, INT);
	static class UObject * CDECL StaticFindObjectChecked(class UClass *, class UObject *, TCHAR const *, INT);
	static void CDECL StaticInit();
	static class UClass * CDECL StaticLoadClass(class UClass *, class UObject *, TCHAR const *, TCHAR const *, DWORD, class UPackageMap *);
	static class UObject * CDECL StaticLoadObject(class UClass *, class UObject *, TCHAR const *, TCHAR const *, DWORD, class UPackageMap *);
	static void CDECL StaticShutdownAfterError();
	static void CDECL StaticTick();
	UObject(class UObject const &);
	UObject(enum EInPlaceConstructor, class UClass *, class UObject *, class FName, DWORD);
	UObject(enum ENativeConstructor, class UClass *, TCHAR const *, TCHAR const *, DWORD);
	UObject(enum EStaticConstructor, TCHAR const *, TCHAR const *, DWORD);
	UObject();
	static void CDECL VerifyLinker(class ULinkerLoad *);
	void eventBeginState();
	void eventEndState();
	void execAbs(struct FFrame &, void * const);
	void execAcos(struct FFrame &, void * const);
	void execAddAdd_Byte(struct FFrame &, void * const);
	void execAddAdd_Int(struct FFrame &, void * const);
	void execAddAdd_PreByte(struct FFrame &, void * const);
	void execAddAdd_PreInt(struct FFrame &, void * const);
	void execAddEqual_ByteByte(struct FFrame &, void * const);
	void execAddEqual_FloatFloat(struct FFrame &, void * const);
	void execAddEqual_IntInt(struct FFrame &, void * const);
	void execAddEqual_RotatorRotator(struct FFrame &, void * const);
	void execAddEqual_VectorVector(struct FFrame &, void * const);
	void execAdd_FloatFloat(struct FFrame &, void * const);
	void execAdd_IntInt(struct FFrame &, void * const);
	void execAdd_RotatorRotator(struct FFrame &, void * const);
	void execAdd_VectorVector(struct FFrame &, void * const);
	void execAndAnd_BoolBool(struct FFrame &, void * const);
	void execAnd_IntInt(struct FFrame &, void * const);
	void execArrayElement(struct FFrame &, void * const);
	void execAsc(struct FFrame &, void * const);
	void execAsin(struct FFrame &, void * const);
	void execAssert(struct FFrame &, void * const);
	void execAt_StringString(struct FFrame &, void * const);
	void execAtan(struct FFrame &, void * const);
	void execBoolToByte(struct FFrame &, void * const);
	void execBoolToFloat(struct FFrame &, void * const);
	void execBoolToInt(struct FFrame &, void * const);
	void execBoolToString(struct FFrame &, void * const);
	void execBoolVariable(struct FFrame &, void * const);
	void execByteConst(struct FFrame &, void * const);
	void execByteToBool(struct FFrame &, void * const);
	void execByteToFloat(struct FFrame &, void * const);
	void execByteToInt(struct FFrame &, void * const);
	void execByteToString(struct FFrame &, void * const);
	void execCalcDirection(struct FFrame &, void * const);
	void execCalcRotation(struct FFrame &, void * const);
	void execCaps(struct FFrame &, void * const);
	void execCase(struct FFrame &, void * const);
	void execCeil(struct FFrame &, void * const);
	void execChr(struct FFrame &, void * const);
	void execClamp(struct FFrame &, void * const);
	void execClassContext(struct FFrame &, void * const);
	void execClassIsChildOf(struct FFrame &, void * const);
	void execClockwiseFrom_IntInt(struct FFrame &, void * const);
	void execComplementEqual_FloatFloat(struct FFrame &, void * const);
	void execComplementEqual_StringString(struct FFrame &, void * const);
	void execComplement_PreInt(struct FFrame &, void * const);
	void execCompress(struct FFrame &, void * const);
	void execConcat_StringString(struct FFrame &, void * const);
	void execContext(struct FFrame &, void * const);
	void execCos(struct FFrame &, void * const);
	void execCross_VectorVector(struct FFrame &, void * const);
	void execDebugInfo(struct FFrame &, void * const);
	void execDefaultVariable(struct FFrame &, void * const);
	void execDelegateFunction(struct FFrame &, void * const);
	void execDelegateProperty(struct FFrame &, void * const);
	void execDisable(struct FFrame &, void * const);
	void execDivideEqual_ByteByte(struct FFrame &, void * const);
	void execDivideEqual_FloatFloat(struct FFrame &, void * const);
	void execDivideEqual_IntFloat(struct FFrame &, void * const);
	void execDivideEqual_RotatorFloat(struct FFrame &, void * const);
	void execDivideEqual_VectorFloat(struct FFrame &, void * const);
	void execDivide_FloatFloat(struct FFrame &, void * const);
	void execDivide_IntInt(struct FFrame &, void * const);
	void execDivide_RotatorFloat(struct FFrame &, void * const);
	void execDivide_VectorFloat(struct FFrame &, void * const);
	void execDot_VectorVector(struct FFrame &, void * const);
	void execDynArrayElement(struct FFrame &, void * const);
	void execDynArrayInsert(struct FFrame &, void * const);
	void execDynArrayLength(struct FFrame &, void * const);
	void execDynArrayRemove(struct FFrame &, void * const);
	void execDynamicCast(struct FFrame &, void * const);
	void execDynamicLoadObject(struct FFrame &, void * const);
	void execEatString(struct FFrame &, void * const);
	void execEnable(struct FFrame &, void * const);
	void execEndFunctionParms(struct FFrame &, void * const);
	void execEqualEqual_BoolBool(struct FFrame &, void * const);
	void execEqualEqual_FloatFloat(struct FFrame &, void * const);
	void execEqualEqual_IntInt(struct FFrame &, void * const);
	void execEqualEqual_NameName(struct FFrame &, void * const);
	void execEqualEqual_ObjectObject(struct FFrame &, void * const);
	void execEqualEqual_RotatorRotator(struct FFrame &, void * const);
	void execEqualEqual_StringString(struct FFrame &, void * const);
	void execEqualEqual_VectorVector(struct FFrame &, void * const);
	void execExp(struct FFrame &, void * const);
	void execExpand(struct FFrame &, void * const);
	void execFClamp(struct FFrame &, void * const);
	void execFClose(struct FFrame &, void * const);
	void execFLoad(struct FFrame &, void * const);
	void execFMax(struct FFrame &, void * const);
	void execFMin(struct FFrame &, void * const);
	void execFOpen(struct FFrame &, void * const);
	void execFOpenWrite(struct FFrame &, void * const);
	void execFRand(struct FFrame &, void * const);
	void execFReadLine(struct FFrame &, void * const);
	void execFUnload(struct FFrame &, void * const);
	void execFWrite(struct FFrame &, void * const);
	void execFWriteLine(struct FFrame &, void * const);
	void execFalse(struct FFrame &, void * const);
	void execFinalFunction(struct FFrame &, void * const);
	void execFindObject(struct FFrame &, void * const);
	void execFloatConst(struct FFrame &, void * const);
	void execFloatToBool(struct FFrame &, void * const);
	void execFloatToByte(struct FFrame &, void * const);
	void execFloatToInt(struct FFrame &, void * const);
	void execFloatToString(struct FFrame &, void * const);
	void execGetAxes(struct FFrame &, void * const);
	void execGetBaseDir(struct FFrame &, void * const);
	void execGetEnum(struct FFrame &, void * const);
	void execGetInputKeyString(struct FFrame &, void * const);
	void execGetLanguageFilter(struct FFrame &, void * const);
	void execGetMilesOnly(struct FFrame &, void * const);
	void execGetNoBlood(struct FFrame &, void * const);
	void execGetNoSniper(struct FFrame &, void * const);
	void execGetPlatform(struct FFrame &, void * const);
	void execGetPrivateProfileInt(struct FFrame &, void * const);
	void execGetPrivateProfileString(struct FFrame &, void * const);
	void execGetPropertyText(struct FFrame &, void * const);
	void execGetStateName(struct FFrame &, void * const);
	void execGetUnAxes(struct FFrame &, void * const);
	void execGetVersionAGPMajor(struct FFrame &, void * const);
	void execGetVersionAGPMinor(struct FFrame &, void * const);
	void execGetVersionAGPTiny(struct FFrame &, void * const);
	void execGetVersionWarfareEngine(struct FFrame &, void * const);
	void execGlobalFunction(struct FFrame &, void * const);
	void execGotoLabel(struct FFrame &, void * const);
	void execGotoState(struct FFrame &, void * const);
	void execGreaterEqual_FloatFloat(struct FFrame &, void * const);
	void execGreaterEqual_IntInt(struct FFrame &, void * const);
	void execGreaterEqual_StringString(struct FFrame &, void * const);
	void execGreaterGreaterGreater_IntInt(struct FFrame &, void * const);
	void execGreaterGreater_IntInt(struct FFrame &, void * const);
	void execGreaterGreater_VectorRotator(struct FFrame &, void * const);
	void execGreater_FloatFloat(struct FFrame &, void * const);
	void execGreater_IntInt(struct FFrame &, void * const);
	void execGreater_StringString(struct FFrame &, void * const);
	void execHighNative0(struct FFrame &, void * const);
	void execHighNative1(struct FFrame &, void * const);
	void execHighNative10(struct FFrame &, void * const);
	void execHighNative11(struct FFrame &, void * const);
	void execHighNative12(struct FFrame &, void * const);
	void execHighNative13(struct FFrame &, void * const);
	void execHighNative14(struct FFrame &, void * const);
	void execHighNative15(struct FFrame &, void * const);
	void execHighNative2(struct FFrame &, void * const);
	void execHighNative3(struct FFrame &, void * const);
	void execHighNative4(struct FFrame &, void * const);
	void execHighNative5(struct FFrame &, void * const);
	void execHighNative6(struct FFrame &, void * const);
	void execHighNative7(struct FFrame &, void * const);
	void execHighNative8(struct FFrame &, void * const);
	void execHighNative9(struct FFrame &, void * const);
	void execInStr(struct FFrame &, void * const);
	void execInitRotRand(struct FFrame &, void * const);
	void execInstanceVariable(struct FFrame &, void * const);
	void execIntConst(struct FFrame &, void * const);
	void execIntConstByte(struct FFrame &, void * const);
	void execIntOne(struct FFrame &, void * const);
	void execIntToBool(struct FFrame &, void * const);
	void execIntToByte(struct FFrame &, void * const);
	void execIntToFloat(struct FFrame &, void * const);
	void execIntToString(struct FFrame &, void * const);
	void execIntZero(struct FFrame &, void * const);
	void execInterpCurveEval(struct FFrame &, void * const);
	void execInterpCurveGetInputDomain(struct FFrame &, void * const);
	void execInterpCurveGetOutputRange(struct FFrame &, void * const);
	void execInvert(struct FFrame &, void * const);
	void execIsA(struct FFrame &, void * const);
	void execIsDebugBuild(struct FFrame &, void * const);
	void execIsInState(struct FFrame &, void * const);
	void execIterator(struct FFrame &, void * const);
	void execJump(struct FFrame &, void * const);
	void execJumpIfNot(struct FFrame &, void * const);
	void execLeft(struct FFrame &, void * const);
	void execLen(struct FFrame &, void * const);
	void execLerp(struct FFrame &, void * const);
	void execLessEqual_FloatFloat(struct FFrame &, void * const);
	void execLessEqual_IntInt(struct FFrame &, void * const);
	void execLessEqual_StringString(struct FFrame &, void * const);
	void execLessLess_IntInt(struct FFrame &, void * const);
	void execLessLess_VectorRotator(struct FFrame &, void * const);
	void execLess_FloatFloat(struct FFrame &, void * const);
	void execLess_IntInt(struct FFrame &, void * const);
	void execLess_StringString(struct FFrame &, void * const);
	void execLet(struct FFrame &, void * const);
	void execLetBool(struct FFrame &, void * const);
	void execLetDelegate(struct FFrame &, void * const);
	void execLocalVariable(struct FFrame &, void * const);
	void execLocalize(struct FFrame &, void * const);
	void execLocs(struct FFrame &, void * const);
	void execLog(struct FFrame &, void * const);
	void execLogFileClose(struct FFrame &, void * const);
	void execLogFileOpen(struct FFrame &, void * const);
	void execLogFileWrite(struct FFrame &, void * const);
	void execLoge(struct FFrame &, void * const);
	void execMax(struct FFrame &, void * const);
	void execMetaCast(struct FFrame &, void * const);
	void execMid(struct FFrame &, void * const);
	void execMin(struct FFrame &, void * const);
	void execMirrorVectorByNormal(struct FFrame &, void * const);
	void execMultiplyEqual_ByteByte(struct FFrame &, void * const);
	void execMultiplyEqual_FloatFloat(struct FFrame &, void * const);
	void execMultiplyEqual_IntFloat(struct FFrame &, void * const);
	void execMultiplyEqual_RotatorFloat(struct FFrame &, void * const);
	void execMultiplyEqual_VectorFloat(struct FFrame &, void * const);
	void execMultiplyEqual_VectorVector(struct FFrame &, void * const);
	void execMultiplyMultiply_FloatFloat(struct FFrame &, void * const);
	void execMultiply_FloatFloat(struct FFrame &, void * const);
	void execMultiply_FloatRotator(struct FFrame &, void * const);
	void execMultiply_FloatVector(struct FFrame &, void * const);
	void execMultiply_IntInt(struct FFrame &, void * const);
	void execMultiply_RotatorFloat(struct FFrame &, void * const);
	void execMultiply_VectorFloat(struct FFrame &, void * const);
	void execMultiply_VectorVector(struct FFrame &, void * const);
	void execNameConst(struct FFrame &, void * const);
	void execNameToBool(struct FFrame &, void * const);
	void execNameToString(struct FFrame &, void * const);
	void execNativeParm(struct FFrame &, void * const);
	void execNew(struct FFrame &, void * const);
	void execNoObject(struct FFrame &, void * const);
	void execNormal(struct FFrame &, void * const);
	void execNormalize(struct FFrame &, void * const);
	void execNotEqual_BoolBool(struct FFrame &, void * const);
	void execNotEqual_FloatFloat(struct FFrame &, void * const);
	void execNotEqual_IntInt(struct FFrame &, void * const);
	void execNotEqual_NameName(struct FFrame &, void * const);
	void execNotEqual_ObjectObject(struct FFrame &, void * const);
	void execNotEqual_RotatorRotator(struct FFrame &, void * const);
	void execNotEqual_StringString(struct FFrame &, void * const);
	void execNotEqual_VectorVector(struct FFrame &, void * const);
	void execNot_PreBool(struct FFrame &, void * const);
	void execNothing(struct FFrame &, void * const);
	void execObjectConst(struct FFrame &, void * const);
	void execObjectToBool(struct FFrame &, void * const);
	void execObjectToString(struct FFrame &, void * const);
	void execOrOr_BoolBool(struct FFrame &, void * const);
	void execOr_IntInt(struct FFrame &, void * const);
	void execOrthoRotation(struct FFrame &, void * const);
	void execPercent_FloatFloat(struct FFrame &, void * const);
	void execPrimitiveCast(struct FFrame &, void * const);
	void execPrivateSet(struct FFrame &, void * const);
	void execQuatFindBetween(struct FFrame &, void * const);
	void execQuatFromAxisAndAngle(struct FFrame &, void * const);
	void execQuatInvert(struct FFrame &, void * const);
	void execQuatProduct(struct FFrame &, void * const);
	void execQuatRotateVector(struct FFrame &, void * const);
	void execRand(struct FFrame &, void * const);
	void execResetConfig(struct FFrame &, void * const);
	void execRight(struct FFrame &, void * const);
	void execRotRand(struct FFrame &, void * const);
	void execRotationConst(struct FFrame &, void * const);
	void execRotatorToBool(struct FFrame &, void * const);
	void execRotatorToString(struct FFrame &, void * const);
	void execRotatorToVector(struct FFrame &, void * const);
	void execRound(struct FFrame &, void * const);
	void execSaveConfig(struct FFrame &, void * const);
	void execSavePrivateProfile(struct FFrame &, void * const);
	void execSelf(struct FFrame &, void * const);
	void execSetLanguageFilter(struct FFrame &, void * const);
	void execSetMilesOnly(struct FFrame &, void * const);
	void execSetNoBlood(struct FFrame &, void * const);
	void execSetNoSniper(struct FFrame &, void * const);
	void execSetPrivateProfileInt(struct FFrame &, void * const);
	void execSetPrivateProfileString(struct FFrame &, void * const);
	void execSetPropertyText(struct FFrame &, void * const);
	void execSin(struct FFrame &, void * const);
	void execSmerp(struct FFrame &, void * const);
	void execSqrt(struct FFrame &, void * const);
	void execSquare(struct FFrame &, void * const);
	void execStaticSaveConfig(struct FFrame &, void * const);
	void execStop(struct FFrame &, void * const);
	void execStringConst(struct FFrame &, void * const);
	void execStringToBool(struct FFrame &, void * const);
	void execStringToByte(struct FFrame &, void * const);
	void execStringToFloat(struct FFrame &, void * const);
	void execStringToInt(struct FFrame &, void * const);
	void execStringToName(struct FFrame &, void * const);
	void execStringToRotator(struct FFrame &, void * const);
	void execStringToVector(struct FFrame &, void * const);
	void execStructCmpEq(struct FFrame &, void * const);
	void execStructCmpNe(struct FFrame &, void * const);
	void execStructMember(struct FFrame &, void * const);
	void execSubtractEqual_ByteByte(struct FFrame &, void * const);
	void execSubtractEqual_FloatFloat(struct FFrame &, void * const);
	void execSubtractEqual_IntInt(struct FFrame &, void * const);
	void execSubtractEqual_RotatorRotator(struct FFrame &, void * const);
	void execSubtractEqual_VectorVector(struct FFrame &, void * const);
	void execSubtractSubtract_Byte(struct FFrame &, void * const);
	void execSubtractSubtract_Int(struct FFrame &, void * const);
	void execSubtractSubtract_PreByte(struct FFrame &, void * const);
	void execSubtractSubtract_PreInt(struct FFrame &, void * const);
	void execSubtract_FloatFloat(struct FFrame &, void * const);
	void execSubtract_IntInt(struct FFrame &, void * const);
	void execSubtract_PreFloat(struct FFrame &, void * const);
	void execSubtract_PreInt(struct FFrame &, void * const);
	void execSubtract_PreVector(struct FFrame &, void * const);
	void execSubtract_RotatorRotator(struct FFrame &, void * const);
	void execSubtract_VectorVector(struct FFrame &, void * const);
	void execSwitch(struct FFrame &, void * const);
	void execTan(struct FFrame &, void * const);
	void execTrue(struct FFrame &, void * const);
	void execUndefined(struct FFrame &, void * const);
	void execUnicodeStringConst(struct FFrame &, void * const);
	void execVRand(struct FFrame &, void * const);
	void execVSize(struct FFrame &, void * const);
	void execVSizeSquared(struct FFrame &, void * const);
	void execVectorConst(struct FFrame &, void * const);
	void execVectorToBool(struct FFrame &, void * const);
	void execVectorToRotator(struct FFrame &, void * const);
	void execVectorToString(struct FFrame &, void * const);
	void execVirtualFunction(struct FFrame &, void * const);
	void execWarn(struct FFrame &, void * const);
	void execXorXor_BoolBool(struct FFrame &, void * const);
	void execXor_IntInt(struct FFrame &, void * const);
	static void CDECL operator delete(void *, unsigned int);
	static void * CDECL operator new(unsigned int, class UObject *, class FName, DWORD);
	static void * CDECL operator new(unsigned int, enum EInternal *);
	class UObject & operator=(class UObject const &);
private:
	void AddObject(INT);
	static void CDECL CacheDrivers(INT);
	static class UObject * GAutoRegister();
	static INT GImportCount();
	static TCHAR * GLanguage();
	static class TArray<int> GObjAvailable();
	static INT GObjBeginLoadCount();
	static TCHAR * GObjCachedLanguage();
	static class TArray<class FRegistryObjectInfo> GObjDrivers();
	static class UObject * * GObjHash();
	static INT GObjInitialized();
	static class TArray<class UObject *> GObjLoaded();
	static class TArray<class UObject *> GObjLoaders();
	static INT GObjNoRegister();
	static class TArray<class UObject *> GObjObjects;
	static class TMultiMap<class FName,class FName> * GObjPackageRemap();
	static class TArray<class FPreferencesInfo> GObjPreferences();
	static INT GObjRegisterCount();
	static class TArray<class UObject *> GObjRegistrants();
	static class TArray<class UObject *> GObjRoot();
	static class UPackage * GObjTransientPkg();
	static class ULinkerLoad * CDECL GetLoader(INT);
	void HashObject();
	static class FName CDECL MakeUniqueObjectName(class UObject *, class UClass *);
	static class UClass PrivateStaticClass();
	static void CDECL PurgeGarbage();
	static INT CDECL ResolveName(class UObject * &, TCHAR const * &, INT, INT);
	static void CDECL SafeLoadError(DWORD, TCHAR const *, TCHAR const *, ...);
	void SetLinker(class ULinkerLoad *, INT);
	void UnhashObject(INT);

	/*
	// Declarations.
	DECLARE_BASE_CLASS(UObject,UObject,CLASS_Abstract,Core)
	typedef UObject WithinClass;
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};
	static const TCHAR* StaticConfigName() {return TEXT("System");}

	// Friends.
	friend class FObjectIterator;
	friend class ULinkerLoad;
	friend class ULinkerSave;
	friend class UPackageMap;
	friend class FArchiveTagUsed;
	friend struct FObjectImport;
	friend struct FObjectExport;

private:
	// Internal per-object variables.
	INT			Index;				// Index of object into table.
	UObject*		HashNext;			// Next object in this hash bin.
	FStateFrame*		StateFrame;			// Main script execution stack.
	ULinkerLoad*		_Linker;			// Linker it came from, or NULL if none.
	INT			_LinkerIndex;		// Index of this object in the linker's export map.
	UObject*		Outer;				// Object this object resides in.
	DWORD			ObjectFlags;		// Private EObjectFlags used by object manager.
	FName			Name;				// Name of the object.
	UClass*			Class;	  			// Class the object belongs to.

	INT			CacheIndex;			//0x0028 LineageII
	INT			HashNextBuffer;			//0x002C LineageII
	INT			IndexBuffer;			//0x0030 LineageII


	// Private systemwide variables.
	static UBOOL			GObjInitialized;	// Whether initialized.
	static UBOOL            GObjNoRegister;		// Registration disable.
	static INT				GObjBeginLoadCount;	// Count for BeginLoad multiple loads.
	static INT				GObjRegisterCount;  // ProcessRegistrants entry counter.
	static INT				GImportCount;		// Imports for EndLoad optimization.
	static UObject*			GObjHash[4096];		// Object hash.
	static UObject*			GAutoRegister;		// Objects to automatically register.
	static TArray<UObject*> GObjLoaded;			// Objects that might need preloading.
	static TArray<UObject*>	GObjRoot;			// Top of active object graph.
	static TArray<UObject*>	GObjObjects;		// List of all objects.
	static TArray<INT>      GObjAvailable;		// Available object indices.
	static TArray<UObject*>	GObjLoaders;		// Array of loaders.
	static UPackage*		GObjTransientPkg;	// Transient package.
	static TCHAR			GObjCachedLanguage[32]; // Language;
	static TArray<UObject*> GObjRegistrants;		// Registrants during ProcessRegistrants call.
	static TArray<FPreferencesInfo> GObjPreferences; // Prefereces cache.
	static TArray<FRegistryObjectInfo> GObjDrivers; // Drivers cache.
	static TMultiMap<FName,FName>* GObjPackageRemap; // Remap table for loading renamed packages.
	static TCHAR GLanguage[64];

	// Private functions.
	void AddObject( INT Index );
	void HashObject();
	void UnhashObject( INT OuterIndex );
	void SetLinker( ULinkerLoad* L, INT I );

	// Private systemwide functions.
	static ULinkerLoad* GetLoader( INT i );
	static FName MakeUniqueObjectName( UObject* Outer, UClass* Class );
	static UBOOL ResolveName( UObject*& Outer, const TCHAR*& Name, UBOOL Create, UBOOL Throw );
	static void SafeLoadError( DWORD LoadFlags, const TCHAR* Error, const TCHAR* Fmt, ... );
	static void PurgeGarbage();
	static void CacheDrivers( UBOOL ForceRefresh );

public:
	// Constructors.
	UObject();
	UObject( const UObject& Src );
	UObject( ENativeConstructor, UClass* InClass, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UObject( EStaticConstructor, const TCHAR* InName, const TCHAR* InPackageName, DWORD InFlags );
	UObject( EInPlaceConstructor, UClass* InClass, UObject* InOuter, FName InName, DWORD InFlags );
	UObject& operator=( const UObject& );
	void StaticConstructor();
	static void InternalConstructor( void* X )
		{ new( (EInternal*)X )UObject(); }

	// Destructors.
	virtual ~UObject();
	void operator delete( void* Object, size_t Size )
		{guard(UObject::operator delete); appFree( Object ); unguard;}

	// FUnknown interface.
	virtual DWORD STDCALL QueryInterface( const FGuid& RefIID, void** InterfacePtr );
	virtual DWORD STDCALL AddRef();
	virtual DWORD STDCALL Release();

	// UObject interface.
   virtual void ProcessDelegate(class FName,struct FScriptDelegate *,void *,void *);
	virtual void ProcessEvent( UFunction* Function, void* Parms, void* Result=NULL );
	virtual void ProcessState( FLOAT DeltaSeconds );
	virtual UBOOL ProcessRemoteFunction( UFunction* Function, void* Parms, FFrame* Stack );
	virtual void Modify();
	virtual void PostLoad();
	virtual void Destroy();
	virtual void Serialize( FArchive& Ar );
	virtual UBOOL IsPendingKill() {return 0;}
	virtual EGotoState GotoState( FName State );
	virtual INT GotoLabel( FName Label );
	virtual void InitExecution();
	virtual void ShutdownAfterError();
	virtual void PostEditChange();
	virtual void CallFunction( FFrame& TheStack, RESULT_DECL, UFunction* Function );
	virtual UBOOL ScriptConsoleExec( const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor );
	virtual void Register();
	virtual void LanguageChange();

	// Systemwide functions.
	static UObject* StaticFindObject( UClass* Class, UObject* InOuter, const TCHAR* Name, UBOOL ExactClass=0 );
	static UObject* StaticFindObjectChecked( UClass* Class, UObject* InOuter, const TCHAR* Name, UBOOL ExactClass=0 );
	static UObject* StaticLoadObject( UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox );
	static UClass* StaticLoadClass( UClass* BaseClass, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox );
	static UObject* StaticAllocateObject( UClass* Class, UObject* InOuter=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0, UObject* Template=NULL, FOutputDevice* Error=GError, UObject* Ptr=NULL );
	//static UObject* StaticConstructObject( UClass* Class, UObject* InOuter=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0       , UObject* Template=NULL, FOutputDevice* Error=GError );
    static   UObject* StaticConstructObject( UClass* Class ,UObject* InOuter=(UObject*)GetTransientPackage(), FName Name=NAME_None, DWORD SetFlags=0,UObject * Template=NULL ,FOutputDevice * Error=GError ,UObject * Z=NULL);
	static void StaticInit();
	static void StaticExit();
	static UBOOL StaticExec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	static void StaticTick();
	static UObject* LoadPackage( UObject* InOuter, const TCHAR* Filename, DWORD LoadFlags );
	static UBOOL SavePackage( UObject* InOuter, UObject* Base, DWORD TopLevelFlags, const TCHAR* Filename, FOutputDevice* Error=GError, ULinkerLoad* Conform=NULL );
	static void CollectGarbage( DWORD KeepFlags );
	static void SerializeRootSet( FArchive& Ar, DWORD KeepFlags, DWORD RequiredFlags );
	static UBOOL IsReferenced( UObject*& Res, DWORD KeepFlags, UBOOL IgnoreReference );
	static UBOOL AttemptDelete( UObject*& Res, DWORD KeepFlags, UBOOL IgnoreReference );
	static void BeginLoad();
	static void EndLoad();
	static void InitProperties( BYTE* Data, INT DataCount, UClass* DefaultsClass, BYTE* Defaults, INT DefaultsCount );
	static void ExitProperties( BYTE* Data, UClass* Class );
	static void ResetLoaders( UObject* InOuter, UBOOL DynamicOnly, UBOOL ForceLazyLoad );
	static UPackage* CreatePackage( UObject* InOuter, const TCHAR* PkgName );
	static ULinkerLoad* GetPackageLinker( UObject* InOuter, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox, FGuid* CompatibleGuid );
	static void StaticShutdownAfterError();
	static UObject* GetIndexedObject( INT Index );
	static void GlobalSetProperty( const TCHAR* Value, UClass* Class, UProperty* Property, INT Offset, UBOOL Immediate );
	static void ExportProperties( FOutputDevice& Out, UClass* ObjectClass, BYTE* Object, INT Indent, UClass* DiffClass, BYTE* Diff );
	static void ResetConfig( UClass* Class );
	static void GetRegistryObjects( TArray<FRegistryObjectInfo>& Results, UClass* Class, UClass* MetaClass, UBOOL ForceRefresh );
	static void GetPreferences( TArray<FPreferencesInfo>& Results, const TCHAR* Category, UBOOL ForceRefresh );
	static UBOOL GetInitialized();
	static UPackage* GetTransientPackage();
	static void VerifyLinker( ULinkerLoad* Linker );
	static void ProcessRegistrants();
	static void BindPackage( UPackage* Pkg );
	static const TCHAR* GetLanguage();
	static void SetLanguage( const TCHAR* LanguageExt );
	static INT GetObjectHash( FName ObjName, INT Outer )
	{
		return (ObjName.GetIndex() ^ Outer) & (ARRAY_COUNT(GObjHash)-1);
	}

	// Functions.
	void AddToRoot();
	void RemoveFromRoot();
	const TCHAR* GetFullName( TCHAR* Str=NULL ) const;
	const TCHAR* GetPathName( UObject* StopOuter=NULL, TCHAR* Str=NULL ) const;
	UBOOL IsValid();
	void ConditionalRegister();
	UBOOL ConditionalDestroy();
	void ConditionalPostLoad();
	void ConditionalShutdownAfterError();
	UBOOL IsA( UClass* SomeBaseClass ) const;
	UBOOL IsIn( UObject* SomeOuter ) const;
	UBOOL IsProbing( FName ProbeName );
	void Rename( const TCHAR* NewName=NULL );
	UField* FindObjectField( FName InName, UBOOL Global=0 );
	UFunction* FindFunction( FName InName, UBOOL Global=0 );
	UFunction* FindFunctionChecked( FName InName, UBOOL Global=0 );
	UState* FindState( FName InName );
	void SaveConfig( DWORD Flags=CPF_Config, const TCHAR* Filename=NULL );
	void LoadConfig( UBOOL Propagate=0, UClass* Class=NULL, const TCHAR* Filename=NULL );
	void LoadLocalized( UBOOL Propagate=0, UClass* Class=NULL );
	void InitClassDefaultObject( UClass* InClass );
	void ProcessInternal( FFrame& TheStack, void*const Result );
	void ParseParms( const TCHAR* Parms );

	// Accessors.
	UClass* GetClass() const
	{
		return Class;
	}
	void SetClass(UClass* NewClass)
	{
		Class = NewClass;
	}
	DWORD GetFlags() const
	{
		return ObjectFlags;
	}
	void SetFlags( DWORD NewFlags )
	{
		ObjectFlags |= NewFlags;
		checkSlow(Name!=NAME_None || !(ObjectFlags&RF_Public));
	}
	void ClearFlags( DWORD NewFlags )
	{
		ObjectFlags &= ~NewFlags;
		checkSlow(Name!=NAME_None || !(ObjectFlags&RF_Public));
	}
	const TCHAR* GetName() const
	{
		return *Name;
	}
	const FName GetFName() const
	{
		return Name;
	}
	UObject* GetOuter() const
	{
		return Outer;
	}
	DWORD GetIndex() const
	{
		return Index;
	}
	ULinkerLoad* GetLinker()
	{
		return _Linker;
	}
	INT GetLinkerIndex()
	{
		return _LinkerIndex;
	}
	FStateFrame* GetStateFrame()
	{
		return StateFrame;
	}

	// UnrealScript intrinsics.
	#define DECLARE_FUNCTION(func) void func( FFrame& TheStack, RESULT_DECL );
	DECLARE_FUNCTION(execUndefined)
	DECLARE_FUNCTION(execLocalVariable)
	DECLARE_FUNCTION(execInstanceVariable)
	DECLARE_FUNCTION(execDefaultVariable)
	DECLARE_FUNCTION(execArrayElement)
	DECLARE_FUNCTION(execDynArrayElement)
	DECLARE_FUNCTION(execBoolVariable)
	DECLARE_FUNCTION(execClassDefaultVariable)
	DECLARE_FUNCTION(execEndFunctionParms)
	DECLARE_FUNCTION(execNothing)
	DECLARE_FUNCTION(execStop)
	DECLARE_FUNCTION(execEndCode)
	DECLARE_FUNCTION(execSwitch)
	DECLARE_FUNCTION(execCase)
	DECLARE_FUNCTION(execJump)
	DECLARE_FUNCTION(execJumpIfNot)
	DECLARE_FUNCTION(execAssert)
	DECLARE_FUNCTION(execGotoLabel)
	DECLARE_FUNCTION(execLet)
	DECLARE_FUNCTION(execLetBool)
	DECLARE_FUNCTION(execEatString)
	DECLARE_FUNCTION(execSelf)
	DECLARE_FUNCTION(execContext)
	DECLARE_FUNCTION(execVirtualFunction)
	DECLARE_FUNCTION(execFinalFunction)
	DECLARE_FUNCTION(execGlobalFunction)
	DECLARE_FUNCTION(execStructCmpEq)
	DECLARE_FUNCTION(execStructCmpNe)
	DECLARE_FUNCTION(execStructMember)
	DECLARE_FUNCTION(execIntConst)
	DECLARE_FUNCTION(execFloatConst)
	DECLARE_FUNCTION(execStringConst)
	DECLARE_FUNCTION(execUnicodeStringConst)
	DECLARE_FUNCTION(execObjectConst)
	DECLARE_FUNCTION(execNameConst)
	DECLARE_FUNCTION(execByteConst)
	DECLARE_FUNCTION(execIntZero)
	DECLARE_FUNCTION(execIntOne)
	DECLARE_FUNCTION(execTrue)
	DECLARE_FUNCTION(execFalse)
	DECLARE_FUNCTION(execNoObject)
	DECLARE_FUNCTION(execIntConstByte)
	DECLARE_FUNCTION(execDynamicCast)
	DECLARE_FUNCTION(execMetaCast)
	DECLARE_FUNCTION(execByteToInt)
	DECLARE_FUNCTION(execByteToBool)
	DECLARE_FUNCTION(execByteToFloat)
	DECLARE_FUNCTION(execByteToString)
	DECLARE_FUNCTION(execIntToByte)
	DECLARE_FUNCTION(execIntToBool)
	DECLARE_FUNCTION(execIntToFloat)
	DECLARE_FUNCTION(execIntToString)
	DECLARE_FUNCTION(execBoolToByte)
	DECLARE_FUNCTION(execBoolToInt)
	DECLARE_FUNCTION(execBoolToFloat)
	DECLARE_FUNCTION(execBoolToString)
	DECLARE_FUNCTION(execFloatToByte)
	DECLARE_FUNCTION(execFloatToInt)
	DECLARE_FUNCTION(execFloatToBool)
	DECLARE_FUNCTION(execFloatToString)
	DECLARE_FUNCTION(execRotationConst)
	DECLARE_FUNCTION(execVectorConst)
	DECLARE_FUNCTION(execStringToVector)
	DECLARE_FUNCTION(execStringToRotator)
	DECLARE_FUNCTION(execVectorToBool)
	DECLARE_FUNCTION(execVectorToString)
	DECLARE_FUNCTION(execVectorToRotator)
	DECLARE_FUNCTION(execRotatorToBool)
	DECLARE_FUNCTION(execRotatorToVector)
	DECLARE_FUNCTION(execRotatorToString)
    DECLARE_FUNCTION(execRotRand)
    DECLARE_FUNCTION(execGetUnAxes)
    DECLARE_FUNCTION(execGetAxes)
    DECLARE_FUNCTION(execSubtractEqual_RotatorRotator)
    DECLARE_FUNCTION(execAddEqual_RotatorRotator)
    DECLARE_FUNCTION(execSubtract_RotatorRotator)
    DECLARE_FUNCTION(execAdd_RotatorRotator)
    DECLARE_FUNCTION(execDivideEqual_RotatorFloat)
    DECLARE_FUNCTION(execMultiplyEqual_RotatorFloat)
    DECLARE_FUNCTION(execDivide_RotatorFloat)
    DECLARE_FUNCTION(execMultiply_FloatRotator)
    DECLARE_FUNCTION(execMultiply_RotatorFloat)
    DECLARE_FUNCTION(execNotEqual_RotatorRotator)
    DECLARE_FUNCTION(execEqualEqual_RotatorRotator)
    DECLARE_FUNCTION(execMirrorVectorByNormal)
    DECLARE_FUNCTION(execVRand)
#if 1 //Math added by Legend on 4/12/2000
	DECLARE_FUNCTION(execRandomSpreadVector)
#endif
    DECLARE_FUNCTION(execInvert)
    DECLARE_FUNCTION(execNormal)
    DECLARE_FUNCTION(execVSize)
    DECLARE_FUNCTION(execSubtractEqual_VectorVector)
    DECLARE_FUNCTION(execAddEqual_VectorVector)
    DECLARE_FUNCTION(execDivideEqual_VectorFloat)
    DECLARE_FUNCTION(execMultiplyEqual_VectorVector)
    DECLARE_FUNCTION(execMultiplyEqual_VectorFloat)
    DECLARE_FUNCTION(execCross_VectorVector)
    DECLARE_FUNCTION(execDot_VectorVector)
    DECLARE_FUNCTION(execNotEqual_VectorVector)
    DECLARE_FUNCTION(execEqualEqual_VectorVector)
    DECLARE_FUNCTION(execGreaterGreater_VectorRotator)
    DECLARE_FUNCTION(execLessLess_VectorRotator)
    DECLARE_FUNCTION(execSubtract_VectorVector)
    DECLARE_FUNCTION(execAdd_VectorVector)
    DECLARE_FUNCTION(execDivide_VectorFloat)
    DECLARE_FUNCTION(execMultiply_VectorVector)
    DECLARE_FUNCTION(execMultiply_FloatVector)
    DECLARE_FUNCTION(execMultiply_VectorFloat)
    DECLARE_FUNCTION(execSubtract_PreVector)
	DECLARE_FUNCTION(execOrthoRotation);
	DECLARE_FUNCTION(execNormalize);
	DECLARE_FUNCTION(execObjectToBool)
	DECLARE_FUNCTION(execObjectToString)
	DECLARE_FUNCTION(execNameToBool)
	DECLARE_FUNCTION(execNameToString)
	DECLARE_FUNCTION(execStringToByte)
	DECLARE_FUNCTION(execStringToInt)
	DECLARE_FUNCTION(execStringToBool)
	DECLARE_FUNCTION(execStringToFloat)
	DECLARE_FUNCTION(execNot_PreBool)
	DECLARE_FUNCTION(execEqualEqual_BoolBool)
	DECLARE_FUNCTION(execNotEqual_BoolBool)
	DECLARE_FUNCTION(execAndAnd_BoolBool)
	DECLARE_FUNCTION(execXorXor_BoolBool)
	DECLARE_FUNCTION(execOrOr_BoolBool)
	DECLARE_FUNCTION(execMultiplyEqual_ByteByte)
	DECLARE_FUNCTION(execDivideEqual_ByteByte)
	DECLARE_FUNCTION(execAddEqual_ByteByte)
	DECLARE_FUNCTION(execSubtractEqual_ByteByte)
	DECLARE_FUNCTION(execAddAdd_PreByte)
	DECLARE_FUNCTION(execSubtractSubtract_PreByte)
	DECLARE_FUNCTION(execAddAdd_Byte)
	DECLARE_FUNCTION(execSubtractSubtract_Byte)
	DECLARE_FUNCTION(execComplement_PreInt)
	DECLARE_FUNCTION(execSubtract_PreInt)
	DECLARE_FUNCTION(execMultiply_IntInt)
	DECLARE_FUNCTION(execDivide_IntInt)
	DECLARE_FUNCTION(execAdd_IntInt)
	DECLARE_FUNCTION(execSubtract_IntInt)
	DECLARE_FUNCTION(execLessLess_IntInt)
	DECLARE_FUNCTION(execGreaterGreater_IntInt)
	DECLARE_FUNCTION(execGreaterGreaterGreater_IntInt)
	DECLARE_FUNCTION(execLess_IntInt)
	DECLARE_FUNCTION(execGreater_IntInt)
	DECLARE_FUNCTION(execLessEqual_IntInt)
	DECLARE_FUNCTION(execGreaterEqual_IntInt)
	DECLARE_FUNCTION(execEqualEqual_IntInt)
	DECLARE_FUNCTION(execNotEqual_IntInt)
	DECLARE_FUNCTION(execAnd_IntInt)
	DECLARE_FUNCTION(execXor_IntInt)
	DECLARE_FUNCTION(execOr_IntInt)
	DECLARE_FUNCTION(execMultiplyEqual_IntFloat)
	DECLARE_FUNCTION(execDivideEqual_IntFloat)
	DECLARE_FUNCTION(execAddEqual_IntInt)
	DECLARE_FUNCTION(execSubtractEqual_IntInt)
	DECLARE_FUNCTION(execAddAdd_PreInt)
	DECLARE_FUNCTION(execSubtractSubtract_PreInt)
	DECLARE_FUNCTION(execAddAdd_Int)
	DECLARE_FUNCTION(execSubtractSubtract_Int)
	DECLARE_FUNCTION(execRand)
	DECLARE_FUNCTION(execMin)
	DECLARE_FUNCTION(execMax)
	DECLARE_FUNCTION(execClamp)
	DECLARE_FUNCTION(execSubtract_PreFloat)
	DECLARE_FUNCTION(execMultiplyMultiply_FloatFloat)
	DECLARE_FUNCTION(execMultiply_FloatFloat)
	DECLARE_FUNCTION(execDivide_FloatFloat)
	DECLARE_FUNCTION(execPercent_FloatFloat)
	DECLARE_FUNCTION(execAdd_FloatFloat)
	DECLARE_FUNCTION(execSubtract_FloatFloat)
	DECLARE_FUNCTION(execLess_FloatFloat)
	DECLARE_FUNCTION(execGreater_FloatFloat)
	DECLARE_FUNCTION(execLessEqual_FloatFloat)
	DECLARE_FUNCTION(execGreaterEqual_FloatFloat)
	DECLARE_FUNCTION(execEqualEqual_FloatFloat)
	DECLARE_FUNCTION(execNotEqual_FloatFloat)
	DECLARE_FUNCTION(execComplementEqual_FloatFloat)
	DECLARE_FUNCTION(execMultiplyEqual_FloatFloat)
	DECLARE_FUNCTION(execDivideEqual_FloatFloat)
	DECLARE_FUNCTION(execAddEqual_FloatFloat)
	DECLARE_FUNCTION(execSubtractEqual_FloatFloat)
	DECLARE_FUNCTION(execAbs)
	DECLARE_FUNCTION(execSin)
	DECLARE_FUNCTION(execCos)
	DECLARE_FUNCTION(execTan)
	DECLARE_FUNCTION(execAtan)
	DECLARE_FUNCTION(execExp)
	DECLARE_FUNCTION(execLoge)
	DECLARE_FUNCTION(execSqrt)
	DECLARE_FUNCTION(execSquare)
	DECLARE_FUNCTION(execFRand)
	DECLARE_FUNCTION(execFMin)
	DECLARE_FUNCTION(execFMax)
	DECLARE_FUNCTION(execFClamp)
	DECLARE_FUNCTION(execLerp)
	DECLARE_FUNCTION(execSmerp)
	DECLARE_FUNCTION(execConcat_StringString)
	DECLARE_FUNCTION(execAt_StringString)
	DECLARE_FUNCTION(execLess_StringString)
	DECLARE_FUNCTION(execGreater_StringString)
	DECLARE_FUNCTION(execLessEqual_StringString)
	DECLARE_FUNCTION(execGreaterEqual_StringString)
	DECLARE_FUNCTION(execEqualEqual_StringString)
	DECLARE_FUNCTION(execNotEqual_StringString)
	DECLARE_FUNCTION(execComplementEqual_StringString)
	DECLARE_FUNCTION(execLen)
	DECLARE_FUNCTION(execInStr)
	DECLARE_FUNCTION(execMid)
	DECLARE_FUNCTION(execLeft)
	DECLARE_FUNCTION(execRight)
	DECLARE_FUNCTION(execCaps)
	DECLARE_FUNCTION(execChr)
	DECLARE_FUNCTION(execAsc)
	DECLARE_FUNCTION(execEqualEqual_ObjectObject)
	DECLARE_FUNCTION(execNotEqual_ObjectObject)
	DECLARE_FUNCTION(execEqualEqual_NameName)
	DECLARE_FUNCTION(execNotEqual_NameName)
	DECLARE_FUNCTION(execLog)
	DECLARE_FUNCTION(execWarn)
	DECLARE_FUNCTION(execNew)
#if 1 //Fix added by Legend on 4/12/2000
	DECLARE_FUNCTION(execRandRange)
#endif
	DECLARE_FUNCTION(execClassIsChildOf)
	DECLARE_FUNCTION(execClassContext)
	DECLARE_FUNCTION(execGoto)
	DECLARE_FUNCTION(execGotoState)
	DECLARE_FUNCTION(execIsA)
	DECLARE_FUNCTION(execEnable)
	DECLARE_FUNCTION(execDisable)
	DECLARE_FUNCTION(execIterator)
	DECLARE_FUNCTION(execLocalize)
	DECLARE_FUNCTION(execNativeParm)
	DECLARE_FUNCTION(execGetPropertyText)
	DECLARE_FUNCTION(execSetPropertyText)
	DECLARE_FUNCTION(execSaveConfig)
	DECLARE_FUNCTION(execStaticSaveConfig)
	DECLARE_FUNCTION(execResetConfig)
	DECLARE_FUNCTION(execGetEnum)
	DECLARE_FUNCTION(execDynamicLoadObject)
	DECLARE_FUNCTION(execIsInState)
	DECLARE_FUNCTION(execGetStateName)
	DECLARE_FUNCTION(execHighNative0)
	DECLARE_FUNCTION(execHighNative1)
	DECLARE_FUNCTION(execHighNative2)
	DECLARE_FUNCTION(execHighNative3)
	DECLARE_FUNCTION(execHighNative4)
	DECLARE_FUNCTION(execHighNative5)
	DECLARE_FUNCTION(execHighNative6)
	DECLARE_FUNCTION(execHighNative7)
	DECLARE_FUNCTION(execHighNative8)
	DECLARE_FUNCTION(execHighNative9)
	DECLARE_FUNCTION(execHighNative10)
	DECLARE_FUNCTION(execHighNative11)
	DECLARE_FUNCTION(execHighNative12)
	DECLARE_FUNCTION(execHighNative13)
	DECLARE_FUNCTION(execHighNative14)
	DECLARE_FUNCTION(execHighNative15)

	// UnrealScript calling stubs.
    void eventBeginState()
    {
        ProcessEvent(FindFunctionChecked(NAME_BeginState),NULL);
    }
    void eventEndState()
    {
        ProcessEvent(FindFunctionChecked(NAME_EndState),NULL);
    }
*/
};

/*----------------------------------------------------------------------------
	Core templates.
----------------------------------------------------------------------------*/

// Hash function.
inline DWORD GetTypeHash( const UObject* A )
{
	return A ? A->GetIndex() : 0;
}

// Parse an object name in the input stream.
template< class T > UBOOL ParseObject( const TCHAR* Stream, const TCHAR* Match, T*& Obj, UObject* Outer )
{
	return ParseObject( Stream, Match, T::StaticClass(), *(UObject **)&Obj, Outer );
}

// Find an optional object.
template< class T > T* FindObject( UObject* Outer, const TCHAR* Name, UBOOL ExactClass=0 )
{
	return (T*)UObject::StaticFindObject( T::StaticClass(), Outer, Name, ExactClass );
}

// Find an object, no failure allowed.
template< class T > T* FindObjectChecked( UObject* Outer, const TCHAR* Name, UBOOL ExactClass=0 )
{
	return (T*)UObject::StaticFindObjectChecked( T::StaticClass(), Outer, Name, ExactClass );
}

// Dynamically cast an object type-safely.
template< class T > T* Cast( UObject* Src )
{
	return Src && Src->IsA(T::StaticClass()) ? (T*)Src : NULL;
}
template< class T, class U > T* CastChecked( U* Src )
{
	if( !Src || !Src->IsA(T::StaticClass()) )
		appErrorf( TEXT("Cast of %s to %s failed"), Src ? Src->GetFullName() : TEXT("NULL"), T::StaticClass()->GetName() );
	return (T*)Src;
}

// Construct an object of a particular class.
template< class T > T* ConstructObject( UClass* Class, UObject* Outer=(UObject*)-1, FName Name=NAME_None, DWORD SetFlags=0 )
{
	check(Class->IsChildOf(T::StaticClass()));
	if( Outer==(UObject*)-1 )
		Outer = UObject::GetTransientPackage();
	return (T*)UObject::StaticConstructObject( Class, Outer, Name, SetFlags );
}

// Load an object.
template< class T > T* LoadObject( UObject* Outer, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox )
{
	return (T*)UObject::StaticLoadObject( T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox );
}

// Load a class object.
template< class T > UClass* LoadClass( UObject* Outer, const TCHAR* Name, const TCHAR* Filename, DWORD LoadFlags, UPackageMap* Sandbox )
{
	return UObject::StaticLoadClass( T::StaticClass(), Outer, Name, Filename, LoadFlags, Sandbox );
}

// Get default object of a class.
template< class T > T* GetDefault()
{
	return (T*)&T::StaticClass()->Defaults(0);
}

/*----------------------------------------------------------------------------
	Object iterators.
----------------------------------------------------------------------------*/

//
// Class for iterating through all objects.
//
class FObjectIterator
{
public:
	FObjectIterator( UClass* InClass=UObject::StaticClass() )
	:	Class( InClass ), Index( -1 )
	{
		check(Class);
		++*this;
	}
	void operator++()
	{
		while( ++Index<UObject::GObjObjects.Num() && (!UObject::GObjObjects(Index) || !UObject::GObjObjects(Index)->IsA(Class)) );
	}
	UObject* operator*()
	{
		return UObject::GObjObjects(Index);
	}
	UObject* operator->()
	{
		return UObject::GObjObjects(Index);
	}
	operator UBOOL()
	{
		return Index<UObject::GObjObjects.Num();
	}
protected:
	UClass* Class;
	INT Index;
};

//
// Class for iterating through all objects which inherit from a
// specified base class.
//
template< class T > class TObjectIterator : public FObjectIterator
{
public:
	TObjectIterator()
	:	FObjectIterator( T::StaticClass() )
	{}
	T* operator* ()
	{
		return (T*)FObjectIterator::operator*();
	}
	T* operator-> ()
	{
		return (T*)FObjectIterator::operator->();
	}
};

#define AUTO_INITIALIZE_REGISTRANTS \
	UObject::StaticClass(); \
	UClass::StaticClass(); \
	USubsystem::StaticClass(); \
	USystem::StaticClass(); \
	UProperty::StaticClass(); \
	UStructProperty::StaticClass(); \
	UField::StaticClass(); \
	UMapProperty::StaticClass(); \
	UArrayProperty::StaticClass(); \
	UFixedArrayProperty::StaticClass(); \
	UStrProperty::StaticClass(); \
	UNameProperty::StaticClass(); \
	UClassProperty::StaticClass(); \
	UObjectProperty::StaticClass(); \
	UFloatProperty::StaticClass(); \
	UBoolProperty::StaticClass(); \
	UIntProperty::StaticClass(); \
	UByteProperty::StaticClass(); \
	ULanguage::StaticClass(); \
	UTextBufferFactory::StaticClass(); \
	UFactory::StaticClass(); \
	UPackage::StaticClass(); \
	UCommandlet::StaticClass(); \
	ULinkerSave::StaticClass(); \
	ULinker::StaticClass(); \
	ULinkerLoad::StaticClass(); \
	UEnum::StaticClass(); \
	UTextBuffer::StaticClass(); \
	UPackageMap::StaticClass(); \
	UConst::StaticClass(); \
	UFunction::StaticClass(); \
	UStruct::StaticClass();

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
