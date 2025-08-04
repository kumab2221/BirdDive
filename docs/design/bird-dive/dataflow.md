# Bird Dive Challenge データフロー図

## システム全体フロー

```mermaid
flowchart TB
    A[Player Input] --> B[PlayerController]
    B --> C[BirdCharacter]
    C --> D[CharacterMovement]
    D --> E[WindForceComponent]
    E --> F[Physics System]
    F --> G[Animation System]
    G --> H[ControlRig]
    H --> I[Skeletal Mesh]
    
    C --> J[Camera System]
    J --> K[SplitScreen Manager]
    K --> L[First Person Camera]
    K --> M[Side View Camera]
    L --> N[Post Process Effects]
    M --> O[Side View Rendering]
    
    F --> P[Collision Detection]
    P --> Q[Landing System]
    Q --> R[Effect Manager]
    R --> S[Niagara Effects]
    Q --> T[Score System]
    T --> U[UI Manager]
    U --> V[Game HUD]
    
    W[Game Mode] --> X[Game State]
    X --> Y[Difficulty Settings]
    Y --> E
    Y --> T
    
    style A fill:#e1f5fe
    style V fill:#f3e5f5
    style S fill:#fff3e0
```

## ゲームプレイフロー

```mermaid
sequenceDiagram
    participant P as Player
    participant PC as PlayerController
    participant BC as BirdCharacter
    participant GM as GameMode
    participant GS as GameState
    participant UI as UIManager
    
    P->>PC: Start Game Input
    PC->>GM: Initialize Game
    GM->>GS: Set Initial State
    GM->>BC: Spawn Bird Character
    GS->>UI: Update Timer Display
    
    loop Game Loop
        P->>PC: Movement Input
        PC->>BC: Apply Input Force
        BC->>BC: Calculate Physics
        BC->>BC: Apply Wind Force
        BC->>GS: Update Position
        GS->>UI: Update Score/Timer
        
        alt Bird Lands
            BC->>GM: Landing Event
            GM->>GS: Calculate Final Score
            GS->>UI: Show Result Screen
            break Game End
        end
        
        alt Speed Exceeded
            BC->>GM: Speed Violation Event
            GM->>GS: Set Disqualified State
            GS->>UI: Show Disqualification
            break Game End
        end
    end
```

## 物理システムデータフロー

```mermaid
flowchart LR
    A[Physics Tick] --> B[Wind Force Calculation]
    B --> C[Perlin Noise Generation]
    C --> D[Difficulty Modifier]
    D --> E[Force Vector]
    E --> F[Character Movement]
    F --> G[Velocity Update]
    G --> H[Position Update]
    H --> I[Collision Check]
    
    I --> J{Collision Detected?}
    J -->|Yes| K[Landing Validation]
    J -->|No| L[Continue Physics]
    L --> A
    
    K --> M{Speed Valid?}
    M -->|Yes| N[Success Landing]
    M -->|No| O[Disqualification]
    
    N --> P[Effect Trigger]
    O --> Q[Game Over]
    
    style A fill:#e8f5e8
    style J fill:#fff2cc
    style M fill:#fff2cc
    style P fill:#ffe6cc
    style Q fill:#ffebee
```

## アニメーションデータフロー

```mermaid
flowchart TD
    A[Physics State] --> B[Animation Blueprint]
    B --> C[State Machine]
    C --> D{Current State}
    
    D -->|Falling| E[Falling Animation]
    D -->|Gliding| F[Gliding Animation]
    D -->|Turning| G[Turning Animation]
    D -->|Landing| H[Landing Animation]
    
    E --> I[ControlRig Processing]
    F --> I
    G --> I
    H --> I
    
    I --> J[Wing Flap Calculation]
    I --> K[Body Rotation Adjustment]
    I --> L[Tail Adjustment]
    
    J --> M[Skeletal Mesh Update]
    K --> M
    L --> M
    
    N[Wind Force Data] --> I
    O[Velocity Data] --> B
    P[Input Data] --> C
    
    style A fill:#e3f2fd
    style D fill:#fff3e0
    style I fill:#f1f8e9
    style M fill:#fce4ec
```

## カメラシステムデータフロー

