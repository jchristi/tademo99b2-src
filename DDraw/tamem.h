#ifndef tamemH
#define tamemH

#include <dsound.h>

#pragma pack(1)

struct PlayerStruct;
struct PlayerInfoStruct;
struct UnitStruct;
struct UnitOrdersStruct;
struct WeaponStruct;
struct MapFileStruct;
struct UnitDefStruct;
struct GafAnimStruct;
struct Object3doStruct;
struct PrimitiveStruct;
struct PrimitiveInfoStruct;
struct ProjectileStruct;
struct FeatureDefStruct;
struct FXGafStruct;
struct FeatureStruct;
struct WreckageInfoStruct;
struct DebrisStruct;
struct Unk1Struct;
struct Point3;
struct SmokeListNode;
struct ParticleSystemStruct;
struct SmokeListNode;
struct ParticleBase;
struct SmokeGraphics;
struct RadarPicStruct;
struct DSoundStruct;

struct Point3{
	int x;
	int y;
	int z;
};

struct PlayerStruct{
  char PlayerActive;
  char data1[11];
  char PlayerNum;
  char data2[26];
  PlayerInfoStruct *PlayerInfo;
  char Name[30];
  char SecondName[30];
  UnitStruct *Units; //0x67
  UnitStruct *NextUnits;
  char data5[213];
  short NumUnits;
  char data6[5];
}; //0x14b

struct WeaponStruct {
  char WeaponName[0x20];
  char WeaponDescription[0x40];
  int Unkn1;
  char data3[20];
  GafAnimStruct *LandExplodeAsGFX;
  GafAnimStruct *WaterExplodeAsGFX;
  char data4[86];
  short Damage; //d6
  short AOE;
  float EdgeEffectivnes;
  short Range;
  char data5[21];
  float ShakeMagnitude;
  char data6[17];
  unsigned char ID;
  char data8[1];
  char RenderType;
  char data7[8];
};  //0x115

struct ExplosionStruct{
	DebrisStruct *Debris;
	short Frame;
	char data2[6];
	FXGafStruct *FXGaf;
	char data3[12];
	int XPos;
	int ZPos;
	int YPos;
	char data4[36];
	short XTurn; //0x4c
	short ZTurn;
	short YTurn;
	char data5[2];
};//0x54

struct DebrisStruct{
	char data1[0x24];
	Point3 *Vertices;
	Unk1Struct *Unk;
	char data2[8];
};//0x34

struct Unk1Struct{
	char data1[0x18];
	FXGafStruct *Texture;
	char data2[0x4];
};//0x20

struct FXGafStruct{
	short Frames;
	char data1[6];
	char Name[0x20];
	int FramePointers[1];
};

struct DSoundStruct{
	char data1[0x24];
	LPDIRECTSOUND Directsound;
	LPDIRECTSOUNDBUFFER DirectsoundBuffer;
	char data2[0xC];
	LPDIRECTSOUNDBUFFER DirectsoundBuffer2;
};

struct TAdynmemStruct{
	char data21[0x10];
	DSoundStruct *DSound;
	char data1[0x1B4F];

	PlayerStruct Players[10];	//0x1B63 , end at 0x2851

	char data4[0x43D];
	short BuildPosX; //0x2C8E
	short BuildPosY;
	int BuildPosRealX; //0x2C92
	int Height;
	int BuildPosRealY;
	int unk1;
	int Height2;

	char data22[0x6];

	short MouseMapPosX;  //0x2CAC
	char data16[6];
	short MouseMapPosY;  //0x2CB4
	char data23[4];
	unsigned short MouseOverUnit; //0x2CBA
	char data17[0x8];
	short BuildNum;  //0x2CC4,  unitindex for selected unit to build
	char BuildSpotState; //0x40=notoktobuild
	char data18[0x2C];

