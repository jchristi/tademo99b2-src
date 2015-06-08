unit idplay;

interface

uses
  DPlay, Windows,packet,Logging,log2, DPLobby,textdata,cstream,classes, registry;

const
  VERSION = '0.99ß2';
  INTERNALVER = 3;  // 0 = ingen/jättegammal 1 = 97 2 = 98/99.b1 3 = 99b2
  UNITSPACE = 5000;

const
  NumDeities = 13;
  Deities :array[1..NumDeities] of string = ('Allah', 'Shiva', 'Odin', 'Zeus', 'Jehova', 'Buddha', 'Zarathustra', 'Thor', 'Mammon', 'Uncle Sam', 'Ra', 'Bill Gates', 'Sugar the Snow Fairy');

type
  Tbuilding = record
    buildid     :word;
    posx        :smallint;
    posy        :smallint;
    hp          :word;
  end;

type
  MKChatMem = record
    chat           :array [1..100] of char;
    dataexists     :integer;
    deathtimes     :array [1..10] of integer;
    tastatus       :integer;
    playernames    :array [1..10,1..20] of char;
    incomeM        :array [1..10] of single;
    incomeE        :array [1..10] of single;
    totalM         :array [1..10] of single;
    totalE         :array [1..10] of single;
    playingDemo    :integer;
    allies         :array [1..10] of integer;
    yehaplayground :array [1..10] of integer;
    storedM        :array [1..10] of single;
    storedE        :array [1..10] of single;
    storageM       :array [1..10] of single;
    storageE       :array [1..10] of single;
    ehaWarning     :integer;
    ehaOff         :integer;
    toAllies       :array [1..100] of char;
    toAlliesLength :integer;
    fromAllies     :array [1..100] of char;
    fromAlliesLength:integer;
    mapX           :integer;
    mapY           :integer;
    otherMapX      :array [1..10] of integer;
    otherMapY      :array [1..10] of integer;
    F1Disable      :integer;
    commanderWarp  :integer;
    mapname        :array [1..100] of char;
    myCheats       : integer;
    playerColors   : array [1..10] of integer;
    lockviewon     : integer;
    unitCount      : integer;
    ta3d           :integer;
  end;

type
 PMKChatMem = ^MKChatMem;

{const
     SY_UNIT = $ebc53551;}

var
  startedfrom :string;
type statstart = procedure (numplayers:dword;maxunits:dword) ; stdcall;
type statnewunit = procedure (unitid:word;netid:word;tid:dword) ; stdcall;
type statunitfinished = procedure (unitid:word;tid:dword) ; stdcall;
type statdamage = procedure (receiver:word;sender:word;amount:word;tid:dword); stdcall;
type statkill = procedure (killed:word;killer:word;tid:dword) stdcall;
type statstat = procedure (player:dword;mstored:single;estored:single;mstorage:single;estorage:single;mincome:single;eincome:single;tid:dword) stdcall;

type
  TDPlay = class ({TObject} TInterfacedObject, IDirectPlay,IDirectPlay2, IDirectPlay3)
  private
    dp1 :IDirectPlay;
    dp3 :IDirectPlay3;

    asmstorage   : array[1..1000] of char;
    expcrc       : integer;
    crcattempts  : integer;
    tal          : integer;
    tastatus     : integer;
    numplayers   : integer;
    filterpack   : byte;
//yxan cmd cont varning
    yxdetected   : boolean;
    cmdcont      : boolean;
    playername   : array [1..10] of string;
    playerip     : array [1..10] of string;
    playerId     : array [1..10] of dword;
    playerside   : array [1..10] of byte;
    playercolor  : array [1..10] of byte;
    cantake      : array [1..10] of boolean;
    laststatmess : array [1..10] of string;
    lastmsg      : array [1..10] of integer;
    startid      : array [1..10] of integer;
    incomem      : array [1..10] of single;
    incomee      : array [1..10] of single;
    sharedm      : array [1..10] of single;
    sharede      : array [1..10] of single;
    lastsharedm  : array [1..10] of single;
    lastsharede  : array [1..10] of single;
    lasttotalm   : array [1..10] of single;
    lasttotale   : array [1..10] of single;
    totalsharedm : array [1..10] of single;
    totalsharede : array [1..10] of single;
    laststat     : array [1..10] of dword;
    lastdead     : array [0..UNITSPACE] of integer;
    health       : array [0..UNITSPACE] of integer;
    donestatus   : array [0..UNITSPACE] of word;
    unitalive    : array [0..UNITSPACE] of boolean;
    numunitsalive: array [0..10] of integer;
    initbase     : array [0..UNITSPACE div 5] of Tbuilding;
    sentpings    : array [0..101] of dword;
    goodpackets  : array [1..10] of integer;
    lostpackets  : array [1..10] of integer;
    lastpacket   : array [1..10] of dword;
    lateplhist   : array [1..10,0..101] of boolean;
    plhist       : array [1..10,0..11] of dword;
    plhistindex  : array [1..10] of integer;
    losscount    : array [1..10] of integer;
    maxloss      : array [1..10] of integer;
    votedgo      : array [1..10] of boolean;
    clickedin    : array [1..10] of boolean;
    enemychat    : array [1..10] of boolean;
    recConnect   : array [1..10] of boolean;
    internVer    : array [1..10] of integer;
    hasWarned    : array [1..10] of boolean;
    doingtake    : array [1..10] of boolean;
    fastspeed,slowspeed : integer;
    forcego      : boolean;
    pingtimer    : integer;
    startx       : array [1..10] of word;
    starty       : array [1..10] of word;
    startz       : array [1..10] of word;
    givebase     : array [1..10] of boolean;
    warpdone     : array [1..10] of boolean;
    basecount    : integer;
    curbuilding  : integer;
    buildstatus  : integer;
    takefrom     : cardinal;
    takeunit     : integer;
//    taid         : array [1..10] of byte;
    servernumber : integer;
    imserver     : boolean;
    filename     : string;
    mapname      : string;
    maxunits     : word;
//    packetwaiting: boolean;
    onlyunits    : boolean;
    datachanged  : boolean;
    dtfound      : boolean;
//    holdstring   : string;
    islogging    : boolean;
    waitsync     : integer;
    fixfacexps   : boolean;
    protectdt    : boolean;
    fixon        : boolean;
    baseon       : boolean;
    fakecd       : boolean;
    fakewatch    : boolean;
    logpl        : boolean;
    prevtime     : integer;
    starttime    : integer;
    unitdata     : string;
    chatlog      : string;

    lastserial   : array[1..10] of longword;
    curspeeds    : array[1..10] of integer;
    otherCheats  : array[1..10] of integer;
    oldMyCheats  : integer;
    stringbuf    : TStringList;
    adjustcount  : integer;
    notime       : boolean;
    nonserverstart:boolean; //är detta ett tidigare inspelat spel ?
    use3d        : boolean;
    auto3d       : boolean;
    usecomp      : boolean;
    autorec      : boolean;
    playernames  : boolean;
    shareMapPos  : boolean;
    createTxtFile: boolean;
    compatibleTA : boolean; //ta som vi känner minnesstrukturen för ?

    ai           : string;
    serverdir    : string;
    demodir      : string;

    crash        : boolean;
    alloff       : boolean;
    killunits    : string;
    norecord     : boolean;
    statdll      : HModule;
    staton       : boolean;
    procstart    : statstart;
    procnewunit  : statnewunit;
    procunitfinished  : statunitfinished;
    procdamage   : statdamage;
    prockill     : statkill;
    procstat     : statstat;
    hMemMap      :Cardinal;
    nextCheatCheck :integer;
