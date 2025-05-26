# SLE键盘与适配器系统设计文档

## 1. 系统架构图

```mermaid
graph TB
    subgraph "SLE Keyboard (Server端)"
        KS[键盘扫描模块]
        LED[LED驱动模块]
        SLE_S[SLE Server服务]
        ADV[广播模块]
        CONN_S[连接管理]
    end
    
    subgraph "SLE Dongle (Client端)"
        SLE_C[SLE Client服务]
        HID[HID键盘模拟]
        CONN_C[连接管理]
        USB[USB接口]
    end
    
    subgraph "硬件层"
        MCU[微控制器]
        SPI[SPI总线]
        GPIO[GPIO引脚]
        RF[射频模块]
    end
    
    subgraph "PC/主机"
        OS[操作系统]
        APP[应用程序]
    end
    
    KS --> SLE_S
    LED --> SPI
    SLE_S --> ADV
    SLE_S --> CONN_S
    CONN_S <-.-> RF
    RF <-.->|SLE协议| RF
    CONN_C <-.-> RF
    SLE_C --> CONN_C
    SLE_C --> HID
    HID --> USB
    USB --> OS
    OS --> APP
    
    MCU --> GPIO
    MCU --> SPI
    MCU --> RF
```

## 2. SLE键盘端完整时序图

```mermaid
sequenceDiagram
    participant App as 应用主任务
    participant KS as 键盘扫描
    participant SLE as SLE服务器
    participant ADV as 广播模块
    participant LED as LED控制
    participant Client as SLE客户端
    
    Note over App: 系统初始化阶段
    App->>KS: uapi_keyscan_init()
    App->>SLE: sle_keyboard_server_init()
    App->>ADV: sle_keyboard_server_adv_init()
    App->>LED: 启动LED任务
    
    Note over App: 广播阶段
    SLE->>ADV: 注册广播回调
    ADV->>ADV: sle_start_announce()
    ADV-->>Client: 发送广播数据
    
    Note over App: 连接建立阶段
    Client->>SLE: 连接请求
    SLE->>SLE: sle_connect_state_changed_cbk()
    SLE->>SLE: sle_pair_complete_cbk()
    SLE-->>App: 发送连接状态消息
    
    Note over App: 数据传输阶段
    loop 键盘输入循环
        KS->>KS: 检测按键
        KS->>App: sle_keyboard_keyscan_callback()
        App->>App: 转换HID报告格式
        App->>SLE: sle_keyboard_server_send_report_by_handle()
        SLE-->>Client: 发送键盘数据
        
        alt 接收客户端数据
            Client->>SLE: 发送LED控制数据
            SLE->>App: ssaps_server_write_request_cbk()
            App->>LED: 更新LED状态
        end
    end
```

## 3. SLE适配器端时序图

```mermaid
sequenceDiagram
    participant App as 适配器主任务
    participant SLE as SLE客户端
    participant HID as HID键盘
    participant USB as USB接口
    participant PC as 电脑
    
    Note over App: 初始化阶段
    App->>SLE: sle_client_init()
    App->>HID: hid_keyboard_init()
    
    Note over App: 扫描连接阶段
    SLE->>SLE: 开始扫描
    SLE->>SLE: 发现键盘设备
    SLE->>SLE: 建立连接
    
    Note over App: 数据转发阶段
    loop 数据处理循环
        SLE->>App: 接收键盘数据
        App->>App: 解析HID报告
        App->>HID: 更新键盘状态
        HID->>USB: 发送HID报告
        USB-->>PC: 传输到电脑
        
        alt PC发送LED控制
            PC->>USB: LED状态数据
            USB->>HID: 解析LED数据
            HID->>App: LED控制信息
            App->>SLE: 发送到键盘
        end
    end
```

## 4. LED控制模块流程图