	WeaponStruct Weapons[256];  //0x2CF3  size=0x11500
	//char data7[4];
	int NumProjectiles;
	ProjectileStruct *Projectiles; //0x141F7
	char data13[0x10];
	WreckageInfoStruct *WreckageInfo; //0x1420B
	char data14[0x24];
	int FeatureMapSizeX; //0x14233
	int FeatureMapSizeY; //0x14237
	char data7[0x18];
	int NumFeatureDefs;
	char data15[0x18];
	FeatureDefStruct *FeatureDef; //0x1426F
	char data8[8];
	LPVOID	*EyeBallMemory;  //0x1427B
	char data12[8];
	FeatureStruct *Features; //0x14287
	char data3[0x40];
	tagRECT MinimapRect;//0x142CB
	RadarPicStruct *RadarFinal; //0x142DB
	RadarPicStruct *RadarMapped;  //0x142DF
	RadarPicStruct *RadarPicture;  //0x142E3
	char data20[4];
	short RadarPicSizeX;  //0x142EB
	short RadarPicSizeY;  //0x142ED
	char data25[4];
	int CirclePointer;//0x142F3 //used in drawcircle funktion
	char data19[0x28];
	int MapX;	//0x1431f
	int MapY;   //0x14323
	int MapXScrollingTo; //0x14327
	int MapYScrollingTo; //0x1432B
	char data24[0x28];
	UnitStruct *Units; //0x14357
	LPVOID Unk1;
	short *HotUnits;//0x1435F
	short *HotRadarUnits;
	int NumHotUnits; //0x14367
	int NumHotRadarUnits;
	char data5[0x2c];
	UnitDefStruct *UnitDef;  //0x1439b
	char data11[0x57C];
	int NumExplosions; //0x1491B
	//char data9[0x6270];
	ExplosionStruct Explosions[300]; //0x1491F
	LPVOID Unk2; //0x1AB8F
	char data10[0x1DEB4];
	int GameTime; //0x38A47
	char data6[0x79E];
	MapFileStruct *MapFile; //0x391E9
};

struct WreckageInfoStruct{
	int unk1;
	LPVOID unk2;
	int XPos;
	int ZPos;
	int YPos;
	char data1[0xC];
	short ZTurn;
	short XTurn;
	short YTurn;
	char data2[0xA];
};

struct FeatureStruct{
	char data1[8];
	short FeatureDefIndex;
	short WreckageInfoIndex;
	char data2[1];
}; //0xD

struct FeatureDefStruct {
	char Name[0x20];
	char data1[0x60];
	char Description[20];
	char Data2[108];
}; //0x100

struct ProjectileStruct {
	WeaponStruct *Weapon;
	int XPos;
	int ZPos;
	int YPos;
	int XPosStart;
	int ZPosStart;
	int YPosStart;
	int XSpeed;
	int ZSpeed;
	int YSpeed;
	char data1[14];
	short XTurn;
	short ZTurn;
	short YTurn;
	char data2[45];
	struct {
		bool unk1 : 1;
		bool Inactive : 1;
		char unk2 : 6;
	} Inactive;
	char data3[1];
}; //0x6B

struct MapFileStruct{
	char data[0x204];
	char TNTFile[MAX_PATH];
};

struct PlayerInfoStruct{
	char MapName[0x20];
	char data1[0x76];
    char PlayerColor;
};

struct UnitDefStruct {
  char Name[0x20];
  char UnitName[0x20];
  char UnitDescription[0x40];
  char ObjectName[0x20];
  char Side[3];
  char data5[0xA7];
  short FootX;  //0x14A
  short FootY;  //0x14C
  char *YardMap;
  char data6[0x6E];
  WeaponStruct *ExplodeAs;
  WeaponStruct *SelfeDestructAs;
  char data7[0x81];
}; //0x249

