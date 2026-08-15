DisableFog = false
LastFog    = true

NormalFOV  = 85
LastZoomed = false
Zoom       = 1


-------------------------------------------------------------------------------
function KeyEvent(Key, Action, Value)

  --Make sure we have a player, the console is closed, and
  --the game menu is closed.
  if (PC.Player == nill) or PC.Player.Console.bTyping or
    PC.Player.GUIController.bVisible then
    return false
  end
  

  --Letter 'O' key toggles fog
  if Key == 0x4F then
    if Action == 1 then
      DisableFog = not DisableFog
    end
    return true
  end


  --MouseWheelUp zooms in
  if Key == 0xEC then
    if Action == 1 then
      Zoom = Zoom / 2
      if Zoom < 0.01 then
        Zoom = Zoom * 2
      end
    end
    return true
  end


  --MouseWheelDown zooms out
  if Key == 0xED then
    if Action == 1 then
      Zoom = Zoom * 2
      if Zoom > 1 then
        Zoom = 1
      end
    end
    return true
  end


  return false
end


-------------------------------------------------------------------------------
function PostRender(Canvas)

  --Make sure we are playing
  LocalPRI = PC.PlayerReplicationInfo
  if LocalPRI == nill then
    return
  end


  --Get the base weapon attachments
  BWAData = {}
  local Index = 1
  local BWA = Engine.FindFirstAActor(CBWA)
  while BWA do
    BWAData[Index] = BWA
    Index = Index + 1
    
    BWA = Engine.FindNextAActor(BWA, CBWA)
  end
  

  --Draw Aiming Dot
  Canvas.SetDrawColor(255, 255, 0, 255)
  Canvas.SetPos((Canvas.SizeX / 2) - 1, (Canvas.SizeY / 2) - 1)
  Canvas.DrawTile(TWhite, 2, 2, 0, 0, 2, 2, Canvas.DrawColor)
  
  
  --Draw Pickup ESP
  local Pickup = Engine.FindFirstAActor(CPickup)
  while Pickup do
    DrawPickupESP(Canvas, Pickup)
    Pickup = Engine.FindNextAActor(Pickup, CPickup)
  end
  
  
  --Draw Player ESP
  local PRI = Engine.FindFirst(LocalPRI)
  while PRI do
    if #PRI ~= #LocalPRI then
      DrawPlayerESP(Canvas, PRI)
    end  
    PRI = Engine.FindNext(PRI)
  end


  --Restore Visuals
  PC.Region.Zone.bDistanceFog = LastFog
  PC.Region.Zone.bClearToFogColor = LastFog
end


-------------------------------------------------------------------------------
function PreRender(Canvas)

  --Get the view angles.
  local Vec = Engine.New(FVector)
  local Rot = Engine.New(FRotator)

  ViewVec, ViewRot = Canvas.GetCameraLocation(Vec, Rot)
  ViewAxesX, ViewAxesY, ViewAxesZ = Canvas.GetAxes(ViewRot,
    Vec, Vec, Vec)


  --Set the new FOV
  if PC.Pawn and PC.Pawn.bPawnZoomed then
    if LastZoomed then
      if NormalFOV == PC.DefaultFOV then
        NormalFOV = PC.DesiredFOV
        Zoom = NormalFOV / PC.DefaultFOV
      else
        PC.DesiredFOV = PC.DefaultFOV * Zoom
      end
    else
      LastZoomed    = true
      NormalFOV     = PC.DefaultFOV
      PC.DesiredFOV = NormalFOV
      PC.FovAngle   = NormalFOV
    end
  else
    if LastZoomed then
      LastZoomed = false
      Zoom = 1
    end
    NormalFOV     = PC.DefaultFOV
    PC.DesiredFOV = NormalFOV * Zoom
    PC.FovAngle   = PC.DesiredFOV
  end
  

  --Modify Visuals
  LastFog = PC.Region.Zone.bDistanceFog
  if DisableFog then
    PC.Region.Zone.bDistanceFog = false
    PC.Region.Zone.bClearToFogColor = false
  end
end


-------------------------------------------------------------------------------
function Tick(DeltaTime, ViewPort)

  --Save the PlayerController
  PC = ViewPort.Actor
  

  --Check for a new level.
  if LevelName ~= PC.Level.sLevelName then
    LevelName = PC.Level.sLevelName
    print('New level: '..LevelName)
    
    FRotator = Engine.UObjects['Struct Object.Rotator']
    FVector  = Engine.UObjects['Struct Object.Vector']
    
    CBWA         = Engine.UObjects['Class AGP.BaseWeaponAttachment']
    CPickup      = Engine.UObjects['Class Engine.Pickup']
    CThrowPickup = Engine.UObjects['Class AGP.ThrowWeaponPickup']
    
    TWhite = Engine.UObjects['Texture Engine.WhiteSquareTexture']
    

    --Perfect Aim
    Fire = Engine.UObjects['Function AGP_Weapon.Fire']
    --Fire[0x15D] = 0x27
    --Fire[0x184] = 0x27
  end  
end