```mermaid
flowchart TB
    A[Bird Character Transform] --> B[Camera Manager]
    B --> C[Split Screen Controller]
    
    C --> D[First Person Camera]
    C --> E[Side View Camera]
    
    D --> F[Head Bone Transform]
    D --> G[Velocity-based FOV]
    D --> H[Post Process Parameters]
    
    E --> I[Side Follow Position]
    E --> J[Fixed Angle Calculation]
    
    F --> K[FP Viewport Rendering]
    G --> K
    H --> K
    
    I --> L[Side Viewport Rendering]
    J --> L
    
    K --> M[Screen Split Compositor]
    L --> M
    M --> N[Final Screen Output]
    
    O[Speed Data] --> G
    O --> H
    P[Player Preferences] --> C
    
    style A fill:#e8eaf6
    style C fill:#f3e5f5
    style K fill:#e0f2f1
    style L fill:#e0f2f1
    style N fill:#fff8e1
```

## UIデータフロー

```mermaid
flowchart LR
    A[Game State Data] --> B[UI Manager]
    B --> C[HUD Widget]
    B --> D[Menu Widget]
    B --> E[Result Widget]
    
    C --> F[Score Display]
    C --> G[Timer Display]
    C --> H[Speed Indicator]
    C --> I[Warning Messages]
    
    D --> J[Difficulty Selection]
    D --> K[Settings Panel]
    D --> L[Audio Controls]
    
    E --> M[Final Score]
    E --> N[Performance Stats]
    E --> O[Replay Button]
    
    P[Player Input] --> J
    P --> K
    P --> O
    
    Q[Score System] --> F
    R[Timer System] --> G
    S[Physics System] --> H
    T[Game Rules] --> I
    
    style A fill:#f9fbe7
    style B fill:#e8f5e8
    style C fill:#e3f2fd
    style D fill:#fff3e0
    style E fill:#fce4ec
```

## エフェクトシステムデータフロー

```mermaid
flowchart TD
    A[Landing Event] --> B[Effect Manager]
    B --> C[Effect Type Determination]
    
    C --> D{Landing Quality}
    D -->|Perfect| E[Perfect Landing Effect]
    D -->|Good| F[Good Landing Effect]
    D -->|Poor| G[Poor Landing Effect]
    D -->|Failed| H[Crash Effect]
    
    E --> I[Niagara System 1]
    F --> J[Niagara System 2]
    G --> K[Niagara System 3]
    H --> L[Niagara System 4]
    
    I --> M[Particle Generation]
    J --> M
    K --> M
    L --> M
    
    M --> N[Rendering System]
    
    O[Landing Position] --> C
    P[Landing Speed] --> C
    Q[Wind Conditions] --> C
    
    R[Audio Manager] --> S[Landing Sound]
    B --> R
    
    style A fill:#ffe0b2
    style D fill:#fff2cc
    style M fill:#e1f5fe
    style N fill:#f3e5f5
    style S fill:#fff3e0
```

## データ永続化フロー

```mermaid
flowchart LR
    A[Game Session] --> B[Score Calculation]
    B --> C[Performance Metrics]
    C --> D[Save Game System]
    
    D --> E[High Score Data]
    D --> F[Player Settings]
    D --> G[Statistics Data]
    
    E --> H[Local Storage]
    F --> H
    G --> H
    
    H --> I[Save File Management]
    I --> J[Data Validation]
    J --> K[Persistent Storage]
    
    L[Load Game Request] --> I
    I --> M[Data Loading]
    M --> N[Game State Restoration]
    
    style A fill:#e8f5e8
    style D fill:#fff3e0
    style H fill:#e3f2fd
    style K fill:#f1f8e9
```

## エラーハンドリングフロー

```mermaid
flowchart TD
    A[System Component] --> B{Error Detected?}
    B -->|No| C[Normal Operation]
    B -->|Yes| D[Error Classification]
    
    D --> E{Error Type}
    E -->|Physics Error| F[Physics Fallback]
    E -->|Render Error| G[Render Fallback]
    E -->|Asset Error| H[Asset Fallback]
    E -->|Critical Error| I[Graceful Shutdown]
    
    F --> J[Reset Physics State]
    G --> K[Use Default Materials]
    H --> L[Load Default Assets]
    
    J --> M[Log Error]
    K --> M
    L --> M
    I --> N[Save Current State]
    
    M --> O[Continue Operation]
    N --> P[Show Error Dialog]
    
    style A fill:#e8f5e8
    style B fill:#fff2cc
    style E fill:#fff2cc
    style I fill:#ffebee
    style P fill:#ffebee
```