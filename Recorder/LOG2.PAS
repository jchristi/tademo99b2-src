unit log2;

interface

uses
  classes,cstream;

type
  TLog2 = class
  private
    FileName : string;
//    LogFile  : File;
    LogFile :TBufStream;
  public
    crc :longword;
    docrc :boolean;
    constructor Create (Filename :string);
    destructor Destroy; override;

    procedure Add (st :string); overload;
  end;

implementation

uses
  SysUtils, textdata;

constructor TLog2.Create (Filename :string);
begin
  inherited Create;
  {$I-}

  Self.Filename := Filename;

  LogFile := TBufStream.Create (Filename, fmCreate or fmShareExclusive);
  crc := 0;
  docrc := false;
//  Assign (LogFile, Filename);
//  Rewrite (Logfile,1);

  {$I+}

end;


procedure TLog2.Add (st :String);
var
  i :integer;
begin
  {$I-}

  LogFile.Write (st[1], Length (st));

  if docrc then
  begin
    crc := CalcCRC (crc, @st[1], length (st));
  end;
//  BlockWrite(Logfile,st[1],length(st));


  {$I+}
end;

destructor TLog2.Destroy;
begin
  {$I-}
  Logfile.Free;
//  CloseFile(LogFile);
  {$I+}
  inherited Destroy;
end;

end.