-------------------------------------------------------------------------------
function DrawPickupESP(Canvas, Pickup)
  LastPickup = Pickup
  

  --Calc the pickup's screen position
  local Vec = Engine.New(FVector)
  Vec.X = Pickup.Location.X
  Vec.Y = Pickup.Location.Y
  Vec.Z = Pickup.Location.Z
  local Screen = Canvas.WorldToScreen(Vec)
  if Screen.Z >= 1 then
    return
  end
  
  
  --Set the pickup's color
  local Size = 20
  if Engine.IsA(Pickup, CThrowPickup) then
    Size = 10
    Canvas.SetDrawColor(0, 255, 255, 255)
  else
    Canvas.SetDrawColor(0, 0, 255, 255)
  end


  --Calc the pickup's distance.
  local Delta = Canvas.Subtract_VectorVector(Vec, ViewVec)
  local Dist  = Canvas.VSize(Delta)

  
  --Calc the pickup's screen size.
  Vec.X = ViewVec.X + (Dist * ViewAxesX.X)
  Vec.Y = ViewVec.Y + (Dist * ViewAxesX.Y)
  Vec.Z = ViewVec.Z + (Dist * ViewAxesX.Z)
  local Box = Canvas.WorldToScreen(Vec)

  Vec.X = Vec.X + (Size * ViewAxesZ.X)
  Vec.Y = Vec.Y + (Size * ViewAxesZ.Y)
  Vec.Z = Vec.Z + (Size * ViewAxesZ.Z)
  local Box2 = Canvas.WorldToScreen(Vec)
  
  local H
  local W
  H = (Box.Y - Box2.Y) / 2
  W = H
  
 
  --Create a good name.
  local Name
  if LocalPRI.Team and (Pickup.idTeamOwner ~= 255) and
    (Pickup.idTeamOwner ~= LocalPRI.Team.TeamIndex) then
    Name = Engine.FNames[Pickup.EnemyPickupClass.Name];
  else
    Name = Engine.FNames[Pickup.Name];
  end
  
  Name = string.sub(Name, 9)
  Name = string.sub(Name, 1, string.find(Name, '_') - 1)
  

  --Draw the ESP box  
  if PC.LineOfSightTo(Pickup) then
    DrawRect(Canvas, Screen.X - W, Screen.Y - H, W*2, H*2)
  else  
    DrawCorners(Canvas, Screen.X - W, Screen.Y - H, W*2, H*2)
  end
  
  Canvas.Font = Canvas.TinyFont
  Canvas.SetPos(Screen.X + W + 5, Screen.Y - H)
  Canvas.DrawTextClipped(Name)
end


