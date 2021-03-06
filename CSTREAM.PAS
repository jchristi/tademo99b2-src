unit cstream;

interface

uses
  classes, sysutils;

//Anv�nd den inte �t b�da h�llen samtidigt!! Antingen l�s eller skriv! Or else..
type
  TBufStream = class (TFileStream)
  protected
    Eof :boolean;
    Buf :pointer;
    CurSize :integer;         //Antal bytes som finns i buffern l�sta fr�n fil
    CurPtr :integer;
    writing :boolean;         //true om vi anv�nt write, s� destroy can flusha
  public
    constructor Create(const FileName: string; Mode: Word);
    destructor Destroy; override;
    function Read(var Buffer; Count: Longint): Longint; override;
    function Write(const Buffer; Count: Longint): Longint; override;
    function Seek(Offset: Longint; Origin: Word): Longint; override;
  end;

implementation

const
  BUF_SIZE = 50000;

type
  PBuf = ^TBuf;
  TBuf = array[0..BUF_SIZE-1] of byte;

constructor TBufStream.Create (const Filename :string; Mode :word);
begin
  GetMem (Buf, BUF_SIZE);
  CurSize := 0;
  CurPtr := 0;
  Eof := false;
  writing := false;

  inherited Create (FileName, Mode);
end;

destructor TBufStream.Destroy;
begin
  if writing then
    inherited Write (Buf^, CurPtr);    //S� att inget blir kvar sen p� slutet.

  FreeMem (Buf, BUF_SIZE);

  inherited Destroy;
end;

function TBufStream.Read (var Buffer; Count :longint): longint;
var
  sent :integer;      //Antal bytes vi lagt i buffern
begin
  if Eof then
  begin
    Result := 0;
    exit;
  end;

  sent := 0;

  while sent < Count do
  begin
    if (Count - Sent) + CurPtr < CurSize then      //Normala fallet, allt finns att h�mta ur buffern
    begin
      Move (PBuf(Buf)^[CurPtr], TBuf(Buffer)[Sent], Count - Sent);
      Inc (CurPtr, Count - Sent);
      Result := Count;
      exit;
    end;

    //Kommer vi hit finns det f�r lite data i buffern
    Move (PBuf(Buf)^[CurPtr], TBuf(Buffer)[Sent], CurSize - CurPtr);
    Inc (Sent, CurSize - CurPtr);

    CurSize := inherited Read (Buf^, BUF_SIZE);

    if CurSize = 0 then    //Inneb�r att filen �r slut
    begin
      EOF := true;
      Result := Sent;
      exit;
    end;

    CurPtr := 0;
  end;
end;

function TBufStream.Write(const Buffer; Count: Longint): Longint;
begin
  writing := true;

  if Count + CurPtr < BUF_SIZE then     //Buffra detta
  begin
    Move (Buffer, PBuf(Buf)^[CurPtr], Count);
    Inc (CurPtr, Count);
    Result := Count;
    exit;
  end;

  //�r vi h�r m�ste vi dumpa buffern till disk
  inherited Write (Buf^, CurPtr);
  CurPtr := 0;

  if Count > BUF_SIZE then              //Hmm ingen id� att buffra detta
  begin
    inherited Write (Buffer, Count);
    Result := count;
    exit;
  end;

  //L�gg in f�rst i buffern
  Move (Buffer, Buf^, Count);
  CurPtr := Count;
  Result := Count;
  exit;
end;

//Inte supportat.
function TBufStream.Seek(Offset: Longint; Origin: Word): Longint;
begin
  raise Exception.Create ('Seek not implemented for TBufStream');
end;

end.
