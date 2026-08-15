/******************************************************************************
*
* Game Engine
*
******************************************************************************/

//Note sometimes the export names can change between game versions slightly.
#define WINDOWTEXT "Lineage II"
#define FNAMEENTRYARRAY "?Names@FName@@0V?$TArray@PAUFNameEntry@@@@A"
#define FSTRING_CONSTRUCTPCHAR "??0FString@@QAE@PBD@Z" // public: __thiscall FString::FString(char const *);
#define FSTRING_DESTRUCT "??1FString@@QAE@XZ"
#define UOBJECTARRAY "?GObjObjects@UObject@@1V?$TArray@PAVUObject@@@@A"
#define UOBJECTHASH "?GObjHash@UObject@@1PAPAV1@A"
#define WINDOWSVIEWPORT "WindowsViewport WindowsClient0.WindowsViewport0"

class FVector
{
public:
	float X;
	float Y;
	float Z;
};

//Functions prototypes
typedef void (__cdecl * FLandMarkAddLandMark_typedef)(class FVector, class FVector, int);
typedef void (__cdecl * UNetworkHandlerMTL_typedef)(void *, class FVector, class FVector, void *, int);


//Structures
template<class Type> struct TArray
{
	Type * pArray;
	DWORD  Count;
	DWORD  Max;
};


typedef DWORD FName;

struct FNameEntry
{
	int   FNameIndex;
	DWORD Unknown0x004;
	DWORD Unknown0x008;
	WCHAR Name[1];
};


struct UClass;
struct UObject
{
	DWORD **     pVMT;						//0x0000 
	DWORD        ObjectInternal;			//0x0004 // Index of object into table.
	UObject *    pHash;						//0x0008 // Next object in this hash bin.
	DWORD *      pFrame;					//0x000C // Main script execution stack.
	DWORD *      pLinkerLoad;				//0x0010 // Linker it came from, or NULL if none.
	int          PackageNumber;				//0x0014 // Index of this object in the linker's export map.
	UObject *    pOuter;					//0x001C
	DWORD        ObjectFlags;				//0x0020
	FName        Name;						//0x0024
	UClass *     pClass;					//0x0028
	DWORD        CacheIndex;				//0x0028 LineageII
	DWORD        HashNextBuffer;			//0x002C LineageII
	DWORD        IndexBuffer;				//0x0030 LineageII

	void GetCPPName(char * pBuffer);
	void GetFullName(char * pBuffer);
	void GetName(char * pBuffer);
	int  IsA(UClass * pClass);
};


struct UField : UObject 
{
	UField *           pSuper;				//0x0038
	UField *           pNext;				//0x003C
	DWORD              Unknown0x003C;		//0x0040
};

struct UProperty : UField 
{
	DWORD              ElementCount;		//0x0044
	DWORD              ElementSize;			//0x0048
	DWORD              Flags;				//0x004C
	WORD               ReplicationOffset;	//0x0050
	WORD               Unknown0x0050;		//0x0052
	WORD               Unknown0x0052;		//0x0054
	DWORD              CStructOffset;		//0x0058
	//char               Unknown0x005C[0x20]; CT1 3 Feb 2008
	char               Unknown0x005C[0x2C];
	UClass *           pRelatedClass;		//0x007C
};

struct UStruct : UField 
{
	DWORD              Unknown0x0040;		//0x0044
	DWORD              Unknown0x0044;		//0x0048
	UField *           pChildren;			//0x004C
	DWORD              Size;				//0x0050
	DWORD              Unknown0x0050;		//0x0054
	TArray<BYTE>       Script;				//0x0058
	char               Unknown0x0060[0x02C]; //(11)
};


struct UFunction : UStruct 
{
	DWORD              Flags;				//0x0090
	WORD               NativeIndex;			//0x0094
	WORD               ReplicationOffset;	//0x0096
	DWORD              Unknown0x094;		//0x0098
	DWORD              Unknown0x098;		//0x009C
	DWORD              pExecFunction;		//0x00A0
};


struct UState : UStruct 
{
};


struct UClass : UState 
{
};


//Viewport->Actor->XLevel->ActorArray

// WindowsViewport WindowsClient0.WindowsViewport0
struct UViewport : UObject
{
	char				Unknown0x0008[0x008];
	struct AActor *		Actor;				//0x003C
};
// Lobby.LineagePlayerController1
struct AActor : UObject
{
	//char				Unknown0x00b0[0xB0]; CT1 3 Feb 2008
	//char				Unknown0x00a0[0xA0]; CT1.5
	char				Unknown0x00a0[0xA4]; // CT2.0
	struct ULevel *		XLevel;				//0x0E4
};
struct ULevel : UObject
{
	char				Unknown0x0001;		//0x0038
	TArray<UObject *>	ActorArray;			//0x003C
};