-------------------------------------------------------------------------------
function DrawPlayerESP(Canvas, PRI)
  --Skip players without names
  if (PRI.PlayerName == nil) or PRI.bDead or PRI.bIsSpectator or 
    PRI.bOnlySpectator or PRI.bWaitingPlayer  then
    return
  end


  --Calc the player's screen position
  local bIsProne = false
  local bIsCrouched = false
  local BleedHealth = 0
  local Health = ' '
  local Vec = Engine.New(FVector)
  local Pawn = PRI.myPawn
  if Pawn then
    Vec.X = Pawn.Location.X
    Vec.Y = Pawn.Location.Y
    Vec.Z = Pawn.Location.Z
    bIsProne    = Pawn.bIsProne
    bIsCrouched = Pawn.bIsCrouched
    BleedHealth = Pawn.BleedHealth
    if BleedHealth > 0 then
      Health = string.format('[%d-%d]', Pawn.Health, BleedHealth)
    else
      Health = string.format('[%d]', Pawn.Health)
    end
  else
    Vec.X = PRI.LocationX
    Vec.Y = PRI.LocationY
    Vec.Z = PRI.LocationZ
  end
  local Screen = Canvas.WorldToScreen(Vec)
  if Screen.Z >= 1 then
    return
  end
  
  
  --Calc the player's color
  local Enemy = true
  if PRI.Team and LocalPRI.Team and (#PRI.Team == #LocalPRI.Team) then
    Enemy = false
    if BleedHealth > 0 then
      --Yellow
      Canvas.SetDrawColor(255, 255, 0, 255)
    else 
      --Green 
      Canvas.SetDrawColor(0, 255, 0, 255)
    end  
  else
    --Red
    Canvas.SetDrawColor(255, 0, 0, 255)
  end


  --Calc the player's distance.
  local Delta = Canvas.Subtract_VectorVector(Vec, ViewVec)
  local Dist  = Canvas.VSize(Delta)

  
  --Calc the player's screen size.
  Vec.X = ViewVec.X + (Dist * ViewAxesX.X)
  Vec.Y = ViewVec.Y + (Dist * ViewAxesX.Y)
  Vec.Z = ViewVec.Z + (Dist * ViewAxesX.Z)
  local Box = Canvas.WorldToScreen(Vec)

  Vec.X = Vec.X + (50 * ViewAxesZ.X)
  Vec.Y = Vec.Y + (50 * ViewAxesZ.Y)
  Vec.Z = Vec.Z + (50 * ViewAxesZ.Z)
  local Box2 = Canvas.WorldToScreen(Vec)
  
  local H
  local W
  if bIsProne then
    W = Box.Y - Box2.Y
    H = W / 2
  elseif bIsCrouched then
    H = (Box.Y - Box2.Y) / 2
    W = H
  else
    H = Box.Y - Box2.Y
    W = H / 2
  end
  

  --Draw the ESP box  
  if Pawn and PC.LineOfSightTo(Pawn) then
    DrawRect(Canvas, Screen.X - W, Screen.Y - H, W*2, H*2)
  else  
    DrawCorners(Canvas, Screen.X - W, Screen.Y - H, W*2, H*2)
  end

  
  Canvas.Font = Canvas.TinyFont
  Canvas.SetPos(Screen.X + W + 5, Screen.Y - H)
  Canvas.DrawTextClipped(string.format(
    '%s %s',
    PRI.PlayerName,
    Health))
  

  --Draw armed and slung weapon names
  if Pawn then
    local Attachments
    local BWA
    local Index
    local Name
    for Index=1, #BWAData do
      BWA = BWAData[Index]
      if BWA.Owner then
        --print(Index, #BWAData, #BWA.Owner, #Pawn, Engine.FNames[BWA.Name])
        if BWA.bCurrentlySelectedAttachment and (#BWA.Owner == #Pawn) then
          if Enemy and (BWA.bDontSwapMeshes == false) then
            Name = Engine.FNames[BWA.EnemyAttachmentClass.Name]
          else
            Name = Engine.FNames[BWA.Name]
          end
          Name = string.sub(Name, 9)
          Name = string.sub(Name, 1, string.find(Name, '_') - 1)
          Attachments = Name
        end
      end
    end
    
    for Index=1, #BWAData do
      BWA = BWAData[Index]
      if BWA.Owner then
        if (BWA.bCurrentlySelectedAttachment == false) and (#BWA.Owner == #Pawn) then
          if Enemy and (BWA.bDontSwapMeshes == false) then
            Name = Engine.FNames[BWA.EnemyAttachmentClass.Name]
          else
            Name = Engine.FNames[BWA.Name]
          end
          Name = string.sub(Name, 9)
          Name = string.sub(Name, 1, string.find(Name, '_') - 1)
          if Attachments then
            Attachments = Attachments .. ' (' .. Name .. ')'
          else
            Attachments = '(' .. Name .. ')'
          end  
        end
      end
    end
    
    if Attachments then
      Canvas.SetPos(Screen.X + W + 5, Screen.Y - H + 10)
      Canvas.DrawTextClipped(Attachments);
    end
  end
end


-------------------------------------------------------------------------------
function DrawCorners(Canvas, X, Y, W, H)
  local DH = H / 4
  local DW = W / 4
  if DW < 1 then
    DW = 1
  end
  

  --Draw the horizontals.
  Canvas.SetPos(X, Y)
  Canvas.DrawTile(TWhite, DW, 1, 0, 0, 2, 2, Canvas.DrawColor)

  Canvas.SetPos(X + W - DW + 1, Y)
  Canvas.DrawTile(TWhite, DW, 1, 0, 0, 2, 2, Canvas.DrawColor)

  Canvas.SetPos(X, Y + H)
  Canvas.DrawTile(TWhite, DW, 1, 0, 0, 2, 2, Canvas.DrawColor)

  Canvas.SetPos(X + W - DW + 1, Y + H)
  Canvas.DrawTile(TWhite, DW, 1, 0, 0, 2, 2, Canvas.DrawColor)


  --Draw the verticals.
  if DH > 0 then
    Canvas.SetPos(X, Y + 1)
    Canvas.DrawTile(TWhite, 1, DH, 0, 0, 2, 2, Canvas.DrawColor)
  
    Canvas.SetPos(X + W, Y + 1)
    Canvas.DrawTile(TWhite, 1, DH, 0, 0, 2, 2, Canvas.DrawColor)

    Canvas.SetPos(X, Y + H - DH)
    Canvas.DrawTile(TWhite, 1, DH, 0, 0, 2, 2, Canvas.DrawColor)

    Canvas.SetPos(X + W, Y + H - DH)
    Canvas.DrawTile(TWhite, 1, DH, 0, 0, 2, 2, Canvas.DrawColor)
  end
end


-------------------------------------------------------------------------------
function DrawRect(Canvas, X, Y, W, H)
  Canvas.SetPos(X, Y)
  Canvas.DrawTile(TWhite, 1, H, 0, 0, 2, 2, Canvas.DrawColor)
  Canvas.DrawTile(TWhite, W, 1, 0, 0, 2, 2, Canvas.DrawColor)
  
  Canvas.SetPos(X, Y+H)
  Canvas.DrawTile(TWhite, W+1, 1, 0, 0, 2, 2, Canvas.DrawColor)
  
  Canvas.SetPos(X+W, Y+1)
  Canvas.DrawTile(TWhite, 1, H-1, 0, 0, 2, 2, Canvas.DrawColor)
end