//    tempStorage  :array[1..100] of char;
//    tempStorageLength:integer;
    tempstorage  :TStringList;

    sharingLos   :array [1..10] of bool;
    chatsent,ressent : boolean;

 {  function CreateGroup(var lpidGroup: TDPID; lpGroupName: PDPName;
      const lpData; dwDataSize, dwFlags: DWORD): HResult;
    function Receive(var lppidFrom, lppidTo: TDPID; dwFlags: DWORD;
      var lpvBuffer; var lpdwSize: DWORD): HResult;
    function Send(idFrom, lpidTo: TDPID; dwFlags: DWORD; const lpData;
      lpdwDataSize: DWORD): HResult;}
    public
    constructor Create (realdp :IDirectPlay);

    function AddPlayerToGroup(pidGroup: TDPID; pidPlayer: TDPID) : HResult; overload; stdcall;
    function Close: HResult; overload; stdcall;
    function CreatePlayer(var lppidID: TDPID; lpPlayerFriendlyName: PChar;
        lpPlayerFormalName: PChar; lpEvent: PHandle) : HResult; overload; stdcall;
    function CreateGroup(var lppidID: TDPID; lpGroupFriendlyName: PChar;
        lpGroupFormalName: PChar) : HResult; overload; stdcall;

    function DeletePlayerFromGroup(pidGroup: TDPID; pidPlayer: TDPID) : HResult; overload; stdcall;
    function DestroyPlayer(pidID: TDPID) : HResult; overload; stdcall;
    function DestroyGroup(pidID: TDPID) : HResult; overload; stdcall;
    function EnableNewPlayers(bEnable: BOOL) : HResult; stdcall;
    function EnumGroupPlayers(pidGroupPID: TDPID; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumGroups(dwSessionID: DWORD; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumPlayers(dwSessionId: DWORD; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumSessions(const lpSDesc: TDPSessionDesc; dwTimeout: DWORD;
        lpEnumSessionsCallback: TDPEnumSessionsCallback; lpContext: Pointer;
        dwFlags: DWORD) : HResult; overload; stdcall;
    function GetCaps(const lpDPCaps: TDPCaps) : HResult; overload; stdcall;
    function GetMessageCount(pidID: TDPID; var lpdwCount: DWORD) : HResult; overload; stdcall;
    function GetPlayerCaps(pidID: TDPID; const lpDPPlayerCaps: TDPCaps) :
        HResult; overload; stdcall;
    function GetPlayerName(pidID: TDPID; lpPlayerFriendlyName: PChar;
        var lpdwFriendlyNameLength: DWORD; lpPlayerFormalName: PChar;
        var lpdwFormalNameLength: DWORD) : HResult; overload; stdcall;
    function Initialize(const lpGUID: TGUID) : HResult; overload; stdcall;
    function Open(const lpSDesc: TDPSessionDesc) : HResult; overload; stdcall;
    function Receive(var lppidFrom, lppidTo: TDPID; dwFlags: DWORD;
        var lpvBuffer; var lpdwSize: DWORD) : HResult; overload; stdcall;
    function SaveSession(lpSessionName: PChar) : HResult; stdcall;
//    function Send(pidFrom: TDPID; pidTo: TDPID; dwFlags: DWORD;
//        const lpvBuffer; dwBuffSize: DWORD) : HResult; overload; stdcall;
    function SetPlayerName(pidID: TDPID; lpPlayerFriendlyName: PChar;
        lpPlayerFormalName: PChar) : HResult; overload; stdcall;


//    function AddPlayerToGroup(idGroup: TDPID; idPlayer: TDPID) : HResult; overload; stdcall;
//    function Close: HResult; stdcall;
    function CreateGroup(var lpidGroup: TDPID; lpGroupName: PDPName;
        const lpData; dwDataSize: DWORD; dwFlags: DWORD) : HResult; overload; stdcall;
    function CreatePlayer(var lpidPlayer: TDPID; pPlayerName: PDPName;
        hEvent: THandle; lpData: Pointer; dwDataSize: DWORD; dwFlags: DWORD) :
        HResult; overload; stdcall;
//    function DeletePlayerFromGroup(idGroup: TDPID; idPlayer: TDPID) : HResult; overload; stdcall;
//    function DestroyGroup(idGroup: TDPID) : HResult; overload; stdcall;
//    function DestroyPlayer(idPlayer: TDPID) : HResult; overload; stdcall;
    function EnumGroupPlayers(idGroup: TDPID; const lpguidInstance: TGUID;
        lpEnumPlayersCallback2: TDPEnumPlayersCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumGroups(lpguidInstance: PGUID; lpEnumPlayersCallback2:
        TDPEnumPlayersCallback2; lpContext: Pointer; dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumPlayers(lpguidInstance: PGUID; lpEnumPlayersCallback2:
        TDPEnumPlayersCallback2; lpContext: Pointer; dwFlags: DWORD) : HResult; overload; stdcall;
    function EnumSessions(var lpsd: TDPSessionDesc2; dwTimeout: DWORD;
        lpEnumSessionsCallback2: TDPEnumSessionsCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult; overload; stdcall;
    function GetCaps(var lpDPCaps: TDPCaps; dwFlags: DWORD) : HResult; overload; stdcall;
    function GetGroupData(idGroup: TDPID; lpData: Pointer; var lpdwDataSize: DWORD;
        dwFlags: DWORD) : HResult; stdcall;
    function GetGroupName(idGroup: TDPID; lpData: Pointer; var lpdwDataSize: DWORD) :
        HResult; stdcall;
//    function GetMessageCount(idPlayer: TDPID; var lpdwCount: DWORD) : HResult; overload; stdcall;
    function GetPlayerAddress(idPlayer: TDPID; lpAddress: Pointer;
        var lpdwAddressSize: DWORD) : HResult; stdcall;
    function GetPlayerCaps(idPlayer: TDPID; var lpPlayerCaps: TDPCaps;
        dwFlags: DWORD) : HResult; overload; stdcall;
    function GetPlayerData(idPlayer: TDPID; lpData: Pointer; var lpdwDataSize: DWORD;
        dwFlags: DWORD) : HResult; stdcall;
    function GetPlayerName(idPlayer: TDPID; lpData: Pointer; var lpdwDataSize: DWORD)
        : HResult; overload; stdcall;
    function GetSessionDesc(lpData: Pointer; var lpdwDataSize: DWORD) : HResult; stdcall;
//    function Initialize(const lpGUID: TGUID) : HResult; overload; stdcall;
    function Open(var lpsd: TDPSessionDesc2; dwFlags: DWORD) : HResult; overload; stdcall;
    function Receive(var lpidFrom: TDPID; var lpidTo: TDPID; dwFlags: DWORD;
        lpData: Pointer; var lpdwDataSize: DWORD) : HResult; overload; stdcall;
    function Send(idFrom: TDPID; lpidTo: TDPID; dwFlags: DWORD; const lpData;
        lpdwDataSize: DWORD) : HResult; overload; stdcall;
    function SetGroupData(idGroup: TDPID; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult; stdcall;
    function SetGroupName(idGroup: TDPID; lpGroupName: PDPName;
        dwFlags: DWORD) : HResult; stdcall;
    function SetPlayerData(idPlayer: TDPID; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult; stdcall;
    function SetPlayerName(idPlayer: TDPID; lpPlayerName: PDPName;
        dwFlags: DWORD) : HResult; overload; stdcall;
    function SetSessionDesc(const lpSessDesc: TDPSessionDesc2; dwFlags: DWORD) :
        HResult; stdcall;

    {----------------------------------------------------------------}

    (*** IDirectPlay3 methods ***)
    function AddGroupToGroup(idParentGroup: TDPID; idGroup: TDPID) : HResult; stdcall;
    function CreateGroupInGroup(idParentGroup: TDPID; var lpidGroup: TDPID;
        lpGroupName: PDPName; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult; stdcall;
    function DeleteGroupFromGroup(idParentGroup: TDPID; idGroup: TDPID) :
        HResult; stdcall;
    function EnumConnections(const lpguidApplication: TGUID;
        lpEnumCallback: TDPEnumConnectionsCallback; lpContext: Pointer;
        dwFlags: DWORD) : HResult; stdcall;
    function EnumGroupsInGroup(idGroup: TDPID; const lpguidInstance: TGUID;
        lpEnumPlayersCallback2: TDPEnumPlayersCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult; stdcall;
    function GetGroupConnectionSettings(dwFlags: DWORD; idGroup: TDPID;
        lpData: Pointer; var lpdwDataSize: DWORD) : HResult; stdcall;
    function InitializeConnection(var lpConnection: TDPLConnection; dwFlags: DWORD) :
         HResult; stdcall;
    function SecureOpen(const lpsd: TDPSessionDesc2; dwFlags: DWORD;
        const lpSecurity: TDPSecurityDesc; const lpCredentials: TDPCredentials)
        : HResult; stdcall;
    function SendChatMessage(idFrom: TDPID; idTo: TDPID; dwFlags: DWORD;
        const lpChatMessage: TDPChat) : HResult; stdcall;
    function SetGroupConnectionSettings(dwFlags: DWORD; idGroup: TDPID;
        const lpConnection: TDPLConnection) : HResult; stdcall;
    function StartSession(dwFlags: DWORD; idGroup: TDPID) : HResult; stdcall;
    function GetGroupFlags(idGroup: TDPID; var lpdwFlags: DWORD) : HResult; stdcall;
    function GetGroupParent(idGroup: TDPID; var lpidParent: TDPID) : HResult; stdcall;
    function GetPlayerAccount(idPlayer: TDPID; dwFlags: DWORD; var lpData;
        var lpdwDataSize: DWORD) : HResult; stdcall;
    function GetPlayerFlags(idPlayer: TDPID; var lpdwFlags: DWORD) : HResult; stdcall;

    function IsSelf(id:TDPID) :boolean;
    function IsServer(id:TDPID) :boolean;
    function convertid(id:TDPID) :byte;
    function smartpak(c:string;from:TDPid) :string;
    function packetHandler(d : String;var from , till : TDPID) :string;
    function getcrc() :integer;
    procedure processcrc(s:string);
    procedure sendchat(s:string);
    procedure handleunitdata(s : string;from , till :TDPID);
    procedure handlechatmsg(s : string;from :TDPID);
    function facexpshandler(s:string;from:TDPid;crccheck:boolean):string;
    procedure createlogfile();
    procedure dobase(till:dword);
    procedure initfastbase(namn:string);
    procedure initbuilding(num,id:word;xpos,ypos:smallint;hps:word);
    procedure setword(p:pointer;w:word);
    procedure setdword(p:pointer;d:dword);
    procedure plHandler(i:dword;from: integer);
    procedure mkchat();
    procedure checkForCheats();
    procedure sendDDrawMsg();
    procedure sendRec2RecMsg(msg:string);
    procedure exiting();
    procedure unitcountchange(player:dword;amount: integer);

    procedure SendLocal (msg :string; dest :TDPid; local, remote :boolean);
    procedure AdjustSpeeds (force :boolean);
    procedure GetRandomMap (fname :string);

    procedure sendchatlocal(s:string);
    procedure ResetRec;

    procedure ExceptMessage;
    function GetGoodSource :integer;
    procedure CalcLosSharing;

    function IsTAHookRunning : boolean;
    function TestByte(p : cardinal;b : byte) : boolean;
{    function QueryInterface(const IID: TGUID; out Obj): HResult; stdcall;
    function _AddRef: Integer; stdcall;
    function _Release: Integer; stdcall;}

  end;

type
  TLobby = class (TInterfacedObject, IDirectPlayLobby, IDirectPlayLobby2)
  private
    lobby1 :IDirectPlayLobby;
    lobby2 :IDirectPlayLobby2;

  public
    constructor Create (reallobby :IDirectPlayLobby);

    (*** IDirectPlayLobby methods ***)
    function Connect(dwFlags: DWORD; var lplpDP: IDirectPlay2;
        pUnk: IUnknown) : HResult; stdcall;
    function CreateAddress(const guidSP, guidDataType: TGUID; const lpData;
        dwDataSize: DWORD; var lpAddress; var lpdwAddressSize: DWORD) : HResult; stdcall;
    function EnumAddress(lpEnumAddressCallback: TDPEnumAdressCallback;
        const lpAddress; dwAddressSize: DWORD; lpContext : Pointer) : HResult; stdcall;
    function EnumAddressTypes(lpEnumAddressTypeCallback:
        TDPLEnumAddressTypesCallback; const guidSP: TGUID; lpContext: Pointer;
        dwFlags: DWORD) : HResult; stdcall;
    function EnumLocalApplications(lpEnumLocalAppCallback:
        TDPLEnumLocalApplicationsCallback; lpContext: Pointer; dwFlags: DWORD)
        : HResult; stdcall;
    function GetConnectionSettings(dwAppID: DWORD; lpData: PDPLConnection;
        var lpdwDataSize: DWORD) : HResult; stdcall;
    function ReceiveLobbyMessage(dwFlags: DWORD; dwAppID: DWORD;
        var lpdwMessageFlags: DWORD; lpData: Pointer; var lpdwDataSize: DWORD) :
        HResult; stdcall;
    function RunApplication(dwFlags: DWORD; var lpdwAppId: DWORD;
        const lpConn: TDPLConnection; hReceiveEvent: THandle) : HResult; stdcall;
    function SendLobbyMessage(dwFlags: DWORD; dwAppID: DWORD; const lpData;
        dwDataSize: DWORD) : HResult; stdcall;
    function SetConnectionSettings(dwFlags: DWORD; dwAppID: DWORD;
        const lpConn: TDPLConnection) : HResult; stdcall;
    function SetLobbyMessageEvent(dwFlags: DWORD; dwAppID: DWORD;
        hReceiveEvent: THandle) : HResult; stdcall;

    function CreateCompoundAddress(const lpElements: TDPCompoundAddressElement;
        dwElementCount: DWORD; lpAddress: Pointer; var lpdwAddressSize: DWORD) :
        HResult; stdcall;

  end;


var
    logsave         : Tlog2 ;


implementation

uses
 sysUtils;


{--------------------------------------------------------------------}

var
  asmtemp      :integer;
  numSharedLos :integer;
  curLos       :integer;
  shareLosWith : array [1..10] of integer;
  chatview     :PMKChatMem;

function IsSameGuid (g1, t1 :TGuid) :boolean;
var
  i :integer;

begin
  Result := false;
  if g1.d1 <> t1.d1 then exit;
  if g1.d2 <> t1.d2 then exit;
  if g1.d3 <> t1.d3 then exit;
  for i := 0 to 7 do
    if g1.d4[i] <> t1.d4[i] then exit;
  Result := true;
end;

procedure TDPlay.setword(p:pointer;w:word);
var
  wp     :^word;
begin
  wp:=p;
  wp^:=w;
end;
procedure TDPlay.setdword(p:pointer;d:dword);
var
  dp     :^dword;
begin
  dp:=p;
  dp^:=d;
end;

procedure TDPlay.exiting();
var
  LogFile :TextFile;
  a       :integer;
begin
  if not (filename='') and createtxtfile and islogging then begin
    if (ExtractFileExt(filename)='.tad') then
      filename:=copy(filename,1,length(filename)-4);
    filename:=filename+'.txt';

    Assign (LogFile, filename);
    Rewrite (Logfile);
    Write(Logfile,'Num players: '+inttostr(numplayers));
    Writeln(Logfile,'Players in game:');
    for a:=1 to numplayers do
      Writeln(Logfile,playername[a]);
    Writeln(Logfile,'Map played: '+mapname);
    Writeln(Logfile,'Max units: '+inttostr(maxunits));
    Writeln(Logfile,'Date recorded: '+datetostr (now));
    Writeln(Logfile,'Chat msgs sent:');
//    while pos(#13+' ',chatlog)>0 do
  //    insert(#10,chatlog,pos(#13+' ',chatlog));
    Writeln(Logfile,chatlog);
    CloseFile (LogFile);
    filename:='';
  end;

  if assigned(chatview) then
    chatview^.tastatus:=1000;
{    UnmapViewOfFile(chatview);
  end;
  chatview:=NIL;

  if hMemMap <> NULL then
    CloseHandle(hMemMap);
  hMemMap:=NULL;
}end;

procedure TDPlay.sendDDrawMsg();
var
  s     :string;
  a     :integer;
  abo   :string;
begin
  s:=#$fb+'#'+#0;
  for a:=1 to chatview^.toAlliesLength do
    s:=s+chatview^.toAllies[a];
  s[2]:=char(chatview^.toAlliesLength);
  chatview^.toAlliesLength:=0;
  sendRec2RecMsg(s);

//  abo := DataToStr (@s[4], length (s) - 3);
//  sendchat ('Sending ddraw (' + abo + ')');
end;

procedure TDPlay.sendRec2RecMsg(msg:string);
var
  c,s     :string;
  a,b     :integer;
begin
  for a:=2 to numPlayers do
    if recConnect[a] then
      sendlocal(msg,playerId[a],false,true);

  if(isLogging) then begin
  c := SmartPak (msg, playerid[1]);

  if length(c)>1 then
  begin
    s:=#0#0;                                //tom storlek
    a:=GetTickCount;
    b:=a-prevtime;
    prevtime:=a;
    if b<0 then
      b:=b+1000*60*60*24;                   //kl24 fix
    s:=s+char(b and $ff)+char(b shr 8);     //tid sedan senaste
    s:=s+char(1);                            //spelare som sände
    s:=s+c;                                 //paketet
    s[1]:=char(length(s) and $ff);          //fyll i storlek
    s[2]:=char(length(s) shr 8);
    logsave.add(s);                            //write a packet
  end;
  end;
end;

function TDPlay.TestByte(p : cardinal;b : byte) : boolean;
var
  bp     :^byte;
begin
  bp:=pointer(p);
  result:= bp^=b;
end;

procedure TDPlay.CheckForCheats();
var
  a     :integer;
  bp    : ^byte;
begin
  if not TestByte($489CF5,4) then
    chatview^.myCheats:=chatview^.myCheats or 1;  //invulnerabilety

  if not TestByte($401805,$67) or not TestByte($4017E7,$0f) or not TestByte($401808,$76) or not TestByte($40181E,$d9) then
    chatview^.myCheats:=chatview^.myCheats or 2;  //invisible

  if not TestByte($402AD9,1) or not TestByte($402AD9,1) or not TestByte($403EFF,1) or not TestByte($4041F5,1) or not TestByte($4142D2,1) or not TestByte($4146F3,1) then
    chatview^.myCheats:=chatview^.myCheats or 4;  //fast build
  if not TestByte($4018BD,$d8) or not TestByte($4018D9,$d8) then
    chatview^.myCheats:=chatview^.myCheats or 4;  //fast build

  if not TestByte($4018BD,$d8) or not TestByte($4018D9,$d8) then
    chatview^.myCheats:=chatview^.myCheats or 8;  //infinite resource

  if not TestByte($484470,$7d) or not TestByte($4844A9,$0f) or not TestByte($466CCB,$0f) or not TestByte($466C38,$0f) or not TestByte($466D16,$75) or not TestByte($466E31,8) or not TestByte($48BC5E,$1e) then
    chatview^.myCheats:=chatview^.myCheats or $10;  //los+radar

  if not TestByte($457ACF,$74) then
    chatview^.myCheats:=chatview^.myCheats or $20;  //kontroll meny

  if not TestByte($47D4C0,$0f) then
    chatview^.myCheats:=chatview^.myCheats or $40;  //bygg överallt

  if not TestByte($404298,$8a) then
    chatview^.myCheats:=chatview^.myCheats or $80;  //instant capture

  if not TestByte($43cf98,$91) then
    chatview^.myCheats:=chatview^.myCheats or $100;  //special move

  if not TestByte($46704A,$31) or not TestByte($467041,$88) then
    chatview^.myCheats:=chatview^.myCheats or $200;  //jam all

  if not TestByte($499DE7,$c1) then
    chatview^.myCheats:=chatview^.myCheats or $400;  //? damage
end;

procedure TDPlay.mkchat();
var
  s     :string;
  a     :integer;
begin
//  sendchat('MK message');
  for a:=1 to 100 do begin
    if chatview^.chat[a]=#0 then
      break;
    s:=s+chatview^.chat[a];
  end;
  sendchatlocal(s);
  chatview^.dataexists:=0;
end;

procedure TDPlay.SendLocal (msg :string; dest :TDPid; local, remote :boolean);
var
  p :TPacket;
  s :String;
  h :HResult;
begin
  if remote then
  begin
    p := TPacket.SJCreateNew (msg);
    s := p.TaData;
    h := dp3.Send (playerid[1], dest, 1, s[1], length(s));
    p.Free;
  end;

  if local then
  begin
    stringbuf.Add (msg);
    Log.Add ('adding ' + datatostr (@msg[1], length (msg)));
  end;
end;

procedure TDPlay.AdjustSpeeds (force :boolean);
var
//  should :array[1..10] of integer;
  i      :integer;
//  max    :integer;
//  diff   :integer;
begin
  if notime then
    exit;


  SendLocal (#$19#$42 + Char(curspeeds [1]), 0, true, false);
  for i := 2 to numplayers do
  begin
    SendLocal (#$19#$42 + Char(curspeeds [i]), playerid [i], false, true);
  end;
end;

procedure TDPlay.GetRandomMap (fname :string);
var
  maps  :TStringList;
  mf    :TextFile;
  state :integer;
  st    :string;
  nr    :integer;
  part  :string;
  error :integer;
  tot   :integer;
  i     :integer;
begin
  fname := Trim (fname);
  if fname = '' then
    fname := 'maps.txt';

  maps := TStringlist.create;
  if not FileExists (fname) then
  begin
    SendChat ('Unable to open file ' + fname);
    exit;
  end;

  AssignFile (mf, fname);
  Reset (mf);
  state := 0;
  tot := 0;

  while not Eof (mf) do
  begin
    Readln (mf, st);
    st := Trim (st);
    If length (st) = 0 then
      continue;
    if st[1] = ';' then
      continue;

    case state of
      0  :begin   //i början
            if st[1] = '+' then
              state := 1;
          end;
      1  :begin
            part := Copy (st, 1, Pos (' ', st) - 1);
            Val (part, nr, error);
            if error <> 0 then
              state := 0;
            if st[1] = '+' then
              state := 1;

            if error = 0 then
            begin
              maps.AddObject (Copy (st, Pos (' ', st) + 1, 500), pointer (nr));
              tot := tot + nr;
            end;
          end;
    end;
  end;

  if maps.count = 0 then
  begin
    Sendchat ('Could not find any map names to pick from');
    Sendchat ('Used filename: ' + fname);
    exit;
  end;

  nr := Random (tot);
  error := 0;
  st := 'none!! should not happen heh';
  for i := 0 to maps.count - 1 do
  begin
    inc (error, integer(maps.Objects [i]));
    if nr < error then
    begin
      st := maps.strings [i];
      break;
    end;
  end;

  Sendchat ('The randomly selected map is:');
  Sendchat (st + ' (Odds: ' + inttostr (integer(maps.Objects [i])) + ' out of ' + inttostr (tot) + ')');
  CloseFile (mf);
  maps.Free;
end;


procedure TDPlay.initbuilding(num,id:word;xpos,ypos:smallint;hps:word);
begin
  initbase[num].buildid:=id;
  initbase[num].posx:=xpos;
  initbase[num].posy:=ypos;
  initbase[num].hp:=hps;
end;

procedure TDPlay.initfastbase(namn:string);
var
  mf    :TextFile;
  state :integer;
  st    :string;
  nr    :integer;
  part  :string;
  error :integer;
  typ,posx,posy,hp :integer;
  i    :integer;
begin
  namn := Trim (namn);
  if namn='' then begin
    basecount:=15;
    initbuilding(1,$84,-100,140,2500);        //vec
    initbuilding(2,$16,100,140,2500);         //adv vec
    initbuilding(3,$3a,0,40,8000);           //fusion
    initbuilding(4,$57,0,-60,1000);          //moho
    initbuilding(5,$0a,0,-120,4000);          //anti
    initbuilding(6,$44,-100,-200,2500);       //kbot
    initbuilding(7,$08,100,-200,2800);        //advkbot
    initbuilding(8,$40,-220,160,2000);        //hlts
    initbuilding(9,$40,220,160,2000);
    initbuilding(10,$40,-220,-160,2000);
    initbuilding(11,$40,220,-160,2000);
    initbuilding(12,$34,-190,160,1700);       //flakkers
    initbuilding(13,$34,190,160,1700);
    initbuilding(14,$34,-190,-160,1700);
    initbuilding(15,$34,190,-160,1700);

    initbuilding(16,$0112,-100,140,2500);        //vec
    initbuilding(17,$9b,100,140,2500);         //adv vec
    initbuilding(18,$bd,0,60,8000);           //fusion
    initbuilding(19,$da,0,-60,1000);          //moho
    initbuilding(20,$cd,0,-120,4000);          //anti
    initbuilding(21,$ca,-100,-200,2500);       //kbot
    initbuilding(22,$91,100,-200,2800);        //advkbot
    initbuilding(23,$c3,-220,160,2000);        //hlts
    initbuilding(24,$c3,220,160,2000);
    initbuilding(25,$c3,-220,-160,2000);
    initbuilding(26,$c3,220,-160,2000);
    initbuilding(27,$b8,-190,160,1700);       //flakkers
    initbuilding(28,$b8,190,160,1700);
    initbuilding(29,$b8,-190,-160,1700);
    initbuilding(30,$b8,190,-160,1700);
    for i:=1 to 10 do
      givebase[i]:=true;
    sendchat('Standard base initiated .baseoff to disable');
    exit;
  end;

  if not FileExists (namn) then
  begin
    SendChat ('Unable to open file ' + namn);
    exit;
  end;

  AssignFile (mf, namn);
  Reset (mf);
  state := 0;

  while not Eof (mf) do
  begin
    Readln (mf, st);
    Trim (st);
    If length (st) = 0 then
      continue;
    if st[1] = ';' then
      continue;
    if state=0 then begin
      Val (st, basecount, error);
      if error<>0 then begin
        sendchat('Erroneous number of possible buildings');
        exit;
      end;
      state:=1;
      continue;
    end;

    i:= Pos (' ', st);
    part := Copy (st, 1, i-1);
    st:=Copy (st, i+1, 2000);
    Val (part, nr, error);

    if error<>0 then begin
      sendchat('Erroneous base file1');
      exit;
    end;

    i:= Pos (' ', st);
    part := Copy (st, 1, i-1);
    st:=Copy (st, i+1, 2000);
    Val (part, typ, error);

    if error<>0 then begin
      sendchat('Erroneous base file2');
      exit;
    end;

    i:= Pos (' ', st);
    part := Copy (st, 1, i-1);
    st:=Copy (st, i+1, 2000);
    Val (part, posx, error);

    if error<>0 then begin
      sendchat('Erroneous base file3');
      exit;
    end;

    i:= Pos (' ', st);
    part := Copy (st, 1, i-1);
    st:=Copy (st, i+1, 2000);
    Val (part, posy, error);

    if error<>0 then begin
      sendchat('Erroneous base file4');
      exit;
    end;

    i:= Pos (';', st);
    part := Copy (st, 1, i-1);
    st:=Copy (st, i+1, 2000);
    Val (part, hp, error);

    if error<>0 then begin
      sendchat('Erroneous base file5');
      exit;
    end;

    initbuilding(nr,typ,posx,posy,hp);
  end;
  for i:=1 to 10 do
    givebase[i]:=true;
  sendchat('Fast base initiated from '+namn+' .baseoff to disable');
end;

procedure tdplay.CalcLosSharing;
var
  a       :integer;
begin
  numSharedLos:=0;
  if not sharinglos[1] then
    exit;
  for a:=2 to numPlayers do begin
    if sharinglos[a] and (chatview^.allies[a]=1) then begin
      numSharedLos:=numSharedLos+1;
      shareLosWith[numSharedLos]:=a-1;
    end;
  end;
//  sendchat(playername[1]+' sharing los from '+inttostr(numsharedlos)+' players');
end;

//los
procedure EntryLos1;
asm
   mov    curLos,0;
   pushad
   sub    esp,$20;
   push   ebx;
   push   esi;
   mov    eax,$48bae5
   jmp    eax;
end;

procedure ExitLos1;
label
  l1;
asm
   pushad
   mov       eax,[$511de8]
   mov       ebx,curLos
   cmp       ebx,numSharedLos
   jnz       l1
   xor       cl,cl
   mov       [eax+$2a43],cl
   popad
   add       esp,32
   ret
l1:
   lea       edx,[sharelosWith+ebx*4]
   mov       ecx,[edx]
   inc       ebx
   mov       curlos,ebx
   mov       [eax+$2a43],cl
   popad
   mov       asmtemp,eax;
   popad
   pushad
   sub       esp,$20
   push      ebx
   push      esi
   mov       esi,[$511de8]
   mov       eax,asmtemp

   mov       [esp+$0c],eax
   lea       ecx,[esi+$37e27]
   mov       [esp+$08],ecx
   mov       ecx,eax
   mov       eax,[esi+$1435f]
   shl       ecx,1;
   add       eax,ecx
   mov       ecx,$48bb03
   jmp       ecx
end;

//radar
procedure EntryLos2;
asm
   mov    curLos,0;
   pushad
   sub       esp,$28;
   push      ebx
   push      ebp
   mov       eax,$467445
   jmp       eax;
end;

procedure MiddleLos2;
label
  l1;
asm
   test ecx, $10000000
   je        l1
   mov       esi,$4674A9
   jmp       esi;
l1:
   mov       esi,[esp+$10]
   mov       ecx,$4674ff
   jmp       ecx;
end;

procedure MiddleLos22;
label
  l1;
asm
   cmp       curlos,0
   jnz       l1;
   mov       [eax],ecx;
l1:
   mov       esi,[esp+$10]
   mov       ecx,$4674ff
   jmp       ecx;
end;

//radar
procedure ExitLos2;
label
  l1;
asm
   pushad
   mov       eax,[$511de8]
   mov       ebx,curLos
   cmp       ebx,numSharedLos
   jnz       l1
   xor       cl,cl
   mov       [eax+$2a43],cl
   popad
   add       esp,32
   ret
l1:
   lea       edx,[sharelosWith+ebx*4]
   mov       ecx,[edx]
   inc       ebx
   mov       curlos,ebx
   mov       [eax+$2a43],cl
   popad
   popad
   pushad
   sub       esp,$28;
   push      ebx
   push      ebp
   mov       eax,$467445
   jmp       eax;
end;

//los ritning
procedure EntryLos3;
asm
   mov    curLos,0;
   pushad
   sub esp,$20;
   mov edx,[$511de8]
   mov ecx,$4843c9
   jmp ecx
end;

procedure MiddleLos3;
label
  l1;
asm
   je        l1
   cmp       curlos,0
   jnz       l1
   mov       edx,$4844f4
   jmp       edx
l1:
   mov       ebp,$4845a5
   jmp       ebp;
end;

procedure MiddleLos32;
label
  l1;
asm
   cmp       curlos,0
   jnz       l1
   mov       eax,$0f
   repz      stosd
   mov       ecx,edx
   and       ecx,3
   repz      stosb
l1:
   mov       edi,$48441b
   jmp       edi;
end;

procedure ExitLos3;
label
  l1;
asm
   pushad
   mov       eax,[$511de8]
   mov       ebx,curLos
   cmp       ebx,numSharedLos
   jnz       l1
   xor       cl,cl
   mov       [eax+$2a43],cl
   popad
   add       esp,32
   ret
l1:
   lea       edx,[sharelosWith+ebx*4]
   mov       ecx,[edx]
   inc       ebx
   mov       curlos,ebx
   mov       [eax+$2a43],cl
   popad
   popad
   pushad
   sub esp,$20;
   mov edx,[$511de8]
   mov ecx,$4843c9
   jmp ecx
end;

procedure TDPlay.dobase(till:dword);
var
  holdstring :string;
  a          :integer;
begin
  if (playerside[convertid(till)]<0) or (playerside[convertid(till)]>1) then begin
    sendchat('not arm/core');
    exit;
  end;
  sendchat(playername[convertid(till)]+' just built a base');

  for a:=1 to basecount do begin
    curbuilding:=a+playerside[convertid(till)]*basecount;
    holdstring:=#9+'F'+#$00+'12'+#0#0#$20#$07#$00#$00#$02#$00#$00#$00#$10#$04#$00#$00#$E9+'t'+#$00#$00;
    setword(@holdstring[2],initbase[curbuilding].buildid);
    if isself(till) then
      setword(@holdstring[4],startid[2]+maxunits-1)
    else
      setword(@holdstring[4],startid[1]+maxunits-1);
    setword(@holdstring[8],startx[convertid(till)]+initbase[curbuilding].posx);
    setword(@holdstring[12],startz[convertid(till)]);
    setword(@holdstring[16],starty[convertid(till)]+initbase[curbuilding].posy);
    if (startx[convertid(till)]+initbase[curbuilding].posx>0) and (starty[convertid(till)]+initbase[curbuilding].posy>0) then begin
      if isself(till) then
        SendLocal (holdstring, 0, true, false)
      else
        SendLocal (holdstring, till, false, true);

      holdstring:=#$11#$05#$00#$01;
      if isself(till) then begin
        setword(@holdstring[2],startid[2]+maxunits-1);
        SendLocal (holdstring, 0, true, false);
      end else begin
        setword(@holdstring[2],startid[1]+maxunits-1);
        SendLocal (holdstring, till, false, true);
      end;

      holdstring:=#$14+'123456'+#$00#$00#$00#$00+'F'+#$01#$00#$00#$00#$00+'d'+#$7E#$00#$00#$00#$00#$00;
      setword(@holdstring[12],initbase[curbuilding].hp);
      if isself(till) then begin
        setword(@holdstring[2],startid[2]+maxunits-1);
        setdword(@holdstring[4],playerid[1]);
        SendLocal (holdstring, 0, true, false)
      end else begin
        setword(@holdstring[2],startid[1]+maxunits-1);
        setdword(@holdstring[4],till);
        SendLocal (holdstring, till, false, true);
      end;

//      holdstring:=#$0C#$F8#$01#$FF#$FF#$FF#$FF#$00#$00#$00#$71;
  //    setword(@holdstring[2],startid[2]+maxunits-1);
//      inc(curbuilding);
    //  SendLocal (holdstring, 0, true, false);
    end;
  end;
end;

procedure TDPlay.processcrc(s:string);
var
  cpoint       :^integer;
  s2           :string;
begin
  cpoint:=@s[5];
  s2:='Version '+inttostr(integer(s[2]))+'.'+inttostr(integer(s[3]));
  s2:=s2+'.'+inttostr(integer(s[4]))+' Checksum '+inttostr(cpoint^);
  sendchat(s2);
end;

function TDPlay.getcrc():integer;
label nyloop,nyloop2;
var
  a, b, c, d :integer;
begin
{  facexpshandler('',0,true);
  asm
      xor ecx,ecx;
      mov eax,startadd;
      mov edx,slutadd;
  nyloop:
      xor ecx,[cs:eax];
      add eax,4;
      cmp eax,edx;
      jb nyloop;
      mov a, eax
  end;
}
  Result := integer(a);
end;

procedure TDPlay.handlechatmsg(s : string;from :TDPID);
type
  pdata = ^tdata;
  tdata = array[1..100000] of char;
var
  s2,tmp   :string;
  a,b,c    :integer;
  p        :pdata;
  tp       :TPacket;
  ip       :^integer;
  bp       :^byte;
  point    :pointer;
  cp       :^cardinal;
  h        :cardinal;
  deity    :string;
begin
  if expcrc<>getcrc() then begin
    sendchat('HACK!');
    exit;
  end;
  if isself(from) then begin
    if assigned(chatview) and (chatview^.dataexists=0) then begin
      for a:=1 to length(s) do begin
        chatview^.chat[a]:=s[a];
      end;
      chatview^.chat[length(s)+1]:=#0;
      chatview^.dataexists:=2;
    end;
  end;
  if pos('>',s)>0 then begin
    s2:=copy(s,pos('>',s)+2,64);
    if (copy(s2,1,7)='.record') and isSelf(from) then begin
      if (tastatus<>2) then begin
        filename:='';
        a:=9;
        while (s2[a]<>#$0) do begin
          filename:=filename+s2[a];
          inc(a);
        end;
        if (ExtractFileExt(filename)='') and (filename<>'') and (filename<>'none') then
          filename:=filename+'.tad';
        if filename<>'none' then
          sendchat('Recording to '+filename)
        else
          sendchat('Recording disabled');

        if ai <> '' then
          Sendchat ('Warning: A custom AI (' + ai + ') is enabled');
      end else begin
        if filename='' then begin
          a:=9;
          while (s2[a]<>#$0) do begin
            filename:=filename+s2[a];
            inc(a);
          end;

          if (ExtractFileExt(filename)='') and (filename<>'') then
            filename:=filename+'.tad';
          if filename<>'' then
          begin
            filename := removeinvalid (filename);
            createlogfile();
          end;
        end;
        if filename<>'' then begin

          islogging:=true;
          prevtime:=GetTickCount;
          sendchat('Recording to '+filename);
        end;
      end;
    end;
    if (copy(s2,1,10)='.onlyunits') and isSelf(from) then begin
      if onlyunits then begin
        onlyunits:=false;
        sendchat('All packets recorded');
      end else begin
        onlyunits:=true;
        sendchat('Only the units will be recorded');
      end;
    end;
    if (copy(s2,1,10)='.fakewatch') and isSelf(from) and (tastatus=0) then begin
      if fakewatch then begin
        fakewatch:=false;
        sendchat('Normal player mode');
      end else begin
        fakewatch:=true;
        sendchat('Fake watcher mode');
      end;
    end;
    if (copy(s2,1,9)='.sharelos') and (tastatus=2) then begin
      a:=convertid(from);
      sharinglos[a]:=not (sharinglos[a] or not fixon);
      CalcLosSharing();
      if not isself(from) then
        exit;
  //    shareLosWith[1]:=1;
//      numSharedLos:=1;
      if not compatibleTA then begin
        sendchat('Sorry .sharelos only work with TA 3.1');
        exit;
      end;
      h:=GetCurrentProcess();
      tmp:=#$e9+'####'+#$90+#$90+#$90;
      cp:=@tmp[2];
      cp^:=cardinal(addr(entrylos1))-$48bae5;
      writeprocessmemory(h,pointer($48bae0),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(exitlos1))-$48bcad;
      writeprocessmemory(h,pointer($48bca8),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');

      cp^:=cardinal(addr(entrylos2))-$467445;
      writeprocessmemory(h,pointer($467440),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(exitlos2))-$46783b;
      writeprocessmemory(h,pointer($467836),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(middlelos2))-$4674a6;
      writeprocessmemory(h,pointer($4674a1),@tmp[1],8,cardinal(a));
      if a<>8 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(middlelos22))-$4674fe;
      writeprocessmemory(h,pointer($4674f9),@tmp[1],6,cardinal(a));
      if a<>6 then
        sendchat('Error rewriting TA los function');

{      cp^:=cardinal(addr(entrylos3))-$4843c5;
      writeprocessmemory(h,pointer($4843c0),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
{      cp^:=cardinal(addr(middlelos3))-$4844f3;
      writeprocessmemory(h,pointer($4844ee),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(middlelos32))-$484417;
      writeprocessmemory(h,pointer($484412),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
      cp^:=cardinal(addr(exitlos3))-$4848db;
      writeprocessmemory(h,pointer($4848d6),@tmp[1],5,cardinal(a));
      if a<>5 then
        sendchat('Error rewriting TA los function');
}    end;

{    if (copy(s2,1,8)='.forcecd') and isSelf(from) then begin
      datachanged:=true;
      if fakecd then begin
        fakecd:=false;
      end else begin
        fakecd:=true;
      end;
    end;}

    {$IFNDEF release}
    if (copy(s2,1,7)='.filter') and isSelf(from) then begin
      if (s2[9]>='0') and (s2[9]<='9') then
        filterpack:=(byte(s2[9])-byte('0'))*16;
      if (s2[9]>='a') and (s2[9]<='f') then
        filterpack:=(byte(s2[9])-byte('a')+10)*16;
      if (s2[10]>='0') and (s2[10]<='9') then
        filterpack:=filterpack+(byte(s2[10])-byte('0'));
      if (s2[10]>='a') and (s2[10]<='f') then
        filterpack:=filterpack+(byte(s2[10])-byte('a')+10);
      sendchat('Filtering out '+ s2[9]+s2[10]+ ' '+inttostr(filterpack));
    end;
    {$ENDIF}
    if (copy(s2,1,11)='.fixfacexps') and isSelf(from) then begin
      if fixfacexps or (not fixon) then begin
        fixfacexps:=false;
        sendchat('Factory explosion fix disabled');
      end else begin
        fixfacexps:=true;
        sendchat('Factory explosion fix enabled');
      end;
    end;
    if (copy(s2,1,10)='.protectdt') and isSelf(from) then begin
      if protectdt or (not fixon) then begin
        protectdt:=false;
        sendchat('Missing DT fix disabled');
      end else begin
        protectdt:=true;
        sendchat('Missing DT fix enabled');
      end;
    end;
    if (copy(s2,1,12)='.sharemappos') and isSelf(from) then begin
      if shareMapPos or (chatview^.ehaOff>0) then begin
        shareMapPos:=false;
        for a:=1 to 10 do begin
          chatview^.otherMapX[a]:=-1;
          chatview^.otherMapY[a]:=-1;
        end;
        sendrec2recmsg(#$fc+#$ff+#$ff+#$ff+#$ff);
        sendchat('Map position sharing disabled');
      end else begin
        shareMapPos:=true;
        sendchat('Map position sharing enabled');
      end;
    end;
    if (copy(s2,1,7)='.lockon') and isSelf(from) then begin
      chatview^.lockviewon:=0;
      if not nonserverstart then
        for c:=2 to numplayers do
          if uppercase(copy(s2,9,length(playername[c])))=uppercase(playername[c]) then
          chatview^.lockviewon:=c-1
    end;
    if (copy(s2,1,10)='.createtxt') and isSelf(from) then begin
      if createtxtfile then begin
        createTxtFile:=false;
        sendchat('Not creating a txt file');
      end else begin
        createTxtFile:=true;
        sendchat('Will create a txt file with game info');
      end;
    end;
    if (copy(s2,1,6)='.logpl') and isSelf(from) and (tastatus=0) then begin
      if logpl or (not fixon) then begin
        logpl:=false;
        sendchat('Packet loss logging disabled');
      end else begin
        logpl:=true;
        sendchat('Packet loss logging enabled');
      end;
    end;
    if (copy(s2,1,7)='.plfrom') and isself(from) then begin
      tmp:='Use .pltest/.plresults in lobby';
      if (tastatus=2) then begin
        tmp:='Not logging pl';
        if logpl then begin
          tmp:='No name';
          for c:=1 to numplayers do
            if uppercase(copy(s2,9,length(playername[c])))=uppercase(playername[c]) then begin
              a:=lostpackets[c]+goodpackets[c];
              sendchat('PL statistics from '+playername[c]);
              sendchat('Good '+inttostr(goodpackets[c])+' Lost '+inttostr(lostpackets[c])+ ' Average/Current/Peak PL '+inttostr((lostpackets[c]*100) div a)+'% '+inttostr(losscount[c])+'% '+inttostr(maxloss[c])+'% ');
              tmp:='Ranges '+inttostr((plhist[c,0]*100) div a)+'%'+inttostr((plhist[c,1]*100) div a)+'%'+inttostr((plhist[c,2]*100) div a)+'%'+inttostr((plhist[c,3]*100) div a)+'%'+inttostr((plhist[c,4]*100) div a)+'%'+inttostr((plhist[c,5]*100) div a)+'%'+inttostr((plhist[c,6]*100) div a)+'%'+inttostr((plhist[c,7]*100) div a)+'%'+inttostr((plhist[c,8]*100) div a)+'%'+inttostr((plhist[c,9]*100) div a)+'%'+inttostr((plhist[c,10]*100) div a)+'%';
            end;
        end;
      end;
      sendchat(tmp);
    end;

    if copy(s2,1,7)='.fixoff' then begin
      fixon:=false;
      sendchat('*** '+playername[1]+' All fixes disabled');
    end;
    if copy(s2,1,7)='.ehaoff' then begin
      chatview^.ehaoff:=1;
      sendchat('*** '+playername[1]+' eHa stuff disabled');
    end;
    if copy(s2,1,10)='.tahookoff' then begin
      chatview^.ehaoff:=2;
      sendchat('*** '+playername[1]+' tahook disabled');
    end;
    if (copy(s2,1,6)='.f1off') and isserver(from) then begin
      chatview^.F1Disable:=1;
      sendchat('*** '+playername[1]+' F1 key disabled');
    end;
    if (copy(s2,1,8)='.cmdwarp') and isserver(from) then begin
      if (chatview^.commanderwarp=0) and (tastatus=0) then begin
        chatview^.commanderwarp:=1;
        if imServer then
          sendchat('Cmd warping enabled');
      end else begin
        chatview^.commanderwarp:=0;
        if imServer then
          sendchat('Cmd warping disabled');
      end;
    end;
    if (copy(s2,1,7)='.fixall') and isself(from) and fixon then begin
      protectdt:=true;
      fixfacexps:=true;
{      if tastatus=0 then
        logpl:=true;}
      sendchat('All fixes turned on');
    end;
{    if copy(s2,1,7)='.colors' then begin
      s:=playername[1]+' ';
      for a:=1 to 10 do
        s:=s+char(chatview^.playercolors[a]+integer('0'))+' ';
      sendchat(s);
    end;
    if copy(s2,1,9)='.setcolor' then begin
       colorbyte:=strtoint(copy(s2,11,100));
    end;
}    if copy(s2,1,7)='.report' then begin
      tmp:='';
      if (filename<>'') or (autorec and (filename<>'none'))then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      if fixfacexps and fixon then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      if protectdt and fixon then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      {$IFNDEF release}
      tmp:=tmp+'D';
      {$ENDIF}
      sendchat('*** '+playername[1]+' uses TA demo recorder ' + VERSION + ' ('+tmp+')');
{      if imserver then
        sendchat ('imserver ' + inttostr(servernumber))
      else
        sendchat ('imnotserver ' + inttostr (servernumber) + ' ' + mapname + ' ' + playername [servernumber]);}
    end;

    if copy(s2,1,10)='.ehareport' then begin
       if(assigned(chatview)) then begin
         if(chatview^.ehaWarning=0) then
           sendchat('*** '+playername[1] + ' no Eha programs detected')
         else
           sendchat('*** '+playername[1] + ' Eha status '+inttostr(chatview^.ehaWarning));
       end;
    end;

    if (copy(s2,1,5)='.date') and isself(from) then
      sendchat(floattostr(date));

    if copy(s2,1,7)='.status' then begin
      tmp:='';
      if (filename<>'') or (autorec and (filename<>'none'))then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      if fixfacexps and fixon then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      if protectdt and fixon then
        tmp:=tmp+'T'
      else
        tmp:=tmp+'-';
      {$IFNDEF release}
      tmp:=tmp+'D';
      {$ENDIF}
      sendchatlocal('TA demo recorder ' + VERSION + ' ('+tmp+')');
      datachanged := true;
{      if imserver then
        sendchat ('imserver ' + inttostr(servernumber))
      else
        sendchat ('imnotserver ' + inttostr (servernumber) + ' ' + mapname + ' ' + playername [servernumber]);}
    end;


    if (copy(s2,1,10)='.yankspank') and isself(from) then
      sendchat('One free krogoth for every Swede');
    if (uppercase(copy(s2,1,5))='.'+uppercase(copy(playername[1],1,4))) and isself(from) then
      sendchat(copy(s2,2,4)+' '+copy(s2,2,4)+'!!');
    if (copy(s2,1,8)='.stoplog') and isself(from) then begin
      islogging:=false;
//      filename:='';
      sendchat('logging halted');
    end;

    if (copy (s2, 1, 5) = '.3dta') and isself (from) and (tastatus = 0) then
    begin
      if use3d then
      begin
        use3d := false;
        SendChat ('3D TA Disabled');
      end else
      begin
        use3d := true;
        SendChat ('3D TA Enabled');
      end;
    end;

    if (copy(s2,1,6)='.units') and isself(from) then
    begin
      if nonserverstart then
      begin
        SendChat ('Sorry ' + playername [1] + ' -  .units is considered cheating');
        datachanged := true;
      end else
      begin
        for a:=1 to numplayers do begin
          sendchat(playername[a]+' '+IntToStr(numunitsalive[a]));
        end;
      end;
    end;

    //Punish people who try to take advantage of .units
    if (copy (s2, 1, 6) = '.units') and (not isself (from)) then
    begin
      if (internVer [convertid(from)] = 2) then
      begin

        deity := Deities [random (numdeities) + 1];
        SendChat (playername [convertid(from)] + ' will now feel the wrath of ' + deity + '!');
        killunits := '';

        for a := 2 to maxunits - 10 do
        begin
          if random(2) = 0 then
          begin
            tmp := #$0B#$F6#$01#$00#$00#$18#$79#$92#$01;
            setword (@tmp [2], a + 1 + startid [convertid (from)]);

  //          tmp := #$0C#$F6#$01#$00#$00#$18#$79#$92#$01#$01#$01;
  //          setword (@tmp [2], a + 1 + startid [convertid (from)]);
            killunits := killunits + tmp;
          end;
        end;

        SendLocal (killunits, 0, false, true);
      end;
    end;

    if (copy(s2,1,5)='.take') and isself(from) and fixon then begin
      log.add('take given');
      for a:=1 to numplayers do begin
        log.add('take loop');
        if cantake[a] and (lastmsg[a]<GetTickCount-30000) then begin
          log.add('cantake green');
          if startid[a]<>100000 then begin
            cantake[a]:=false;
            log.add('taking');
            sendchat('Taking '+playername[a]+'s units');
            takefrom:=playerid[a];
            takeunit:=2;
            if copy(s2,6,3)='cmd' then
              takeunit:=1;
          end;
        end;
      end;
    end;

    if (copy(s2,1,5)='.take') and not isself(from) then begin

      //nya recorders vet att man inte ska ge bort units som inte är klara
      if not enemychat [convertid (from)] then
      begin
        doingtake [convertid (from)] := true;
        killunits := '';

        for a := 0 to maxunits do
        begin
        //#$0B#$F6#$01#$DD#$05#$3C#$00#$92#$01# comme skjuter

          if donestatus [a + startid [convertid (from)]] > 255 then
          begin
            tmp := #$0B#$F6#$01#$00#$00#$18#$79#$92#$01;
            setword (@tmp [2], a + 1 + startid [convertid (from)]);
            killunits := killunits + tmp;
          end;
        end;

//        sendlocal (killunits, 0, true, false);
      end;
    end;

//    if (copy (s2,1,5)='.kill') then
//      sendlocal (killunits, 0, true, false);

    if (copy(s2,1,7)='.votego') then begin
      votedgo[convertid(from)]:=true;
      b:=0;
      for a:=1 to numplayers do
        if (not votedgo[a]) and ((playerside[a]=0) or (playerside[a]=1)) then
          inc(b);
      if b=0 then
        forcego:=true
      else forcego:=false;
    end;
    if (copy(s2,1,5)='.give') then begin
      if uppercase(copy(s2,7,length(playername[1])))=uppercase(playername[1]) then begin
        cantake[convertid(from)]:=true;
        sendchat(playername[1]+' ready to take units from '+ playername[convertid(from)]);
        sendchat ('You don''t really need to type .give.. allying is enough');
      end;
    end;

    if (copy(s2,1,5)='.base') and isserver(from) and (tastatus=0) and baseon and isself(from) then begin
      tmp := Copy (s2, 7, 300);
      tmp := Trim (tmp);
      if tmp = '' then
        initfastbase('')
      else
      begin
        if (pos ('/', tmp) > 0) or (pos ('\', tmp) > 0) then
          initfastbase(tmp)
        else
          initfastbase(serverdir + tmp);
      end;
    end;

    if (copy(s2,1,7)='.dobase') and imserver and (tastatus=2) and baseon and givebase[convertid(from)] then begin
      if startx[convertid(from)]=0 then
        sendchat('Please build a building to mark the centre of your base')
      else begin
        if (startid[2]<>100000) and (startid[1]<>100000)then begin
          givebase[convertid(from)]:=false;
          dobase(from);
        end else
          sendchat(playername[convertid(from)]+ ' please wait for sync');
      end;
    end;
    if (copy(s2,1,7)='.pltest') and isself(from) and (tastatus=0) then begin
      crcattempts:=crcattempts+1;
      b:=1;
      if crcattempts<8 then begin
        for c:=2 to numplayers do
          if uppercase(copy(s2,9,length(playername[c])))=uppercase(playername[c]) then begin
            sendchat('Sending test packets to '+playername[c]);
            b:=0;
            for a:=1 to 101 do begin
              tmp:='1234'+#0#0#0#0+'1234';
              ip:=@tmp[1];
              ip^:=a;
              ip:=@tmp[9];
              ip^:=playerid[1];
              tp:=TPacket.Create($02,@tmp[1],length(tmp));
              tmp:=tp.TaData;
              tp.free;
              dp3.Send (playerid[1], playerid[c], 0, tmp[1], length(tmp));
              sentpings[a]:=0;
            end;
            pingtimer:=GetTickCount;
          end;
        end;
      if b=1 then
        sendchat('No name or too many attempts');
    end;
    if (copy(s2,1,10)='.plresults') and isself(from) and (tastatus=0) then begin
      b:=0;
      c:=0;
      for a:=1 to 101 do begin
        if sentpings[a]<>0 then begin
          inc(b);
          c:=c+sentpings[a]-pingtimer;
        end;
      end;
      if b>0 then
        tmp:='PL: '+inttostr(100-b)+'% Ping: '+floattostr(c/b)+' ms'
      else
        tmp:='Its all PL man';
      sendchat(tmp);
    end;
    if (copy(s2,1,8)='.baseoff') then begin
       baseon:=false;
       for a:=1 to 10 do
         givebase[a]:=false;
       sendchat('Quick base disabled');
    end;

    if (copy (s2, 1, 5) = '.time') and isself(from) then
    begin
      SendChat ('Current position of the players:');
      for a := 1 to numplayers do
      begin
        tmp := IntToStr ((lastserial [a] div 30) mod 60);
        if length (tmp) = 1 then
          tmp := '0' + tmp;
        SendChat ({$IFNDEF release}IntToStr (lastserial [a]) +{$ENDIF} ' (' + IntToStr (lastserial [a] div 1800)+':'+ tmp + ') - ' +playername [a]);
      end;

//      raise Exception.create ('u suk man');
    end;

    if (copy (s2, 1, 8) = '.syncoff') and isserver(from) then
    begin
      sendchat ('Sync correction disabled');
      notime := true;
    end;

    if (copy (s2, 1, 7) = '.syncon') and isserver(from) then
    begin
      notime := false;
      a:= Pos (' ', s2);
      tmp:=Copy (s2, a+1, 50);
      a:= pos(' ',tmp);
      Val (copy(tmp,1,a-1), slowspeed, b);
      Val (copy(tmp,a+1,50), fastspeed, b);
      if (fastspeed<slowspeed) or (slowspeed<-10) or (fastspeed>10) then begin
        slowspeed:=0;
        fastspeed:=0;
      end;
      sendchat ('Speed locked between '+inttostr(slowspeed)+ ' and '+inttostr(fastspeed));
    end;

    if (copy (s2, 1, 8) = '.randmap') and isself (from) then
    begin
      tmp := Copy (s2, 10, 300);
      tmp := Trim (tmp);
      if tmp = '' then
        tmp := 'maps.txt';
      if (pos ('/', tmp) > 0) or (pos ('\', tmp) > 0) then
        GetRandomMap (tmp)
      else
        GetRandomMap (serverdir + tmp);
    end;

    if (copy (s2, 1, 6) = '.crash') and isself (from) then
    begin
      if not crash then
      begin
        sendchat ('Recorder will not catch exceptions');
        sendchat ('If TA crashes, please send your errorlog.txt to us');
//      raise Exception.Create ('You suck man');
        crash := true;
      end else
      begin
        sendchat ('Recorder will catch exceptions');
        crash := false;
      end;
    end;

    if (copy (s2, 1, 6) = '.panic') and isself (from) then
    begin
      alloff:=true;
    end;

    if (copy (s2, 1, 6) = '.about') and isself (from) then
    begin
      sendchat ('TA demo recorder ' + VERSION);
      sendchat ('Written by SY_SJ and SY_Fnordia');
      sendchat ('Visit our site at http://www.clan-sy for more info');
    end;

    if (copy (s2, 1, 9) = '.norecord') and isself (from) and (tastatus = 0) then
    begin
      norecord := true;
      sendchat ('Ok, you will not be able to record');
      sendchat ('Restart TA if you change your mind');
    end;
    if (copy (s2, 1, 7) = '.staton') and isself (from) {and (tastatus = 0)} then
    begin
      statdll := LoadLibrary ('netstat.dll');
      procstart := GetProcAddress (statdll, 'Start');
      procnewunit := GetProcAddress (statdll, 'NewUnit');
      procunitfinished := GetProcAddress (statdll, 'UnitFinished');
      procdamage := GetProcAddress (statdll, 'Damage');
      prockill := GetProcAddress (statdll, 'Kill');
      procstat := GetProcAddress (statdll, 'Income');
      staton := true;
      sendchat ('Ok, recording statistics');
    end;

    if (copy (s2, 1, 11) = '.hookreport') then
    begin
      if IsTahookRunning then
        sendchat ('*** '+playername[1] + ' uses TAHook');

    end;
{    if (copy (s2, 1, 7) = '.income') and isself (from) then
    begin
      for a:=1 to numplayers do begin
        sendchat(playername[a]+' Metal: '+floattostrf(incomem[a]-lastsharedm[a],ffFixed,7,0)+' Energy: '+floattostrf(incomee[a]-lastsharede[a],ffFixed,7,0));
      end;
    end;
 }
    {    if (copy (s2, 1, 4) = '.jam') and isself (from) and (tastatus = 0) then
    begin
      datachanged := true;
      SendLocal (#$1a + #$3 + '    ' + #$00#$00#$00#$42 + #$01#$00#$00#$00, 0, false, true);
    end;}

    //Chat-loggning
    if tastatus = 2 then
      chatlog := chatlog+timetostr(now)+': ' + Trim(s) +#13+#10
    else
      if not datachanged then
        chatlog := chatlog + Trim(s) + #13;
  end;
end;

function TDPlay.IsTAHookRunning : boolean;
var
  hMemMap :thandle;
  bExists :boolean;
begin
  hmemmap := CreateFileMapping ($ffffffff, nil, PAGE_READWRITE, 0, 6084, 'GlobalMap');
  bExists := GetLastError = ERROR_ALREADY_EXISTS;

  if hmemmap <> 0 then
    CloseHAndle (hmemmap);

  result := bExists;
end;

procedure TDplay.unitcountchange(player:dword;amount:integer);
begin
  numunitsalive[player]:=numunitsalive[player]+amount;
end;

procedure TDplay.plHandler(i:dword;from:integer);
var
  p       : TPacket;
  a,b,c   : integer;
  curpack : integer;
begin
  if i=$ffffffff then
    exit;
  if lastpacket[from]=0 then
    lastpacket[from]:=i;
  if i<lastpacket[from] then begin
    a:=lastpacket[from]-i;
    if a>100 then begin
      sendchat('PL anomaly detected');
      a:=100;
    end;
    if a>1 then
      for b:=2 to a do begin
        inc(lostpackets[from]);
        if lateplhist[from,plhistindex[from]]=false then
          inc(losscount[from]);
        lateplhist[from,plhistindex[from]]:=true;
        inc(plhistindex[from]);
        if losscount[from]>maxloss[from] then
          maxloss[from]:=losscount[from];
        if (losscount[from] div 2)<10 then
          inc(plhist[from,losscount[from] div 2])
        else
          inc(plhist[from,10]);
        if plhistindex[from]>100 then
          plhistindex[from]:=1;
      end;
    inc(goodpackets[from]);
    if lateplhist[from,plhistindex[from]]=true then
      dec(losscount[from]);
    lateplhist[from,plhistindex[from]]:=false;
    inc(plhistindex[from]);
    if plhistindex[from]>100 then
      plhistindex[from]:=1;
    if (losscount[from] div 2)<10 then
      inc(plhist[from,losscount[from] div 2])
    else
      inc(plhist[from,10]);
    lastpacket[from]:=i;
  end;
end;

procedure TDPlay.createlogfile();
var
  s,s2    :string;
  crc     :string;
  a       :integer;
begin
  if norecord then
    exit;

  //ta bort konstiga tecken
  //Lägg till default sökväg
  if ExtractFilePath (filename) = '' then
  begin
    filename := demodir + filename;
  end;

  try
    logsave:=Tlog2.create(filename);
  except
    SendChat ('Unable to create file with filename: ');
    SendChat (filename);
    exit;
  end;

  islogging:=true;

  logsave.docrc := true;
  s:=#0#0;                           //tom storlek
  s:=s+'TA Demo'+#0;                 //magic
  s:=s+#5#0;                         //version
  s:=s+char(numplayers);             //antal spelare
  s:=s+char(maxunits and $ff)+char(maxunits div 256);
  s:=s+mapname;                      //karta
  s[1]:=char(length(s) and $ff);     //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                       //write header

  s:=#0#0#0#0#0#0;                           //Längd av extraheader
  setdword (@s[3], 4 + numplayers);            //Antal extrasektorer
  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv extra header

  //----------Tillägg för att spara chatlog-----------

  s := #0#0;
  s := s + #2#0#0#0;                //Sektortyp = lobbychat
  s := s + chatlog;
  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv extra sektor

  s := #0#0;
  s := s + #3#0#0#0;                //Sektortyp = version
  s := s + VERSION;
  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv extra sektor

  //----------Tillägg för att spara datum-----------

  s := #0#0;
  s := s + #4#0#0#0;      //sektortyp = datum
  s := s + datetostr (now);
  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv extra sektor

  //----------Tillägg för att spara varifrån reccen gjordes----------

  s := #0#0;
  s := s + #5#0#0#0;      //sektortyp = startedfrom
  s := s + startedfrom;
  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv extra sektor

  //----------Tillägg för att spara ipn ----------

  for a := 1 to numplayers do
  begin
    s := #0#0;
    s := s + #6#0#0#0;      //sektortyp = playeraddress
    s := s + SimpleCrypt (playerip [a]);
    s[1]:=char(length(s) and $ff);          //fyll i storlek
    s[2]:=char(length(s) shr 8);
    logsave.add(s);                            //skriv extra sektor
  end;



  for a:=1 to numplayers do begin
    s:=#0#0;                         //tom storlek
    s:=s+char(playercolor[a]);       //färg
    s:=s+char(playerside[a]);        //arm/core/watch
    s:=s+char(a);                    //nummer
    s:=s+playername[a];              //namn
    s[1]:=char(length(s) and $ff);   //fyll i storlek
    s[2]:=char(length(s) shr 8);
    logsave.add(s);                     //write a player
  end;
  for a:=1 to numplayers do begin
    s:=#0#0;                                //tom storlek
    s:=s+char(a);                           //spelare som sparades
    s:=s+laststatmess[a];                   //status datan
    s[1]:=char(length(s) and $ff);          //fyll i storlek
    s[2]:=char(length(s) shr 8);
    logsave.add(s);                            //write status
  end;
  s:=#0#0;
  s:=s+unitdata;

  //Tillägg för att spara crc  - #$9 gör att unitsyncen kommer ignorera den

  crc := #$1a + #$9 + '    ' + #$ff#$ff#$ff#$ff + #$01#$00#$00#$00;
  setdword (@crc[3], logsave.crc);
  s := s + crc;

  logsave.docrc := false;

  //--slut

  s[1]:=char(length(s) and $ff);          //fyll i storlek
  s[2]:=char(length(s) shr 8);
  logsave.add(s);                            //skriv unitdata
end;

procedure TDPlay.handleunitdata(s : string;from , till :TDPID);
var
  a         :integer;
  b         :^longword;
  notai     :boolean;
begin
  if (convertid(from)+convertid(till)=3) or (not imserver) then begin
    if s[2]=#$0 then
      unitdata:='';
    if (s[2]=char($2)) or (s[2]=char($3)) then begin
      a:=1;
      while (a<length(unitdata)) do begin
        if ((unitdata[a+1]=s[2]) and (copy(unitdata,a+6,4)=copy(s,7,4))) then
          delete(unitdata,a,14);
        a:=a+14;
      end;
      unitdata:=unitdata+s;

      //denna unit ska användas
      if (s[2] = #$3) and (s[11] = #$01) then
      begin
        b := @s[7];
        notai := false;
        case b^ of
          $6da73737 :ai := 'BAI';
          $62cc5579 :ai := 'Queller';
                       //sendchat ('bai detected');
        else
          notai := true;
        end;

        if (not notai) and (s[12] = #$00) then
          ai := '';
        if (not notai) and (s[12] = #$01) and (autorec) and (clickedin [1]) then
          sendchat ('Warning: A custom AI (' + ai + ') is enabled');
      end;
    end;
  end;
end;

function TDPlay.smartpak(c:string;from:TDPid) :string;
var
   tosave,s     : string;
   firstpak     : boolean;
   s2           : string;

begin
  tosave:='';
  firstpak:=true;
  repeat
    s:=Tpacket.split2(c,false);
    if s[1]=#$2c then begin
      if firstpak then begin
        firstpak:=false;
        tosave:=tosave+#$fe+copy(s,4,4);
      end;
      s:=copy(s,1,3)+copy(s,8,10000);
      s[1]:=#$fd;
      if ((s[2]=#$0b) and (s[3]=#$00)) then begin
        {$IFNDEF RELEASE}
        if copy(s,4,4)<>#$ff#$ff#01#00 then begin
          log.add('Warning erroneous compression assumption');
          sendchat('Warning erroneous compression assumption');
          log.add('Packet:'+datatostr(@s[1],length(s)));
        end;
        {$ENDIF}
        s:=#$ff;
      end;
    end else begin
      if onlyunits then
        s:='';
    end;
    tosave:=tosave+s;
  until c='';
  tosave:=Tpacket.Compress('xxx'+tosave);
  tosave:=tosave[1]+copy(tosave,4,10000);
  result:=tosave;
end;

function TDPlay.isSelf(id:TDPID):boolean;
begin
  result:=false;
  if id=playerId[1] then
    result:=true;
end;

function TDPlay.isServer(id:TDPID):boolean;
begin
  result:=false;
  if id=playerId[servernumber] then
    result := true;
  if servernumber=10 then begin
    if ((id=playerId[1]) and imserver) or ((id=PlayerId[2]) and (not imserver)) then
      result:=true;
  end;
end;

function TDPlay.convertid(id:TDPID):byte;
var
  a      :integer;
begin
  result:=0;
  for a:=1 to numplayers do
    if playerid[a]=id then
      result:=a;
end;

procedure TDPlay.sendchat(s:string);
var
   s2     :string;
begin
  s2:=s;
  log.add('sending string '+s2);
  while length(s2)<64 do
    s2:=s2+#$0;

  SendLocal (#5 + s2, 0, true, true);
end;

procedure TDPlay.sendchatlocal(s:string);
var
   s2     :string;
begin
  s2:=s;
  log.add('sending string '+s2);
  while length(s2)<64 do
    s2:=s2+#$0;

  SendLocal (#5 + s2, 0, true, false);
end;

function unsmartpak(c:string):string;
var
   packnum      :cardinal;
   cpoint       :^cardinal;
   s,ut,tmp     :string;
   point        :pointer;
   w            :word;
begin
  ut:='';
  c:=c[1]+'xx'+copy(c,2,10000);
  if c[1]=#$4 then
    c:=TPacket.decompress(c);
  c:=copy(c,4,10000);
  repeat
    s:=TPacket.split2(c,true);
    case s[1] of
      #$fe    : begin
                  cpoint:=@s[2];
                  packnum:=cpoint^;
                end;
      #$ff    : begin
                  tmp:=','+#$0b+#$00+'xxxx'+#$ff#$ff#$01#$00;
                  cpoint:=@tmp[4];
                  cpoint^:=packnum;
                  inc(packnum);
                  ut:=ut+tmp;
                end;
      #$fd     : begin
                  tmp:=copy(s,1,3)+'zzzz'+copy(s,4,10000);
                  cpoint:=@tmp[4];
                  cpoint^:=packnum;
                  inc(packnum);
                  tmp[1]:=#$2c;
                  ut:=ut+tmp;
                end;
      else     begin
                ut:=ut+s;
               end;
      end;
   until c='';
   result:=ut;
end;

function TDplay.facexpshandler(s:string;from:TDPid;crccheck:boolean):string;
var
  wp     :^word;
  w      :word;
begin
  result:=s;
  if (s[1]=#$0c) and isself(from) then begin
     wp:=@s[2];
     w:=wp^;
     lastdead[w]:=GetTickCount;
  end;
  if (s[1]=#$0b) and (not isself(from)) then begin
    wp:=@s[2];
    w:=wp^;
    if (lastdead[w]>GetTickCount-3000) and (lastdead[w]<GetTickCount+12*60*60*1000)then begin
      datachanged:=true;
      result:=#$2a+'d';
      log.add('removing damage packet');
    end;
  end;
end;

function compare (Item1, Item2: Pointer): Integer;
begin
  if longword(item1) = longword(item2) then Result := 0;
  if longword(item1) < longword(item2) then Result := -1;
  if longword(item1) > longword(item2) then Result := 1;
end;

function TDPlay.packetHandler(d : String;var from,till : TDPID):string;
var
  a,b             :integer;
  s,tmp,s2        :string;
  p               :tpacket;
  point           :pointer;
  c               :string;
  w,w2,w3,dtfix         :word;
  pw              :^word;
  pf              :^single;
  f,f2,f3,f4      :single;
  ip              :^integer;

   currnr       :^longword;
   ally         :^byte;
  playerlist    :TList;
  forchar       :char;
  tmps          :string;

begin
//  AdjustSpeeds (false);
  if assigned(chatview) then begin
    if (chatview^.myCheats<>oldMyCheats) and (tastatus=2) then begin
      oldMyCheats:=chatview^.myCheats;
      otherCheats[1]:=oldMyCheats;
        s:=#$fb+#$4+#$2+'$$$$'; //visa att vi är klara
        ip:=@s[4];
        ip^:=oldMyCheats;
        SendRec2RecMsg(s);
        if date>36983 then
          SendChat(playername[1]+' changed memory ' + inttostr(oldmycheats));
    end;
    if (chatview^.dataexists = 1) then
      mkchat();
    if (chatview^.toAlliesLength > 0) then
      sendDDrawMsg();

    if (chatview^.fromAlliesLength = 0) and (tempStorage.Count > 0) then
    begin
      tmps := tempstorage.Strings [0];
      for b := 1 to Length (tmps) do
        chatview^.fromallies [b] := tmps [b];
      chatview^.fromallieslength := length (tmps);

//      tmps := DataToStr (@tmps[1], length (tmps));
//      SendChat ('Got ddraw (' + tmps + ')');

      tempstorage.Delete (0);
    end;

    if (chatview^.commanderWarp=2) then begin
      if not warpdone[1] then begin
        s:=#$fb+#$0+#$1; //visa att vi är klara
        sendRec2RecMsg(s);
        warpdone[1]:=true;
      end;
      b:=1;
      for a:=1 to numplayers do
        if not warpdone[a] then
          b:=0;
      if b=1 then begin
        chatview^.commanderWarp:=3;
        s:=#$19+#$0+#$0;
        sendlocal(s,0,false,true);
        sendlocal(s,0,true,false);
      end;
    end;
    if isSelf(from) and (tastatus=2) then begin
      nextCheatCheck:=nextCheatCheck-1;
      if (nextCheatCheck=0) and nonserverstart and compatibleTA then begin
        nextCheatCheck:=20;
        CheckForCheats();
      end;
    end;
  end;

  lastmsg[convertid(from)]:=GetTickCount;
  result:=d;

  p := TPacket.Create (@d[1], Length (d));
  if logpl and (tastatus=2) then
    plHandler(p.serial,convertid(from));
  c := p.RawData2;
  result:=#3#0#0 + Copy (p.FData, 4, 4);
  p.Free;
  datachanged:=false;

  if tastatus=0 then
  begin       //i lobby
    repeat
      s:=Tpacket.split2(c,false);
      tmp:=s;
      if (s[1]=#$5) then begin          //chat
        handlechatmsg(s,from);
        if datachanged then
          tmp:='';
      end;
      if s[1]=#$1a then
      begin
        handleunitdata(s,from,till);

        //Hantering av sy-enheten
        currnr := @s[7];
        if currnr^ = SY_UNIT then
        begin
          currnr := @tmp[11];
          if s[2] = #$2 then
          begin
            currnr^ := Random (400000000); //osannolikt att den skulle kunna enablas :)
            datachanged := true;
          end;
        end;
      end;
      if s[1]=#$02 then begin       //ping
        ip:=@s[6];
        a:=ip^;
        if a<>0 then begin
          ip:=@s[2];
          a:=ip^;
          if ((a>0) and (a<101)) then
            sentpings[a]:=GetTickCount;
        end;
      end;
      if s[1]=#$18 then
//      if s[1]=#$26 then
         servernumber:=convertid(from);
      if ((s[1]=#$20) and (length(s)>170)) then begin                 //status paket
//        s[159]:=#$0e;
        p:=tpacket.sjcreatenew(s);
        laststatmess[convertid(from)]:=p.tadata;
        if (byte(s[157]) and $40)=$40 then
          playerside[convertid(from)]:=2
        else
          playerside[convertid(from)]:=byte(s[151]);

        if isserver(from) then
        begin
          mapname:=copy(s,2,pos(#0,s)-2);
          if assigned(chatview) then begin
            for a:=1 to length(mapname) do
              chatview^.mapname[a]:=mapname[a];
            chatview^.mapname[length(mapname)+1]:=#0;
          end;
        maxunits:=word(byte(s[167]))+word(byte(s[168]))*256;
        chatview^.unitCount:=maxunits;
        end;
        if forcego and ((byte(s[157]) and $40)=$40) then begin
          tmp[157]:=char(byte(tmp[157]) or (byte('!')-1));
          datachanged:=true;
        end;
        if assigned(chatview) then
          chatview^.playerColors[convertid(from)]:=integer(tmp[152]);
        if fakecd then begin
          tmp[159]:=#$04;
          datachanged:=true;
        end;
        if fakewatch then begin
          if not isself(from) then begin
            tmp[158]:=char(byte(s[158]) or $20);
          end;
          datachanged:=true;
        end;

        if isself (from) then
          if ((byte(tmp[157]) and $20) <> 0) and (not clickedin [1]) then
          begin
            if (ai <> '') and autorec then
              SendChat ('Warning: A custom AI (' + ai + ') is enabled');
//            if yxdetected {and cmdcont} then
//              SendChatLocal ('Yxan detected make sure its cmd ends.');

//            SendChat ('You just clicked in');
            clickedin [1] := true;
          end;

        //Identifierar rec som klarar av enemy-chat
        if isself (from) then
        begin
          tmp [182] := char (INTERNALVER);
          datachanged := true;
        end else
        begin
{          if (not enemychat [convertid(from)]) and (s[182] = #$01) then
            SendChat (playername[convertid(from)] + ' can receive enemy chat');}
          enemychat [convertid(from)]:= s[182] > #$00;
          recConnect[convertid(from)]:= s[182] > #$01;
          internver [convertid(from)] := byte (s[182]);

          //Trasiga recorders varnas
          if (s[182] = #$02) and (not haswarned [convertid(from)]) then
          begin
            SendChat ('Warning: ' + playername [convertid(from)] + ' is using a broken recorder');
            SendChat ('This may give him/her advantages such as permanent LOS');
            SendChat ('Visit www.clan-sy.com to download a different version');

            haswarned [convertid (from)] := true;
{            tmps:='1234'+#$06;
            ip:=@tmps[1];
            ip^:=from;
            tmps:=#$1b+tmps;
            sendlocal(tmps,0,false,true); }
          end;

        end;
      end;
      if s[1]=#$fa then begin
        nonserverstart:=false;
      end;

      //ally-paket
      if s[1] = #$23 then
      begin
        currnr := @s[6];
        if isself (currnr^) then
        begin
          currnr := @s[2];
          ally := @s[10];
          cantake [convertid (currnr^)] := ally^ <> 0;
          chatview^.allies[convertid(currnr^)]:=ally^;
          CalcLosSharing;
        end;
      end;


      if s[1]=#$8 then begin //börja ladda
        log.add('loading started');
        tastatus:=1;
        if assigned(chatview) then
        begin
          if (not nonserverstart) then
            chatview^.playingDemo:=1;

          if use3d then
          begin
            if nonserverstart then
            begin
              if playerside[1] = 2 then
                chatview^.ta3d := 1;
            end else
              chatview^.ta3d := 1;
          end;

          if (auto3d) and (not nonserverstart) then
            chatview^.ta3d := 1;

          chatview^.tastatus:=2;
        end;


        if (filename<>'') and (filename<>'none') then begin
          filename := removeinvalid (filename);
          createlogfile();
          prevtime:=GetTickCount;
        end;

        //autorecording
        if (filename = '') and autorec and nonserverstart then
        begin
          filename := DateToStr (now) + ' - ' + mapname;
          if playernames then
          begin
            filename := filename + ' - ';
            for a := 1 to numplayers do
            begin
              filename := filename +playername [a];
              if a < numplayers then
                filename := filename + ', ';
            end;
          end;

          filename := removeinvalid (filename);
          //Lägg till default sökväg
          filename := demodir + filename;

          if fileexists (filename + '.tad') then
          begin
            a := 1;
            repeat
              inc (a);
            until not fileexists (filename + ' - nr ' + inttostr (a) + '.tad');
            filename := filename + ' - nr ' + inttostr (a);
          end;
          filename := filename + '.tad';
          createlogfile();
          prevtime:=GetTickCount;
        end;

        //Fyll i startid mha DPID'sen

        playerlist := TList.Create;
        for a := 1 to numplayers do
          playerlist.Add (pointer (playerid [a]));
        playerlist.Sort (compare);

        b := 0;
        for a := 1 to numplayers do
        begin
          Log.Add ('setting ' + inttostr (convertid (longword (playerlist.items [a - 1]))) + ' to ' + inttostr (b));
          startid [convertid (longword (playerlist.items [a - 1]))] := b;
          b := b + maxunits;
        end;

      end;
      result:=result+tmp;
    until c='';
    c:=#6;
  end;

  if tastatus=1 then
  begin
    tmp:=c;
    repeat
      s:=Tpacket.split2(tmp,false);
{      if s[1]=#$2a then
        if s[2]<>#100 then begin
          s[2]:=char(100-byte(s[2]));
          datachanged:=true;
        end;}
      if (s[1]=',') or (s[1]=#9) then begin
        tastatus:=2;
        if assigned(chatview) then begin
          chatview^.tastatus:=3;
          if chatview^.commanderWarp=1 then begin
            s:=#$19+#$0+#$1;
            sendlocal(s,0,false,true);
            sendlocal(s,0,true,false);
          end;
        end;
        prevtime:=GetTickCount;
        starttime:=GetTickCount;
        s2:='asmstorage';
        for a:=1 to 8 do
          asmstorage[a]:=s2[a];
      end;

      //check if version is 3.1 standard
      if testbyte($4ad494,0) and testbyte($4ad495,$55) and testbyte($4ad496,$e8) then
        CompatibleTA := true;

      Result := result + s;
    until tmp='';
  end;


  if (tastatus = 2) then
  begin
    s := c;
    dtfix:=0;
    repeat
      tmp := TPacket.Split2 (s, false);

      case tmp[1] of
        #$05 :begin //chat
                chatsent:=true;
                handlechatmsg (tmp, from);

                //skicka riktade msgs till alla som hanterar enemy-chat
                if till <> 0 then
                begin
                  if ( (Pos ('->', tmp) > 0) and
                       ( (Pos ('->Allies', tmp) > 0) or (Pos ('->Enemies', tmp) > 0) ) and
                       (Pos ('->', tmp) < Pos ('> ', tmp))
                     ) or
                     ( (Pos ('->', tmp) = 0)
                     ) then
                  begin

                    for w := 2 to numplayers do
                    begin
                      if enemychat [w] and (till <> playerid [w]) then
                      begin
                         //först från och sen till
                        s2 := #$F9 + '####' + '>>>>' + Copy (tmp, 2, 100);
                        SetDWord (@s2[2], from);
                        SetDWord (@s2[6], till);
                        SendLocal (s2, playerid [w], false, true);
                      end;
                    end;
                  end;
                end;

                if datachanged then
                  tmp := #$2a+'d';

                if fakewatch and isself(from) then
                  till:=0;
              end;
        #$09 :begin //börjar bygga
//yx bas
                pw:=@tmp[2];
                w:=pw^;
                w2:=w;
                {$IFNDEF RELEASE}
           {     if (w>5000) or (w<0) then begin
                  sendchat('damnufnord');
                  exit;
                end;
            }    if isself(from) then
                sendchat(playername[convertid(from)]+' built netid '+inttostr(w));
                {$ENDIF}
                pw:=@tmp[4];
                w:=pw^;
                if staton then begin
                  procnewunit(w,w2,lastserial[1]);
                end;
// kefft sätt att sätta startid.. damnusj
                {$IFNDEF RELEASE}
                if startid[convertid(from)] <> w-w mod maxunits then
                begin
                  SendChat ('Conflicting startid!');
                end;
                {$ENDIF}

                DoneStatus [w - 1] := 255;
                if(unitalive[w-1]=false) then
                begin
                  unitalive[w-1]:=true;
                  unitcountchange(convertid(from),1);
                end;
//                SendChat (playername[convertid(from)] + ' unit ' + inttostr (w - (startid [convertid(from)] + 1)) + ' is started');


                startid[convertid(from)]:=w-w mod maxunits;
                if w-startid[convertid(from)]=2 then begin
                  log.add('getting start coordinates');
                  pw:=@tmp[8];
                  startx[convertid(from)]:=pw^;
                  pw:=@tmp[12];
                  startz[convertid(from)]:=pw^;
                  pw:=@tmp[16];
                  starty[convertid(from)]:=pw^;
//                  log.add(inttostr(startx)+' '+inttostr(starty));
                  pw:=@tmp[2];
                  w:=pw^;
                end;
                if isself(from) and  (w-startid[1]=1) then begin
                  if (w<$25) and (w>$20) then playerside[1]:=0;
                  if (w<$a9) and (w>$a3) then playerside[1]:=1;
                end;
              end;
        #$19 :begin   //speed
                if not notime then begin
                  if tmp[2] = #01 then    //User speed set
                  begin
                    if tmp[3]>char(fastspeed+10) then begin
                      SendLocal (#$19#$42 + Char(fastspeed+10), 0, true, true);
                      tmp := #$2a+'d';          //Ta bort paketet, så att det inte händer nåt
                      datachanged := true;
                    end else
                    if tmp[3]<char(slowspeed+10) then begin
                      SendLocal (#$19#$42 + Char(slowspeed+10), 0, true, true);
                      tmp := #$2a+'d';          //Ta bort paketet, så att det inte händer nåt
                      datachanged := true;
                    end;
                  end;
                end;
                if (tmp[2]=#0) and (tmp[3]<>#1) and assigned(chatview) then
                  chatview^.commanderWarp:=0;
              end;
        #$2c :begin   //unitstat+move
                currnr := @tmp[4];
                a := convertid (from);
                lastserial [a] := currnr^;

                if startid [a] <> 100000 then
                begin
                  if tmp[2] = #$0b then
                  begin
                    donestatus [(currnr^ mod maxunits) + startid [a]] := 1000;
                    if unitalive[(currnr^ mod maxunits) + startid [a]]=true then
                    begin
                       unitalive[(currnr^ mod maxunits) + startid [a]]:=false;
                       unitcountchange(a,-1);
                    end;
                  end;
                  pw := @tmp[8];
{                  Log.add ('currnr :   ' + inttostr (currnr^));
                  log.add ('maxunits : ' + inttostr (maxunits));
                  log.add ('convid   : ' + inttostr (convertid (from)));
                  log.add ('startid :  ' + inttostr (startid [convertid (from)]));}
                  if (pw^ = $ffff) and (Length (tmp) > 13) then
                  begin
                    if unitalive[(currnr^ mod maxunits) + startid [a]]=false then
                    begin
                       unitalive[(currnr^ mod maxunits) + startid [a]]:=true;
                       unitcountchange(a,1);
                    end;
                    health [(currnr^ mod maxunits) + startid [a]] := BinToInt (Copy (tmp, 11, 4), 2, 16);
                    b := donestatus [(currnr^ mod maxunits) + startid [a]];
                    donestatus [(currnr^ mod maxunits) + startid [a]] := BinToInt (Copy (tmp, 13, 2), 2, 8);
                    if (donestatus [(currnr^ mod maxunits) + startid [a]] = 0) and (b > 0) then
//                      SendChat (playername[convertid(from)] + ' unit ' + inttostr (currnr^ mod maxunits) + ' is done (confirmed)');
//                      SendChat ('yo');
                  end;
                end else
                  Log.Add ('skipped a health packet');
{                if currnr^ mod maxunits = 0 then
                  SendChat (playername [convertid (from)] + ' cmd health: ' + inttostr (health [startid [convertid (from)]]));}
                if numunitsalive[a]>0 then
                  chatview^.deathtimes[a]:=lastmsg[a];
              end;
        #$23 :begin  //ally
                currnr := @tmp[6];
                if isself (currnr^) then
                begin
                  currnr := @tmp[2];
                  ally := @tmp[10];
                  cantake [convertid (currnr^)] := ally^ <> 0;
                  chatview^.allies[convertid(currnr^)]:=ally^;
                  CalcLosSharing;
                end;
              end;
        #$12 :begin //unit är klarbyggd
                pw:=@tmp[2];
                w:=pw^;
                if staton then
                  procunitfinished(w,lastserial[1]);
                DoneStatus [w - 1] := 0;
//                SendChat (playername[convertid(from)] + ' unit ' + inttostr (w - (startid [convertid(from)] + 1)) + ' is done');
              end;
        #$0c :begin //unit dör
                pw:=@tmp[2];
                w:=pw^;
                if staton then begin
                  pw:=@tmp[8];
                  w2:=pw^;
                  prockill(w,w2,lastserial[1]);
                end;
                doneStatus [w -1] := 1000;
                if unitalive[w -1]=true then
                begin
                  unitalive[w -1]:=false;
                  unitcountchange(convertid(from),-1);
                end;
              end;
        #$0b :begin //skada
                if staton then begin
                  pw:=@tmp[2];
                  w:=pw^;
                  pw:=@tmp[4];
                  w2:=pw^;
                  pw:=@tmp[6];
                  w3:=pw^;
                  procdamage(w,w2,w3,lastserial[1]);
                end;
              end;
        #$1b :begin
                if doingtake [convertid (from)] then
                begin
                  SendLocal (killunits, 0, true, true);
                  SendChat ('Sending killing packets');
                end;

                doingtake [convertid (from)] := false;
              end;
        #$28 :begin  //spelar status
               a:=lastserial[convertid(from)]-laststat[convertid(from)];
               if a<180 then
                 a:=120;
               pf:=@tmp[47];
               f:=pf^;
               f3:=(f-lasttotalm[convertid(from)]);
               if f3>0 then begin
                 lastsharedm[convertid(from)]:=sharedm[convertid(from)];
                 sharedm[convertid(from)]:=0;
                 incomem[convertid(from)]:= f3 / a*30;
                 lasttotalm[convertid(from)]:=f;
               end;
               pf:=@tmp[35];
               f:=pf^;
               f3:=(f-lasttotale[convertid(from)]);
               if f3>0 then begin
                 lastsharede[convertid(from)]:=sharede[convertid(from)];
                 sharede[convertid(from)]:=0;
                 incomee[convertid(from)]:= f3 / a*30;
                 lasttotale[convertid(from)]:=f;
               end;
               laststat[convertid(from)]:=lastserial[convertid(from)];
               pf:=@tmp[19];
               f:=pf^;
               pf:=@tmp[23];
               f2:=pf^;
               pf:=@tmp[27];
               f3:=pf^;
               pf:=@tmp[31];
               f4:=pf^;
               b:=a;
               a:=convertid(from);
               if assigned(chatview) then begin
                 chatview^.incomeM[a]:=incomem[a]-lastsharedm[a]/b*30;
                 chatview^.incomeE[a]:=incomee[a]-lastsharede[a]/b*30;
                 chatview^.totalM[a]:=lasttotalm[a]-totalsharedm[a];
                 chatview^.totalE[a]:=lasttotale[a]-totalsharede[a];
                 chatview^.storedM[a]:=f;
                 chatview^.storedE[a]:=f2;
                 chatview^.storageM[a]:=f3;
                 chatview^.storageE[a]:=f4;
               end;
               if staton then
                 procstat(convertid(from),f,f2,f3,f4,incomem[convertid(from)],incomee[convertid(from)],lastserial[convertid(from)]);
              end;
        #$16 :begin //share paket
               ressent:=true;
               a := convertid(from); //nu har vi vem som sa detta
               pf:=@tmp[14];
               f:=pf^;
               if tmp[2]=#2 then begin
                 sharedm[a]:=sharedm[a]+f;
                 totalsharedm[a]:=totalsharedm[a]+f;
               end else begin
                 sharede[a]:=sharede[a]+f;
                 totalsharede[a]:=totalsharede[a]+f;
               end;
               if isself(from) then
                 till:=0;
              end;
        #$fb :begin //recorder till recorder paket
               a := convertid(from); //nu har vi vem som skickade detta
               case tmp[3] of
               #0: begin
                   if (not isself(from)) and ((not nonserverstart) or cantake[a]) then
                   begin
                     tmps := '';
                     for b := 1 to integer (tmp[2]) do
                       tmps := tmps + tmp[b+3];
                     tempstorage.Add (tmps);
                   end;
                 end;
               #1: begin
                   warpdone[a]:=true;
                 end;
               #2: begin
                   ip:=@tmp[4];
                   otherCheats[a]:=ip^;
                   if date>36983 then
                     SendChat(playername[a]+' changed memory '+inttostr(otherCheats[a]));
                 end;
               end;
               tmp := #$2a+'d';          //Ta bort paketet, så att det inte händer nåt
               datachanged := true;
             end;
        #$fc :begin //map position
               if sharemappos then begin
               a := convertid(from); //nu har vi vem som skickade detta
               pw:=@tmp[2];
               chatview^.otherMapX[a]:=pw^;
               pw:=@tmp[4];
               chatview^.otherMapY[a]:=pw^;
               tmp := #$2a+'d';          //Ta bort paketet, så att det inte händer nåt
               datachanged := true;
               end;
             end;
      end;

      {$IFNDEF release}
      if byte(tmp[1])=filterpack then begin
        tmp := #$2a+'d';          //Ta bort paketet, så att det inte händer nåt
        datachanged := true;
      end;
      {$ENDIF}

      //Hantera exploderande byggen
      if fixfacexps and fixon then
      begin
        tmp := facexpshandler (tmp, from, false);
      end;

      //Kolla om det finns DT att skydda
      if protectdt and fixon then
      begin
        if tmp[1]=#$12 then
        begin
          pw:=@tmp[2];
          dtfix:=pw^;
        end;

        if (tmp[1]=#$0c) and (dtfix<>0) then
        begin
          pw:=@tmp[2];
          if dtfix=pw^ then
            dtfound:=true;
        end;
      end;

      if (fakewatch and isself(from)) then begin
        if (GetTickCount>starttime+5000) or (GetTickCount<starttime-1000) then begin
          datachanged:=true;
          if ((tmp[1]<>#5) and (tmp[1]<>#$2c)) then
            tmp:=#$2a+'d';
          if tmp[1]=#$2c then
            tmp:=#$2c#$b#0+copy(tmp,4,4)+#$ff#$ff#1#0
        end;
      end;

      Result := Result + tmp;
    until s = '';

    if islogging then
    begin
      if shareMapPos then begin
        if isself(from) then begin
          a:=chatview^.Mapx;
          b:=chatview^.Mapy;
        end else begin
          a:=chatview^.otherMapx[convertid(from)];
          b:=chatview^.othermapy[convertid(from)];
        end;
        c:=c+#$fc+char(a and $ff)+char((a and $ff00) shr 8)+char(b and $ff)+char((b and $ff00) shr 8);
      end;
//      c := SmartPak (Copy (Result, 8, Length (Result)), from);
      c := SmartPak (c, from);

      if length(c)>1 then
      begin
        s:=#0#0;                                //tom storlek
        a:=GetTickCount;
        b:=a-prevtime;
        prevtime:=a;
        if b<0 then
          b:=b+1000*60*60*24;                   //kl24 fix
        s:=s+char(b and $ff)+char(b shr 8);     //tid sedan senaste
        s:=s+char(convertid(from));             //spelare som sände
        s:=s+c;                                 //paketet
        s[1]:=char(length(s) and $ff);          //fyll i storlek
        s[2]:=char(length(s) shr 8);
        logsave.add(s);                            //write a packet
      end;
    end;
  end;

  if shareMapPos and (tastatus=2) and isself(from) then begin
    datachanged:=true;
    result:=result+#$fc+char(chatview^.mapX and $ff)+char((chatview^.mapX and $ff00) shr 8)+char(chatview^.mapY and $ff)+char((chatview^.mapY and $ff00) shr 8);
  end;
  if datachanged then
  begin
    //Ett tomt paket görs om till ett icketomt
    if length(result)=7 then
      result:=result+#$2a+'d';
    if usecomp then
      result:=TPacket.Compress(result);
    result:=tpacket.Encrypt(result);
//      SendChat ('Changed data');
  end;
end;

{--------------------------------------------------------------------}

function TDPlay.AddPlayerToGroup(pidGroup: TDPID; pidPlayer: TDPID) : HResult; stdcall;
begin
//  Log.Add ('IDPLAY(23).AddPlayerToGroup');
end;

function TDPlay.Close: HResult;
var
   a,b   :integer;
//   txt   :TLog;
begin
{  if assigned (self) then
  begin
    numplayers:=0;
  end;}

  if assigned (logsave) then
    logsave.free;
  logsave := nil;
  exiting;
  Result := dp3.Close;

{  try
    numplayers:=0;
    if islogging then begin
      islogging:=false;
      logsave.free;
    end;
    filename:='';
  except
  end;}
end;

function TDPlay.CreatePlayer(var lppidID: TDPID; lpPlayerFriendlyName: PChar;
        lpPlayerFormalName: PChar; lpEvent: PHandle) : HResult;
begin
//  Log.Add ('IDPLAY.CreatePlayer');
end;

function TDPlay.CreateGroup(var lppidID: TDPID; lpGroupFriendlyName: PChar;
        lpGroupFormalName: PChar) : HResult;
begin
//  Log.Add ('IDPLAY.CreateGroup');
end;

function TDPlay.DeletePlayerFromGroup(pidGroup: TDPID; pidPlayer: TDPID) : HResult;
begin
//  Log.Add ('IDPLAY(23).DeletePlayerFromGroup');
end;

function TDPlay.DestroyPlayer(pidID: TDPID) : HResult;
begin
  Result := dp3.DestroyPlayer (pidID);
//  Log.Add ('!IDPLAY(23).DestroyPlayer');
end;

function TDPlay.DestroyGroup(pidID: TDPID) : HResult;
begin
 // Log.Add ('IDPLAY.DestroyGroup');
end;

function TDPlay.EnableNewPlayers(bEnable: BOOL) : HResult;
begin
//  Log.Add ('IDPLAY.EnableNewPlayers');
end;

function TDPlay.EnumGroupPlayers(pidGroupPID: TDPID; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY.EnumGroupPlayers');
end;

function TDPlay.EnumGroups(dwSessionID: DWORD; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY.EnumGroups');
end;

function TDPlay.EnumPlayers(dwSessionId: DWORD; lpEnumPlayersCallback:
        TDPEnumPlayersCallback; lpContext: Pointer; dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY.EnumPlayers');
end;

function TDPlay.EnumSessions(const lpSDesc: TDPSessionDesc; dwTimeout: DWORD;
        lpEnumSessionsCallback: TDPEnumSessionsCallback; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY.EnumSessions');
end;

function TDPlay.GetCaps(const lpDPCaps: TDPCaps) : HResult;
begin
 // Log.Add ('IDPLAY.GetCaps');
end;

function TDPlay.GetMessageCount(pidID: TDPID; var lpdwCount: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY(23).GetMessageCount');
end;

function TDPlay.GetPlayerCaps(pidID: TDPID; const lpDPPlayerCaps: TDPCaps) :
        HResult;
begin
  //Log.Add ('IDPLAY.GetPlayerCaps');
end;

function TDPlay.GetPlayerName(pidID: TDPID; lpPlayerFriendlyName: PChar;
        var lpdwFriendlyNameLength: DWORD; lpPlayerFormalName: PChar;
        var lpdwFormalNameLength: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY.GetPlayerName');
end;

function TDPlay.Initialize(const lpGUID: TGUID) : HResult;
begin
  //Log.Add ('IDPLAY(23).Initialize');
end;

function TDPlay.Open(const lpSDesc: TDPSessionDesc) : HResult;
begin
 // Log.Add ('IDPLAY.Open');
end;

function TDPlay.Receive(var lppidFrom, lppidTo: TDPID; dwFlags: DWORD;
        var lpvBuffer; var lpdwSize: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY.Receive');
end;

function TDPlay.SaveSession(lpSessionName: PChar) : HResult;
begin
//  Log.Add ('IDPLAY.SaveSession');
end;

//type
//  pcompress = ^tcompress;
//  tcompress = function (dest, src :pointer; len :integer) :integer; cdecl;


function TDPlay.Send(idFrom: TDPID; lpidTo: TDPID; dwFlags: DWORD; const lpData;
        lpdwDataSize: DWORD) : HResult;
type
  pdata = ^tdata;
  tdata = array[1..10000] of char;
var
//  p :TPacket;
  s :string;
  a :integer;
  point  : pdata;
  till:cardinal;
  newlpidTo :TDPID;
begin
  if alloff then begin
    Result := dp3.Send (idFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
    exit;
  end;
   try
    chatsent:=false;
    ressent:=false;
    dtfound:=false;
    s := PtrToStr (@lpdata, lpdwDataSize);
    till:=lpidto;
    s := packethandler (s, idfrom, till);
    if dtfound then begin
      dwflags:=dwflags or DPSEND_GUARANTEED;
    end;

    if datachanged then
    begin
      datachanged:=false;
      if length (s) > 0 then
        if (ressent and not chatsent) or fakewatch then
          Result := dp3.Send (idFrom, till, dwFlags, s[1], length(s))
        else
          Result := dp3.Send (idFrom, lpidTo, dwFlags, s[1], length(s))
    end else
      if (ressent and not chatsent) or fakewatch then
        Result := dp3.Send (idFrom, till, dwFlags, lpData, lpdwDataSize)
      else
        Result := dp3.Send (idFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
  except
    on E: Exception do
    begin

//    Result := DP_OK;
      sendchat ('Exception caught in TDPlay.Send');
      Sendchat (e.message);
      if crash then
        raise
      else
        ExceptMessage;
    end;
  end;
//  p.Free;
end;

function TDPlay.SetPlayerName(pidID: TDPID; lpPlayerFriendlyName: PChar;
        lpPlayerFormalName: PChar) : HResult;
begin
 // Log.Add ('IDPLAY.SetPlayerName');
end;

{--------------------------------------------------------------------}

{function TDPlay.AddPlayerToGroup(idGroup: TDPID; idPlayer: TDPID) : HResult;
begin
  Log.Add ('AddPlayerToGroup');
end;}

{function TDPlay.Close: HResult;
begin
  Log.Add ('Close');
end;}

function TDPlay.CreateGroup(var lpidGroup: TDPID; lpGroupName: PDPName;
        const lpData; dwDataSize: DWORD; dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY2(3).CreateGroup');
end;


function TDPlay.CreatePlayer(var lpidPlayer: TDPID; pPlayerName: PDPName;
        hEvent: THandle; lpData: Pointer; dwDataSize: DWORD; dwFlags: DWORD) :
        HResult;
begin
  Result := dp3.CreatePlayer (lpidPlayer, pPlayerName, hEvent, lpData, dwDataSize, dwFlags);
 { Log.Add ('!IDPLAY2(3).CreatePlayer');
  Log.Add (' + lpidPlayer                : ', lpidPlayer);
  Log.Add (' + pPlayerName.lpszLongName  : ' + pPlayerName^.lpszLongName);
  Log.Add (' + pPlayerName.lpszShortName : ' + pPlayerName^.lpszShortName);
  Log.Add (' + hEvent                    : ', hEvent);
  Log.Add (' + lpData                    : ', lpData, dwDataSize);
  Log.Add (' + dwDataSize                : ', dwDataSize);
  Log.Add (' + dwFlags                   : ', dwFlags); }
end;

{function TDPlay.DeletePlayerFromGroup(idGroup: TDPID; idPlayer: TDPID) : HResult;
begin
  Log.Add ('DeletePlayerFromGroup');
end;}

{function TDPlay.DestroyGroup(idGroup: TDPID) : HResult;
begin
  Log.Add ('DestroyGroup');
end;}

{function TDPlay.DestroyPlayer(idPlayer: TDPID) : HResult;
begin
  Log.Add ('DestroyPlayer');
end;}

function TDPlay.EnumGroupPlayers(idGroup: TDPID; const lpguidInstance: TGUID;
        lpEnumPlayersCallback2: TDPEnumPlayersCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
  //Log.Add ('IDPLAY2(3).EnumGroupPlayers');
end;

function TDPlay.EnumGroups(lpguidInstance: PGUID; lpEnumPlayersCallback2:
        TDPEnumPlayersCallback2; lpContext: Pointer; dwFlags: DWORD) : HResult;
begin
  //Log.Add ('IDPLAY2(3).EnumGroups');
end;

function TDPlay.EnumPlayers(lpguidInstance: PGUID; lpEnumPlayersCallback2:
        TDPEnumPlayersCallback2; lpContext: Pointer; dwFlags: DWORD) : HResult;
begin
  Result := dp3.EnumPlayers (lpguidInstance, lpEnumPlayersCallback2, lpContext, dwFlags);
{  Log.Add ('!IDPLAY2(3).EnumPlayers');
  if lpguidInstance <> nil then
    Log.Add (' + lpguidInstance^ : ', lpguidInstance^)
  else
    Log.Add (' + lpguidInstance : ', lpguidInstance);

  Log.Add (' + lpEnumPlayersCallback2 : ', @lpEnumPlayersCallback2);
  Log.Add (' + lpContext : ', lpContext);
  Log.Add (' + dwFlags : ', dwFlags); }
end;

function TDPlay.EnumSessions(var lpsd: TDPSessionDesc2; dwTimeout: DWORD;
        lpEnumSessionsCallback2: TDPEnumSessionsCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
  Result := dp3.EnumSessions (lpsd, dwTimeout, lpEnumSessionsCallback2, lpContext, dwFlags);
 { Log.Add ('!IDPLAY2(3).EnumSessions');
  Log.Add (' + lpsd.dwFlags            : ', lpsd.dwFlags);
  Log.Add (' + lpsd.guidInstance       : ', lpsd.guidInstance);
  Log.Add (' + lpsd.guidApplication    : ', lpsd.guidApplication);
  Log.Add (' + lpsd.dwMaxPlayers       : ', lpsd.dwMaxPlayers);
  Log.Add (' + lpsd.dwCurrentPlayers   : ', lpsd.dwCurrentPlayers);
  Log.Add (' + lpsd.lpszSessionName    : ' + lpsd.lpszSessionName);
  Log.Add (' + lpsd.lpszPassword       : ' + lpsd.lpszPassWord);
  Log.Add (' + dwTimeout               : ', dwTimeout);
  Log.Add (' + lpEnumSessionsCallback2 : ', @lpenumsessionsCallback2);
  Log.Add (' + lpContext               : ', lpContext);
  Log.Add (' + dwFlags                 : ', dwFlags); }
end;

function TDPlay.GetCaps(var lpDPCaps: TDPCaps; dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY2(3).GetCaps');
end;

function TDPlay.GetGroupData(idGroup: TDPID; lpData: Pointer; var lpdwDataSize: DWORD;
        dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY2(3).GetGroupData');
end;

function TDPlay.GetGroupName(idGroup: TDPID; lpData: Pointer; var lpdwDataSize: DWORD) :
        HResult;
begin
 // Log.Add ('IDPLAY2(3).GetGroupName');
end;

{function TDPlay.GetMessageCount(idPlayer: TDPID; var lpdwCount: DWORD) : HResult;
begin
  Log.Add ('GetMessageCount');
end;}

function TDPlay.GetPlayerAddress(idPlayer: TDPID; lpAddress: Pointer;
        var lpdwAddressSize: DWORD) : HResult;
begin
  Result := dp3.GetPlayerAddress (idPlayer, lpAddress, lpdwAddressSize);
 { Log.Add ('!IDPLAY2(3).GetPlayerAddress');  }
end;

function TDPlay.GetPlayerCaps(idPlayer: TDPID; var lpPlayerCaps: TDPCaps;
        dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY2(3).GetPlayerCaps');
end;

function TDPlay.GetPlayerData(idPlayer: TDPID; lpData: Pointer; var lpdwDataSize: DWORD;
        dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY2(3).GetPlayerData');
end;

function TDPlay.GetPlayerName(idPlayer: TDPID; lpData: Pointer; var lpdwDataSize: DWORD)
        : HResult;
type
  pdata = ^tdata;
  tdata = array[1..100000] of char;
var
  a,b  :integer;
  p  :pdata;
  ip :pointer;
  iplen :longword;
  s     :string;
begin
  Result := dp3.GetPlayerName (idPlayer, lpData, lpdwDataSize);
  numplayers:=numplayers+1;
  playerId[numplayers]:=idPlayer;
  p:=lpData;
  a:=lpdwDatasize-2;
  while(p^[a]<>#$0) do
    a:=a-1;
  a:=a+1;
  playername[numplayers] := '';
  while(p^[a]<>#$0) do
  begin
    playername[numplayers]:=playername[numplayers]+p^[a];
    a:=a+1;
  end;
  playerside[numplayers]:=0;
  playercolor[numplayers]:=numplayers;
  if givebase[1] then begin
    if imserver then
      sendchat('New player. Quick base toggled off');
    for a:=1 to 10 do
      givebase[a]:=false;
  end;
  votedgo[numplayers]:=false;
  b:=0;
  for a:=1 to numplayers do
    if votedgo[a] then
      inc(b);
  if (b>=numplayers-1) and (b>1) then
    forcego:=true
  else forcego:=false;
  //Tillägg för att spara ip-adressen
  GetMem (ip, 1000);
  iplen := 1000;
  dp3.GetPlayerAddress (idPlayer, ip, iplen);
  playerip[numplayers] := PtrToStr (ip, iplen);
  FreeMem (ip, 1000);
  //tillägg för att hitta yxor
  if numplayers>1 then begin
    if pos('yxan',lowercase(playername[numplayers]))>0 then
      yxdetected:=true;
    if pos('130.237.226.',playerip[numplayers])>0 then
      yxdetected:=true;
  end;
  //tillägg för skicka namn till mk
  if not assigned(chatview) then
    exit;
  s:=playername[numplayers];
  for a:=1 to length(playername[numplayers]) do begin
    chatview^.playernames[numplayers][a]:=s[a];
  end;
  chatview^.playernames[numplayers][length(playername[numplayers])+1]:=#0;
end;

function TDPlay.GetSessionDesc(lpData: Pointer; var lpdwDataSize: DWORD) : HResult;
var
  lpsd :TDPSessionDesc2;
begin
  Result := dp3.GetSessionDesc (lpData, lpdwDataSize);
//  Log.Add ('!IDPLAY2(3).GetSessionDesc');

end;

{function TDPlay.Initialize(const lpGUID: TGUID) : HResult;
begin
  Log.Add ('Initialize');
end;}

function TDPlay.Open(var lpsd: TDPSessionDesc2; dwFlags: DWORD) : HResult;
begin
  Result := dp3.Open (lpsd, dwFlags);

  ResetRec;

  if dwflags=2 then
    imserver:=true;
end;

function TDPlay.Receive(var lpidFrom: TDPID; var lpidTo: TDPID; dwFlags: DWORD;
        lpData: Pointer; var lpdwDataSize: DWORD) : HResult;
type
  pdata = ^tdata;
  tdata = array[1..10000] of char;
var
   p      :TPacket;
   a,b,c   :integer;
   point  : pdata;
   s      :string;
   wp     :^word;
   w      :word;
   ip     :^cardinal;
   holdstring :string;
   bufsize :integer;
   bogustill :cardinal;
begin
  if alloff then begin
    Result := dp3.Receive (lpidFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
    exit;
  end;
  bufsize := lpdwDataSize;
  try
{    if stringbuf.Count <> 0 then
      SendChat ('');}


    if stringbuf.count <> 0 then
    begin
      lpidFrom:=playerid[GetGoodSource];
    end else begin
      if takefrom<>0 then begin
        log.add('inserting give data');
        lpidFrom:=takefrom;

        holdstring:='123456';
        wp:=@holdstring[1];
        w:=takeunit+startid[convertid(takefrom)];
        wp^:=w;
        ip:=@holdstring[3];
        ip^:=playerid[1];                                                          //ändrat från 7E
        holdstring:=#$14+holdstring+#$00#$00#$00#$00+'F'+#$01#$00#$00#$00#$00+'d'+#$7F#$00#$00#$00#$00#$00;
        setword(@holdstring[12], health[w - 1]);

        //Bara take på unitar som är färdigbyggda
        if donestatus [w - 1] = 0 then
          SendLocal (holdstring, 0, true, false); //kopierar den hit och ändrar nedan till maxunits-2 från maxunits-1

        if takeunit>=maxunits-2 then begin
          log.add('rejecting');
          holdstring:='1234'+#$06;
          ip:=@holdstring[1];
          ip^:=takefrom;
          holdstring:=#$1b+holdstring;
          sendlocal(holdstring,0,false,true);
          takefrom := 0;
        end;
        takeunit := takeunit + 1;

  //      SendLocal (holdstring, 0, true, false);
  //      packetwaiting:=true;
      end;
    end;

    if stringbuf.count <> 0 then
    begin
      holdstring := stringbuf.Strings [0];

      if length (holdstring)+10 > lpdwdatasize then
      begin
//        SendChat ('Overflow correction 1');
        lpdwdatasize := length (holdstring) + 10;
        Result := DPERR_BUFFERTOOSMALL;
        exit;
      end;

      stringbuf.Delete (0);

      p:=TPacket.sjcreatenew(holdstring);
      holdstring:=p.tadata;

      p.free;
      lpidTo:=playerid[1];
      lpdwDataSize:=length(holdstring);
      point:=lpdata;
      a:=1;
      while a<=lpdwDatasize do begin
        point^[a]:=holdstring[a];
        inc(a);
      end;

      Log.Add ('final : ', point, lpdwdatasize);
      result:=DP_OK;
      exit;
    end;

    Result := dp3.Receive (lpidFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
    if Result = DP_OK then begin
      if lpidfrom=0 then begin
        if (dword(lpdata^)=DPSYS_DESTROYPLAYERORGROUP) and (tastatus=0)then
        begin
          Log.Add ('Begin processing sys');
          point:=lpdata;

          b := PDPMsg_DestroyPlayerGroup (lpData)^.tdpid;
          a:=convertid(b);
          Log.Add ('Player ' + inttostr (a) + ' left');
          {$IFNDEF RELEASE}
          if numplayers>2 then
            sendchat('player ' + playername[a] + ' left');
          {$ENDIF}
          if a<numplayers then begin
            for b:=a to numplayers-1 do begin
              playerId[b]:=playerId[b+1];
              playername[b]:=playername[b+1];
              playerside[b]:=playerside[b+1];
              playercolor[b]:=playercolor[b+1];
              laststatmess[b]:=laststatmess[b+1];
              playerip[b] := playerip[b+1];

              cantake[b] := cantake[b+1];
              chatview^.allies[b]:=chatview^.allies[b+1];
              lastmsg [b] := lastmsg[b+1];
              lastpacket[b] := lastpacket[b+1];
              votedgo[b] := votedgo[b+1];
              clickedin[b] := clickedin[b+1];
              enemychat[b] := enemychat[b+1];
              recConnect[b] := recconnect[b+1];
              internver[b] := internver[b+1];
              hasWarned[b] := haswarned[b+1];
              for c:=1 to 20 do
                chatview^.playernames[b][c]:=chatview^.playernames[b+1][c];
              chatview^.allies[b]:=chatview^.allies[b+1];
              chatview^.deathtimes[b]:=chatview^.deathtimes[b+1];
              chatview^.playerColors[b]:=chatview^.playerColors[b+1];
            end;
          end;
          dec(numplayers);
          Log.Add ('End processing sys');
        end;
      end else
      begin
        s := PtrToStr (lpdata, lpdwDataSize);
        s := PacketHandler (s, lpidfrom, bogustill);
        if datachanged then begin
          datachanged := false;

//          Log.Add ('bufsize : ' + inttostr (bufsize));
//          log.add ('packet  : ' + inttostr (length (s)));
          if length (s) > bufsize then
          begin
            {$IFNDEF release}
            Log.add ('overflow correction 2');
            SendChat ('Overflow correction 2');
            {$ENDIF}
            lpdwdatasize := length (s) + 10;
            Result := DPERR_BUFFERTOOSMALL;
            p := TPacket.Create (@s[1], Length (s));

            SendLocal (p.rawdata2, 0, true, false);
            p.free;
            exit;
          end;

          point:=lpdata;
          lpdwDataSize:=length(s);
          for a:=1 to length(s) do
            point^[a]:=s[a];
        end;
      end;
    end;
  except
    on E: Exception do
    begin
      sendchat ('Exception caught in TDPlay.Receive ');
      sendchat (E.Message);
      if crash then
        raise
      else
        ExceptMessage;
    end;
//        Result := DP_OK;
  end;
end;

{function TDPlay.Send(idFrom: TDPID; lpidTo: TDPID; dwFlags: DWORD; const lpData;
        lpdwDataSize: DWORD) : HResult;
begin
  Log.Add ('Send');
end;}

function TDPlay.SetGroupData(idGroup: TDPID; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY2(3).SetGroupData');
end;

function TDPlay.SetGroupName(idGroup: TDPID; lpGroupName: PDPName;
        dwFlags: DWORD) : HResult;
begin
  //Log.Add ('IDPLAY2(3).SetGroupName');
end;

function TDPlay.SetPlayerData(idPlayer: TDPID; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY2(3).SetPlayerData');
end;

function TDPlay.SetPlayerName(idPlayer: TDPID; lpPlayerName: PDPName;
        dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY2(3).SetPlayerName');
end;

function TDPlay.SetSessionDesc(const lpSessDesc: TDPSessionDesc2; dwFlags: DWORD) :
        HResult;
var
  lpsd :TDPSessionDesc2;
begin
  lpsd := lpSessDesc;

  Result := dp3.SetSessionDesc (lpSessDesc, dwFlags);
{  Log.Add ('!IDPLAY2(3).SetSessionDesc');
  Log.Add (' + lpsd.dwFlags            : ', lpsd.dwFlags);
  Log.Add (' + lpsd.guidInstance       : ', lpsd.guidInstance);
  Log.Add (' + lpsd.guidApplication    : ', lpsd.guidApplication);
  Log.Add (' + lpsd.dwMaxPlayers       : ', lpsd.dwMaxPlayers);
  Log.Add (' + lpsd.dwCurrentPlayers   : ', lpsd.dwCurrentPlayers);
  Log.Add (' + lpsd.lpszSessionName    : ' + lpsd.lpszSessionName);
//  Log.Add (' + lpsd.lpszPassword       : ' + lpsd.lpszPassWord);
  Log.Add (' + dwFlags                 : ', dwFlags);  }
end;

{--------------------------------------------------------------------}

function TDPlay.AddGroupToGroup(idParentGroup: TDPID; idGroup: TDPID) : HResult;
begin
 // Log.Add ('IDPLAY3.AddGroupToGroup');
end;

function TDPlay.CreateGroupInGroup(idParentGroup: TDPID; var lpidGroup: TDPID;
        lpGroupName: PDPName; lpData: Pointer; dwDataSize: DWORD;
        dwFlags: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY3.CreateGroupInGroup');
end;

function TDPlay.DeleteGroupFromGroup(idParentGroup: TDPID; idGroup: TDPID) :
        HResult;
begin
 // Log.Add ('IDPLAY3.DeleteGroupFromGroup');
end;

function TDPlay.EnumConnections(const lpguidApplication: TGUID;
        lpEnumCallback: TDPEnumConnectionsCallback; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
  Result := dp3.EnumConnections (lpguidApplication, lpEnumCallback, lpContext, dwFlags);
{  Log.Add ('!IDPLAY3.EnumConnections');
  Log.Add (' + lpguidApplication : ', lpguidapplication);
  Log.Add (' + lpEnumCallback    : ', @lpenumcallback);
  Log.Add (' + lpContext         : ', lpcontext);
  Log.Add (' + dwFlags           : ', dwFlags);  }
end;

function TDPlay.EnumGroupsInGroup(idGroup: TDPID; const lpguidInstance: TGUID;
        lpEnumPlayersCallback2: TDPEnumPlayersCallback2; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY3.EnumGroupsInGroup');
end;

function TDPlay.GetGroupConnectionSettings(dwFlags: DWORD; idGroup: TDPID;
        lpData: Pointer; var lpdwDataSize: DWORD) : HResult;
begin
//  Log.Add ('IDPLAY3.GetGroupConnectionSettings');
end;

function TDPlay.InitializeConnection(var lpConnection: TDPLConnection; dwFlags: DWORD) :
         HResult;
begin
  Result := dp3.InitializeConnection (lpConnection, dwFlags);
{  Log.Add ('!IDPLAY3.InitializeConnection');
  Log.Add (' + lpConnection.dwFlags       : ', lpConnection.dwFlags);
  Log.Add (' + lpConnection.lpSessionDesc : ', lpConnection.lpSessionDesc);
  Log.Add (' + lpConnection.lpPlayerName  : ', lpConnection.lpPlayerName);
  Log.Add (' + lpConnection.guidSP        : ', lpConnection.guidSP);
  Log.Add (' + lpConnection.lpAddress     : ', lpConnection.lpAddress);
  Log.Add (' + lpConnection.dwAddressSize : ', lpConnection.dwAddressSize);  }
end;

function TDPlay.SecureOpen(const lpsd: TDPSessionDesc2; dwFlags: DWORD;
        const lpSecurity: TDPSecurityDesc; const lpCredentials: TDPCredentials)
        : HResult;
begin
 // Log.Add ('IDPLAY3.SecureOpen');
end;

function TDPlay.SendChatMessage(idFrom: TDPID; idTo: TDPID; dwFlags: DWORD;
        const lpChatMessage: TDPChat) : HResult;
begin
 // Log.Add ('IDPLAY3.SendChatMessage');
end;

function TDPlay.SetGroupConnectionSettings(dwFlags: DWORD; idGroup: TDPID;
        const lpConnection: TDPLConnection) : HResult;
begin
 // Log.Add ('IDPLAY3.SetGroupConnectionSettings');
end;

function TDPlay.StartSession(dwFlags: DWORD; idGroup: TDPID) : HResult;
begin
 // Log.Add ('IDPLAY3.StartSession');
end;

function TDPlay.GetGroupFlags(idGroup: TDPID; var lpdwFlags: DWORD) : HResult;
begin
  //Log.Add ('IDPLAY3.GetGroupFlags');
end;

function TDPlay.GetGroupParent(idGroup: TDPID; var lpidParent: TDPID) : HResult;
begin
 // Log.Add ('IDPLAY3.GetGroupParent');
end;

function TDPlay.GetPlayerAccount(idPlayer: TDPID; dwFlags: DWORD; var lpData;
        var lpdwDataSize: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY3.GetPlayerAccount');
end;

function TDPlay.GetPlayerFlags(idPlayer: TDPID; var lpdwFlags: DWORD) : HResult;
begin
 // Log.Add ('IDPLAY3.GetPlayerFlags');
end;

{function TDPlay.QueryInterface(const IID: TGUID; out Obj): HResult; stdcall;
begin
  Log.Add ('QueryInterface');
end;

function TDPlay._AddRef: Integer; stdcall;
begin
  Log.Add ('AddRef');
end;

function TDPlay._Release: Integer; stdcall;
begin
  Log.Add ('Release');
end;}


{function TDPlay.QueryInterface(const IID: TGUID; out Obj): HResult; stdcall;
begin
  Log.Add ('IDPLAY.QueryInterface');
  if IsSameGuid (IID, IID_IDirectPlay3) then
  begin
    Log.Add ('IDPLAY + IID_IDirectPlay3');
    TDPlay3(Obj) := TDPlay3.Create;
    Log.Add ('IDPLAY + Created');
    Result := DP_OK;
  end else
  begin
    Log.Add ('IDPLAY + Unsupported interface requested');
    Result := DPERR_NOINTERFACE;
  end;
end;

function TDPlay._AddRef: Integer; stdcall;
begin
  Log.Add ('IDPLAY._AddRef');
  Result := 2;
end;

function TDPlay._Release: Integer; stdcall;
begin
  Log.Add ('IDPLAY._Release');
end;}

{--------------------------------------------------------------------}

procedure TDPlay.ResetRec;
var
  a,b         : integer;
  f         : Tfilestream;
  p         : pointer;
  reg       :TRegIniFile;
begin
  reg := TRegInifile.Create ('Software\Yankspankers\TA Demo');

  servernumber:=10;
  tal := 0;
  numplayers :=0;
  filterpack:=255;
  tastatus:=0;
  imserver:=false;
  islogging:=false;

  fixfacexps := reg.ReadBool ('Options', 'fixall', true);
  protectdt := reg.ReadBool ('Options', 'fixall', true);
  shareMapPos:=reg.ReadBool ('Options', 'sharepos', false);
  createTxtFile:=reg.ReadBool ('Options', 'createtxt', false);

  logpl:=false;
  autorec := reg.ReadBool ('Options', 'autorec', false);
  auto3d := reg.ReadBool ('Options', 'ta3d', true);
  usecomp := reg.ReadBool ('Options', 'usecomp', true);
  playernames := reg.ReadBool ('Options', 'playernames', false);
  serverdir := FixPath (reg.ReadString ('Options', 'serverdir', ''));
  demodir := FixPath (reg.ReadString ('Options', 'defdir', ''));

  crash := false;
  alloff := false;

  fixon:=true;
  baseon:=true;
//  packetwaiting:=false;
  onlyunits:=false;
  notime:=true;
  fakecd:=false;
  fakewatch:=false;
  filename:='';
  unitdata:='';
  takefrom:=0;
  fastspeed:=0;
  slowspeed:=0;
  nonserverstart:=true;
  use3d := false;
  yxdetected:=false;
  cmdcont:=true;
  oldMyCheats:=0;
  nextCheatCheck:=5;
  compatibleTA:=false;
  for a:=0 to UNITSPACE do
  begin
    lastdead[a]:=0;
    health[a] := 42;
  end;
  forcego:=false;
  for a:=1 to 10 do begin
    otherCheats[a]:=0;
    numunitsalive[a]:=0;
    startx[a]:=0;
    givebase[a]:=false;
    votedgo[a]:=false;
    cantake[a]:=false;
    warpdone[a]:=false;
    startid[a]:=100000;
    goodpackets[a]:=0;
    lostpackets[a]:=0;
    lastpacket[a]:=0;
    for b:=1 to 100 do
      lateplhist[a,b]:=false;
    for b:=0 to 10 do
      plhist[a,b]:=0;
    plhistindex[a]:=1;
    losscount[a]:=0;
    maxloss[a]:=0;
    sharede[a]:=0;
    sharedm[a]:=0;
    totalsharedm[a]:=0;
    totalsharede[a]:=0;
    recConnect[a]:=false;
    internver[a] := 0;
    enemychat[a]:=false;
    haswarned[a] := false;
  end;

  for a := 0 to UNITSPACE do
  begin;
    donestatus[a] := 1000;
    unitalive[a]:=false;
  end;
  takefrom:=0;
  expcrc:=getcrc();
  log.add('initialization finished');
  waitsync:=0;
  chatlog := '';
  crcattempts:=0;

  ai := '';
  stringbuf := TStringlist.Create;
  norecord := false;
  staton := false;

  tempstorage := TStringList.Create;

  for a := 1 to 10 do
    curspeeds [a] := 10;

  reg.free;
end;

procedure TDPlay.ExceptMessage;
begin
  SendLocal (#$19#$00#$01, 0, true, true);
  SendChat ('The affected person is: ' + playername [1]);
  SendChat ('The recorder has caused an illegal operation. It is possible');
  SendChat ('that it will continue to do so until you shut it down. If');
  SendChat ('you want to help us fix this bug, do the following:');
  SendChat (' 1)  Type .crash and unpause. This will cause TA to quit.');
  SendChat (' 2)  Send the file "errorlog.txt" from your TA dir to us.');
  SendChat ('     Please send the file immediately after TA exits.');
  SendChat ('You can shutdown the recorder by typing .panic (hopefully)')
end;

function TDPlay.GetGoodSource :integer;
var
  cur :integer;
  i   :integer;
  max :integer;
begin
  max := 0;
  cur := 2;
  for i := 2 to numplayers do
  begin
    if lastmsg [i] > max then
    begin
      max := lastmsg [i];
      cur := i;
    end;
  end;

  Result := cur;
end;

constructor TDPlay.Create (realdp :IDirectPlay);
var
   a        :integer;
begin
  inherited Create;

//  sysbufl:=GetSystemDirectory(@sysbuf[1],100);
//  sysdir:=copy(sysbuf,1,sysbufl);

  dp1 := realdp;
  dp1.QueryInterface (IID_IDirectPlay3, dp3);

  ResetRec;

  hMemMap := CreateFileMapping($ffffffff,0,PAGE_READWRITE,0,sizeof(MKChatMem),'TADemo-MKChat');
  chatview := MapViewOfFile(hMemMap,FILE_MAP_ALL_ACCESS,0,0,sizeof(MKChatMem));
  chatview^.dataexists:=0;
  for a:=1 to 10 do begin
    chatview^.deathtimes[a]:=0;
    chatview^.playernames[a][1]:=#0;
    chatview^.allies[a]:=0;
  end;
  chatview^.tastatus:=1;
  chatview^.playingDemo:=0;
  chatview^.myCheats:=0;
end;

//----------------------------------------------

function TLobby.Connect(dwFlags: DWORD; var lplpDP: IDirectPlay2;
        pUnk: IUnknown) : HResult;
var
  dp :IDirectPlay2;
  dp1 :IDirectPlay;
begin
  Result := lobby2.Connect (dwFlags, dp, pUnk);
  Log.Add ('LOBBY.Connect');
  Log.Add (' + dwFlags : ', dwFlags);

  dp.QueryInterface (IID_IDirectPlay, dp1);
  lplpdp := TDPlay.Create (dp1);
  startedfrom := 'TLobby.Connect';
end;

function TLobby.CreateAddress(const guidSP, guidDataType: TGUID; const lpData;
        dwDataSize: DWORD; var lpAddress; var lpdwAddressSize: DWORD) : HResult;
begin
  Result := lobby2.CreateAddress (guidSp, guiddatatype, lpdata, dwdatasize, lpaddress, lpdwaddresssize);
  Log.Add ('LOBBY.CreateAddress');
end;

function TLobby.EnumAddress(lpEnumAddressCallback: TDPEnumAdressCallback;
        const lpAddress; dwAddressSize: DWORD; lpContext : Pointer) : HResult;
begin
  Result := lobby2.enumaddress (lpenumaddresscallback, lpaddress, dwaddresssize, lpcontext);
  Log.Add ('LOBBY.EnumAddress');
end;

function TLobby.EnumAddressTypes(lpEnumAddressTypeCallback:
        TDPLEnumAddressTypesCallback; const guidSP: TGUID; lpContext: Pointer;
        dwFlags: DWORD) : HResult;
begin
  result := lobby2.enumaddresstypes (lpenumaddresstypecallback, guidsp, lpcontext, dwflags);
  Log.Add ('LOBBY.EnumAddressTypes');
end;

function TLobby.EnumLocalApplications(lpEnumLocalAppCallback:
        TDPLEnumLocalApplicationsCallback; lpContext: Pointer; dwFlags: DWORD)
        : HResult;
begin
  result := lobby2.EnumLocalApplications (lpenumlocalappcallback, lpcontext, dwflags);
  Log.Add ('LOBBY.EnumLocalApplications');
end;

function TLobby.GetConnectionSettings(dwAppID: DWORD; lpData: PDPLConnection;
        var lpdwDataSize: DWORD) : HResult;
begin
  result := lobby2.getconnectionsettings (dwappid, lpdata, lpdwdatasize);
  Log.Add ('LOBBY.GetConnectionSettings');
end;

function TLobby.ReceiveLobbyMessage(dwFlags: DWORD; dwAppID: DWORD;
        var lpdwMessageFlags: DWORD; lpData: Pointer; var lpdwDataSize: DWORD) :
        HResult;
begin
  result := lobby2.ReceiveLobbyMessage (dwflags, dwappid, lpdwmessageflags, lpdata, lpdwdatasize);

  Log.Add ('LOBBY.ReceiveLobbyMessage');
  if result = DP_OK then
  begin
    Log.add (' + dwFlags          : ', dwflags);
    Log.Add (' + lpdwmessageflags : ', lpdwmessageflags);
    Log.Add (' + dwappid          : ', dwappid);
    Log.Add (' + lpdwdatasize     : ', lpdwdatasize);
    log.add (' + lpdata           : ', lpdata, lpdwDataSize);
  end else
    Log.Add ('Error');
end;

function TLobby.RunApplication(dwFlags: DWORD; var lpdwAppId: DWORD;
        const lpConn: TDPLConnection; hReceiveEvent: THandle) : HResult;
begin
  result := lobby2.RunApplication (dwflags, lpdwappid, lpconn, hreceiveevent);
  Log.Add ('LOBBY.RunApplication');
  Log.Add (' + dwFlags                                 : ' , dwFlags);
  Log.Add (' + lpdwappid                               : ', lpdwappid);
  Log.Add (' + lpconn.dwflags                          :', lpconn.dwflags);

  Log.Add (' + lconn.lpsessiondesc.dwFlags             : ', lpconn.lpsessiondesc.dwFlags);
  Log.Add (' + lpconn.lpsessiondesc.guidInstance       : ', lpconn.lpsessiondesc.guidInstance);
  Log.Add (' + lpconn.lpsessiondesc.guidApplication    : ', lpconn.lpsessiondesc.guidApplication);
  Log.Add (' + lpconn.lpsessiondesc.dwMaxPlayers       : ', lpconn.lpsessiondesc.dwMaxPlayers);
  Log.Add (' + lpconn.lpsessiondesc.dwCurrentPlayers   : ', lpconn.lpsessiondesc.dwCurrentPlayers);
  Log.Add (' + lpconn.lpsessiondesc.lpszSessionName    : ' + lpconn.lpsessiondesc.lpszSessionName);
  Log.Add (' + lpconn.dwUser1                          : ', lpconn.lpsessiondesc.dwUser1);
  Log.Add (' + lpconn.dwUser2                          : ', lpconn.lpsessiondesc.dwUser2);
  Log.Add (' + lpconn.dwUser3                          : ', lpconn.lpsessiondesc.dwUser3);
  Log.Add (' + lpconn.dwUser4                          : ', lpconn.lpsessiondesc.dwUser4);

  Log.Add (' + lpconn.pPlayerName.lpszLongName         : ' + lpconn.lpPlayerName.lpszLongName);
  Log.Add (' + lpconn.pPlayerName.lpszShortName        : ' + lpconn.lpPlayerName.lpszShortName);

  Log.Add (' + lpconn.guidsp                           : ', lpconn.guidsp);
  log.add (' + lpconn.lpaddress                        : ', lpconn.lpaddress);
  log.add (' + lpconn.dwaddresssize                    : ', lpconn.dwaddresssize);

  Log.add (' + hreceiveevent                           : ', hreceiveevent);
end;

function TLobby.SendLobbyMessage(dwFlags: DWORD; dwAppID: DWORD; const lpData;
        dwDataSize: DWORD) : HResult;
begin
  result := lobby2.SendLobbyMessage (dwflags, dwappid, lpdata, dwdatasize);
  Log.Add ('LOBBY.SendLobbyMessage');
  if result = DP_OK then
  begin
    Log.add (' + dwFlags    : ', dwflags);
    Log.Add (' + dwappid    : ', dwappid);
    Log.add (' + dwdatasize : ', dwdatasize);
    log.add (' + lpdata     :', @lpdata, dwDataSize);
  end else
    Log.Add ('Error');
end;

function TLobby.SetConnectionSettings(dwFlags: DWORD; dwAppID: DWORD;
        const lpConn: TDPLConnection) : HResult;
begin
  result := lobby2.SetConnectionSettings (dwflags, dwappid, lpconn);
  Log.Add ('LOBBY.SetConnectionSettings');
end;

function TLobby.SetLobbyMessageEvent(dwFlags: DWORD; dwAppID: DWORD;
        hReceiveEvent: THandle) : HResult;
begin
  result := lobby2.SetLobbyMessageEvent (dwflags, dwappid, hreceiveevent);
  Log.Add ('LOBBY.SetLobbyMessageEvent');
end;

{--------------------------------------------------------------------}

function TLobby.CreateCompoundAddress(const lpElements: TDPCompoundAddressElement;
        dwElementCount: DWORD; lpAddress: Pointer; var lpdwAddressSize: DWORD) :
        HResult;
begin
  result := lobby2.CreateCompoundAddress (lpelements, dwelementcount, lpaddress, lpdwaddresssize);
  Log.Add ('LOBBY2.CreateCompoundAddress');
end;

{--------------------------------------------------------------------}

constructor TLobby.Create (reallobby :IDirectPlayLobby);
begin
  inherited Create;

  lobby1 := reallobby;
  lobby1.QueryInterface (IID_IDirectPlayLobby2, lobby2);

  Log.Add ('TLobby.Create');
end;

{function TDPlay.CreateGroup(var lpidGroup: TDPID; lpGroupName: PDPName;
  const lpData; dwDataSize, dwFlags: DWORD): HResult;
begin

end;

function TDPlay.Receive(var lppidFrom, lppidTo: TDPID; dwFlags: DWORD;
  var lpvBuffer; var lpdwSize: DWORD): HResult;
begin

end;

function TDPlay.Send(idFrom, lpidTo: TDPID; dwFlags: DWORD; const lpData;
  lpdwDataSize: DWORD): HResult;
begin

end;
}
end.