#pragma pack(4)

struct User
{
public:
	int	SummonedID;			//	0
	int	PetID;				//	4
	int	Unknown2;			//	8 (Something to do with ItemSlotType)
	int	Unknown3;			//	C
	int	bCanBeAttacked;		//	10
	int	NPCID;				//	14
	int	Class;				//	18 (NPC_ID)
	TCHAR	Name[96];		//	CT1 3 Feb 2008
	//TCHAR	Name[48];		//	1C as a Unicode / Wide string char [?] e.g. :
							// "O.b.s.i.d.i.a.n. .G.o.l.e.m..."
							// "M.u.r.d.o.c..."
							// "A.d.v.e.n.t.u.r.e. .G.u.i.l.d.s.m.a.n..."
							// "E.l. d.e. r. . L.o. n.g. t.a. i.l.  .K. e.l. t.i. r..."
	int	Race;				//	Race (Human=0, Demon=1, Dark Elf=2, Elf=3, Dwarf=4) 
	int	Gender;				//	50	(Something to do with MeshType)
	int	ClassType;			//	54	(Something to do with MeshType)
	int	Level;				//	58
	int	ExpLow;				//	58
	int	ExpHigh;				//	58
	//__int64	Exp;			//	5C..60
	int	Str;				//	64
	int	Dex;				//	68
	int	Con;				//	6C
	int	Int;				//	70
	int	Wit;				//	74
	int	Men;				//	78
	int	MaxHP;				//	7C
	int	HP;					//	80
	int	MaxMP;				//	84
	int	MP;					//	88
	int	CarryWeight;		//	8C
	int	AttackRange;		//	90
	int	bTransformed;			//	94 01
	int	EQUIPITEM_Underwear;	//	98  (Something to do with ItemSlotType 1)
	int	EQUIPITEM_LEar;			//	9C  (Something to do with ItemSlotType 2)
	int	EQUIPITEM_REar;			//	A0  (Something to do with ItemSlotType 3)
	int	EQUIPITEM_Neck;			//	A4  (Something to do with ItemSlotType 4)
	int	EQUIPITEM_RFinger;		//	A8  (Something to do with ItemSlotType 5)
	int	EQUIPITEM_LFinger;		//	AC  (Something to do with ItemSlotType 6)
	int	EQUIPITEM_Head;			//	B0  (Something to do with ItemSlotType 7)
	int	EQUIPITEM_RHand;		//	B4  (Something to do with ItemSlotType 8)
	int	EQUIPITEM_LHand;		//	B8  (Something to do with ItemSlotType 9)
	int	EQUIPITEM_Gloves;		//	BC  (Something to do with ItemSlotType 10)
	int	EQUIPITEM_Chest;		//	C0  (Something to do with ItemSlotType 11)
	int	EQUIPITEM_Legs;			//	C4  (Something to do with ItemSlotType 12)
	int	EQUIPITEM_Feet;			//	C8  (Something to do with ItemSlotType 13)
	int	Unknown51;				//	CC  (Something to do with ItemSlotType 14) Tiara?
	int	Unknown52;				//	D0  (Something to do with ItemSlotType 15) Weapon
	int	Unknown53;				//	D4  (Something to do with ItemSlotType 16)
	int	Unknown54;				//	D8  (Something to do with ItemSlotType 17)
	int	Unknown55;				//	DC  (Something to do with ItemSlotType 18) Hair
	int	Unknown56;				//	E0  (Something to do with ItemSlotType 19) Hair 2
	int	Unknown57;			//	E4
	int	Unknown58;			//	E8
	int	Unknown59;			//	EC
	int	Unknown60;			//	F0
	int	Unknown61;			//	F4
	int	Unknown62;			//	F8
	int	Unknown63;			//	FC
	int	Unknown64;			//	100
	int	Unknown65;			//	104
	int	Unknown66;			//	108
	int	Unknown67;			//	10C
	int	Unknown68;			//	110
	int	Unknown69;			//	114
	int	Unknown70;			//	118
	int	Unknown71;			//	11C
	int	Unknown72;			//	120
	int	Unknown73;			//	124
	int	Unknown74;			//	128
	int	Unknown75;			//	12C
	int	Unknown76;			//	130
	int	Unknown77;			//	134
	int	Unknown78;			//	138
	int	Unknown79;			//	13C
	int	Unknown80;			//	140
	int	Unknown81;			//	144
	int	Unknown82;			//	148
	int	Unknown83;			//	14C
	int	Unknown84;			//	150
	int	Unknown85;			//	154
	int	Unknown86;			//	158
	int	Unknown87;			//	15C
	int	Unknown88;			//	160
	int	Unknown89;			//	164
	int	Unknown90;			//	168
	int	Unknown91;			//	16C
	int	Unknown92;			//	170
	int	Unknown93;			//	174
	int	Unknown94;			//	178
	int	Unknown95;			//	17C
	int	Unknown96;			//	180
	int	Unknown97;			//	184
	int	Unknown98;			//	188
	int	Unknown99;			//	18C
	TCHAR	Unknown100[96];		//	CT1 3 Feb 2008
	int	NickColor;			//	190
	int	Guilty;				//	194
	int	CriminalRate;		//	198
	int	Unknown103;			//	19C
	int	Unknown104;			//	1A0 64
	int	Unknown105;			//	1A4 9 
	int	Unknown106;			//	1A8 64
	int	Unknown107;			//	1AC 9 
	int	Unknown108;			//	1B0 64
	int	Unknown109;			//	1B4 9 
	int	Unknown110;			//	1B8 64
	int	Unknown111;			//	1BC c0000000 float
	int	Unknown112;			//	1C0 caa1fb3f
	int	Unknown113;			//	1C4 00000060 float
	int	Unknown114;			//	1C8 5555f13f
	float	Unknown115;		//	1CC 00001041 float
	float	Unknown116;		//	1D0 00009041 float
	int	Unknown117;			//	1D4	initialised to 0
	int	Unknown118;			//	1D8
	int	Unknown119;			//	1DC
	int	Unknown120;			//	1E0	initialised to 0
	int	Unknown121;			//	1E4
	int	Unknown122;			//	1E8
	int	Unknown123;			//	1EC	initialised to 0
	int	Unknown124;			//	1F0
	int	Unknown125;			//	1F4	initialised to 0
	int	Unknown126;			//	1F8
	int	Unknown127;			//	1FC
	int	Unknown128;			//	200
	void * Pawn;			//	204
	int	Unknown130;			//	208
	int	Unknown131;			//	20C
	//int	Unknown132;			//	210 //	CT1 3 Feb 2008
	int	CarryingWeight  ;	//	214
	int	SP;					//	218
	int	HitRate;			//	21C (Accuracy)
	int	CriticalRate;		//	220 (Crit. Rate)
	int	PhysicalAttack;		//	224
	int	PhysicalAttackSpeed;	//	228 (Atk. Spd.)
	int	PhysicalDefence;	//	22C
	int	PhysicalAvoid;		//	230
	int	MagicalAttack;		//	234
	int	MagicDefense;		//	238
	int	MagicCastingSpeed;	//	23C (Casting Spd.)
	int	Unknown144;			//	240 02000000
	int	Unknown145;			//	244 02000000
	int	Unknown146;			//	248 01000000
	int	Unknown147;			//	24C
	int	Unknown148;			//	250
	//TCHAR	NickName[96];	//	CT1 3 Feb 2008
	TCHAR	NickName[48];   //	254 
							//  "W.a.r.e.h.o.u.s.e. .F.r.e.i.g.h.t.m.a.n.."
							//  "G.r.a.y. .P.i.l.l.a.r. .M.e.m.b.e.r..."
							//  "P.r.i.e.s.t.e.s.s. .o.f. .t.h.e. .E.a.r.t.h..."
	int	PledgeID;			//	284
	int	Unknown162;			//	288
	int	Unknown163;			//	28C
	int	Unknown164;			//	290
	int	Pledge;				//	294
	int	Unknown166;			//	298	(Something to do with PledgePower)
	int	SurrenderWarID;		//	29C
	int	Unknown168;			//	2A0
	int	Unknown169;			//	2A4
	int	Unknown170;			//	2A8 01000000
	int	Unknown171;			//	2AC d7010000
	int	PrivateStoreState;	//	2B0
	int	PKCount;			//	2B4
	int	PvPCount;			//	2B8
	int	Unknown175;			//	2BC
	int	ActiveClassType;		//	2C0
	int	MaxCP;				//	2C4
	int	CP;					//	2C8
	int	Unknown179;			//	2CC
	int	Unknown180;			//	2D0
	int	Unknown181;			//	2D4
	int	Unknown182;			//	2D8	(Something to do with PledgePower) 	initialised to 0 (bytes)
	int	Unknown183;			//	2DC	(Something to do with PledgePower) 	initialised to 0 (bytes)
	int	Unknown184;			//	2E0 03000000
	int	Unknown185;			//	2E4 64000000
	int	Unknown186;			//	2E8
	int	Unknown187;			//	2EC
	int	Unknown188;			//	2F0
	int	Unknown189;			//	2F4
	int	Unknown190;			//	2F8
	int	Unknown191;			//	2FC
	int	Unknown192;			//	300
	int	Unknown193;			//	304
	int	UniqueNameColor;	//	308	initialised to 0FFFFFFFFh
	int	Unknown195;			//	30C
	int	Unknown196;			//	310	rep movsd 0C5h = 197 DWORDS	
};

int lua_LineageIIopen (struct lua_State *L);