struct UnitStruct {
  int IsUnit;
  char data1[12];
  WeaponStruct *Weapon1;
  char data2[11];
  char Builder;
  char data3[12];
  WeaponStruct *Weapon2;
  char data4[24];
  WeaponStruct *Weapon3;
  char data5[16];
  UnitOrdersStruct *UnitOrders;  //5c
  char UnitState;
  char data6[3];
  unsigned short ZTurn;
  unsigned short XTurn;
  unsigned short YTurn;
  int XPos; //0x6A
  int ZPos;
  int YPos;
  short XGridPos;
  short YGridPos;
  short XLargeGridPos;
  short YLargeGridPos;
  char data8[4];
  LPVOID UnkPTR1;
  char data15[8];
  UnitStruct *FirstUnit; //?
  UnitDefStruct *UnitType; //0x92
  PlayerStruct *Owner; //?
  LPVOID UnkPTR2;
  Object3doStruct *Object3do;
  char data9[22];
  short Kills;
  char data17[50];
  PlayerStruct *Owner2; //?
  char data16[6];
  char HealthPerA;  //health in percent
  char HealthPerB;  //health in percent, changes slower (?)
  char data19[2];
  unsigned char RecentDamage;  //0xFA
  unsigned char Height;
  char data10[8];
  int Nanoframe;
  short Health;
  char data14[6];
  char UnitSelected;
  char data11[7];
}; //0x118

struct Object3doStruct {
	short NumParts;
	char data1[2];
	int TimeVisible;
	char data2[4];
	UnitStruct *ThisUnit;
	LPVOID *UnkPTR1;
	LPVOID *UnkPTR2;
	char data3[6];
	PrimitiveStruct *BaseObject;

};

struct PrimitiveInfoStruct{
	char data1[28];
	char *Name;

};

struct PrimitiveStruct{
	PrimitiveInfoStruct *PrimitiveInfo;
	int XPos;
	int ZPos;
	int YPos;
	unsigned short XTurn;
	unsigned short ZTurn;
	unsigned short YTurn;
	char data3[18];
	struct {
	bool Visible: 1;
    bool unk1 : 7;
	} Visible;
	char data2[1];
	PrimitiveStruct *SiblingObject;
	PrimitiveStruct *ChildObject;
	PrimitiveStruct *ParrentObject;
}; //0x36


struct UnitOrdersStruct {
  char data1[30];
  UnitOrdersStruct *ThisPTR;
  //char data2[2];
  int PosX;
  int PosZ;
  int PosY;
  char data3[6];
  char FootPrint;
  char data4[12];
  struct {
    char unk1 : 4;
    bool ThisStart : 1;
    char unk2 : 3;
  } ThisStart;
  char data5[6];
  UnitOrdersStruct *NextOrder;
};

struct GafAnimStruct {


};

struct RadarPicStruct{
	int XSize;
	int YSize;
	int Unk1;
	LPVOID *PixelPTR;
};

struct ParticleSystemStruct{
	LPVOID DrawFunc; //4FD5F8 wake or smoke?; 4FD638 - Smoke1, 4FD618 - Smoke2; 4FD5B8, 4FD5A8 - Nanolath; 4FD5D8 - fire; //?
	char data1[8];
	int Type; //1 smoke, 2 wake, 6 nano, 7 fire
	SmokeGraphics* firstDraw;		//0 om denna partikel ej är aktiv (?)
	SmokeGraphics* lastDraw;			//rita alla fram till men inte denna ?
	LPVOID *Unk;				//? inte för sista ?
	char data2[48];
};//76

struct ParticleBase{
	char data[8];
	ParticleSystemStruct **particles;		
	char data2[8];
	ParticleSystemStruct **ParticlePTRArray;
	int SmokeParticleStructSize; //? 76
	int maxParticles; //? 1000
	int curParticles;			//antalet aktiva i arrayen men de är inte nödvändigtvis i ordning
};

struct SmokeGraphics{
	FXGafStruct* gaf;
	int XPos,ZPos,YPos;
	int unknown;
	int frame;
	int unknown2;
	int MoreSubs;  //0 ifall inga fler subparticles efter denna
};//0x20

/*struct SmokeListNode{
	SmokeParticleStruct* next;
	SmokeParticleStruct* me;
};*///?

#pragma pack()

#endif