```mermaid
flowchart TD
    A[启动LED任务] --> B[初始化随机数生成器]
    B --> C[创建LED数据结构数组]
    C --> D[生成随机颜色数据]
    D --> E[配置SPI引脚]
    E --> F[初始化SPI主机]
    F --> G[编码LED数据]
    G --> H[通过SPI发送数据]
    H --> I{传输成功?}
    I -->|否| J[重试发送]
    J --> H
    I -->|是| K[发送复位信号]
    K --> L[释放内存]
    L --> M[任务结束]
    
    subgraph "数据编码过程"
        G1[RGB数据] --> G2[4位编码转换]
        G2 --> G3[组合成字节数据]
        G3 --> G4[生成SPI传输数据]
    end
    
    G --> G1
```

## 5. 键盘扫描处理流程图

```mermaid
flowchart TD
    A[键盘扫描中断] --> B[读取按键矩阵]
    B --> C[去抖动处理]
    C --> D[映射到键值]
    D --> E{有按键变化?}
    E -->|否| F[返回]
    E -->|是| G[构建HID报告]
    G --> H[区分特殊键和普通键]
    H --> I[设置特殊键位图]
    I --> J[填充普通键数组]
    J --> K[发送到SLE服务器]
    K --> L[通过SLE传输到适配器]
    L --> F
    
    subgraph "HID报告格式"
        H1[报告类型]
        H2[特殊键位图]
        H3[保留字节]
        H4[普通键数组6字节]
    end
    
    G --> H1
```

## 6. SLE连接管理流程图

```mermaid
flowchart TD
    A[SLE设备初始化] --> B[注册连接回调]
    B --> C[开始广播]
    C --> D{客户端扫描到?}
    D -->|否| E[继续广播]
    E --> D
    D -->|是| F[接收连接请求]
    F --> G[建立连接]
    G --> H[配对过程]
    H --> I{配对成功?}
    I -->|否| J[断开连接]
    J --> C
    I -->|是| K[连接建立完成]
    K --> L[开始数据传输]
    L --> M{连接断开?}
    M -->|是| N[清理连接状态]
    N --> C
    M -->|否| O[处理数据收发]
    O --> L
    
    subgraph "广播数据内容"
        B1[设备名称]
        B2[服务UUID]
        B3[设备能力]
        B4[发射功率]
    end
    
    C --> B1
```

## 7. 整体数据流图

```mermaid
graph LR
    subgraph "物理输入"
        KEY[按键]
        MAT[键盘矩阵]
    end
    
    subgraph "键盘端处理"
        SCAN[扫描模块]
        MAP[键值映射]
        HID1[HID报告生成]
        SLE_TX[SLE发送]
    end
    
    subgraph "无线传输"
        RF1[RF发送]
        RF2[RF接收]
    end
    
    subgraph "适配器端处理"
        SLE_RX[SLE接收]
        HID2[HID处理]
        USB_TX[USB发送]
    end
    
    subgraph "主机端"
        PC[电脑]
        APP[应用程序]
    end
    
    subgraph "反向控制"
        LED_CMD[LED控制命令]
        LED_DRIVER[LED驱动]
        LEDS[LED灯]
    end
    
    KEY --> MAT
    MAT --> SCAN
    SCAN --> MAP
    MAP --> HID1
    HID1 --> SLE_TX
    SLE_TX --> RF1
    RF1 -.->|SLE协议| RF2
    RF2 --> SLE_RX
    SLE_RX --> HID2
    HID2 --> USB_TX
    USB_TX --> PC
    PC --> APP
    
    PC --> LED_CMD
    LED_CMD --> RF1
    RF1 -.-> RF2
    RF2 --> LED_DRIVER
    LED_DRIVER --> LEDS
```

## 系统特点总结

1. **双向通信**: 键盘到电脑的按键数据传输，电脑到键盘的LED控制
2. **模块化设计**: 键盘扫描、SLE通信、LED控制、HID处理等独立模块
3. **实时性**: 中断驱动的键盘扫描，快速响应按键事件
4. **可扩展性**: 支持多种LED效果，可配置的键盘布局
5. **低功耗**: SLE协议优化的无线传输
6. **标准兼容**: 符合HID键盘标准，即插即用
