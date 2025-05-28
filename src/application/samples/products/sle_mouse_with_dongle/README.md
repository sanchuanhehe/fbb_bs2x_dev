
### 1. 时序图（Sequence Diagram）

````mermaid
sequenceDiagram
    participant App as app_run
    participant Main as sle_mouse_with_dongle
    participant Task as sle_mouse_dongle_task
    participant USB as usb_init_app
    participant Client as sle_mouse_client_init
    participant LL as sle_low_latency_dongle_init
    participant Set as sle_low_latency_set
    participant Enable as sle_low_latency_dongle_enable

    App->>Main: sle_mouse_with_dongle()
    alt CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE
        Main->>Task: osThreadNew(sle_mouse_dongle_task)
        Task->>Client: sle_mouse_client_init()
        Task->>USB: usb_init_app(DEV_HID)
        alt usb_hid_index >= 0
            loop 连接未建立
                Task->>Task: osal_msleep(USB_MOUSE_TASK_DELAY_MS)
            end
            Task->>LL: sle_low_latency_dongle_init(usb_hid_index)
            Task->>Set: sle_low_latency_set(conn_id, TRUE, report_rate_idx)
            Task->>Enable: sle_low_latency_dongle_enable()
            loop 连接已建立
                Task->>Task: osal_msleep(USB_MOUSE_TASK_DELAY_MS)
            end
        end
    else CONFIG_SAMPLE_SUPPORT_SLE_MOUSE
        Main->>mouse_init: mouse_init(PWM3395DM)
        Main->>LL: sle_low_latency_mouse_app_init()
        Main->>Enable: sle_low_latency_mouse_enable()
        Main->>sle_mouse_server_init: sle_mouse_server_init()
    end
````

### 2. 时序图（Sequence Diagram）

````mermaid
sequenceDiagram
    participant Sensor as PAW3395
    participant SPI as mouse_sensor_spi_opration
    participant Operator as g_sle_paw3395_operator
    participant Mouse as mouse_init
    participant LLApp as sle_low_latency_mouse_app_init
    participant LL as sle_low_latency_mouse_enable
    participant Server as sle_mouse_server_init
    participant Dongle as sle_low_latency_dongle_init
    participant USB as usb_init_app

    Note over Sensor,Operator: 采集阶段
    Mouse->>Operator: .init()
    Operator->>SPI: mouse_sensor_spi_opration(g_sle_paw3395db_cfg)
    Operator->>Operator: paw3395_chaneg_dpi()
    Operator->>SPI: mouse_sensor_spi_opration(dpi_reg)
    Note over Operator: 采集数据
    Operator->>Operator: paw3395_get_xy(x, y)
    Operator->>SPI: mouse_spi_burst_read(BURST_MOTION_READ, ...)
    SPI->>Sensor: 读取运动数据
    Operator->>LLApp: sle_low_latency_mouse_app_init()
    LLApp->>LL: sle_low_latency_mouse_register_callbacks
    LL->>Server: sle_mouse_server_init()
    Note over LL, Dongle: 低延迟数据通道建立
    Dongle->>USB: usb_init_app(DEV_HID)
    Dongle->>Dongle: sle_low_latency_dongle_init(usb_hid_index)
    Dongle->>Dongle: sle_low_latency_set(conn_id, TRUE, report_rate_idx)
    Dongle->>Dongle: sle_low_latency_dongle_enable()
    Note over Dongle, USB: 数据通过USB上传
````

---

### 3. 调用关系图（Call Graph）

````mermaid
graph TD
    mouse_init --> sle_mouse_get_paw3395_operator
    sle_mouse_get_paw3395_operator --> g_sle_paw3395_operator
    g_sle_paw3395_operator --> paw_3395_mouse_init
    paw_3395_mouse_init --> mouse_sensor_spi_open
    paw_3395_mouse_init --> mouse_sensor_spi_opration
    paw_3395_mouse_init --> paw3395_chaneg_dpi
    paw3395_chaneg_dpi --> mouse_sensor_spi_opration
    g_sle_paw3395_operator --> paw3395_get_xy
    paw3395_get_xy --> mouse_spi_burst_read
    mouse_spi_burst_read --> Sensor
    sle_low_latency_mouse_app_init --> sle_low_latency_mouse_register_callbacks
    sle_low_latency_mouse_enable
    sle_mouse_server_init
    sle_low_latency_dongle_init --> usb_init_app
    sle_low_latency_dongle_init --> sle_low_latency_set
    sle_low_latency_dongle_init --> sle_low_latency_dongle_enable
````
