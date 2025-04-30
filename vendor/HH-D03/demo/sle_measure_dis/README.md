# 测距

- 步骤一：参考  <a href="debug/SDK Sample User Guide/BS2XV100 SDK Sample使用指南.md">BS2XV100 SDK Sample使用指南.md</a>

- 步骤二：修改drivers\chips\bs2x\main_init\app_os_init.c中\#define TASK_COMMON_APP_DELAY_MS       20000修改为\#define TASK_COMMON_APP_DELAY_MS       7000

  ```
  #define TASK_COMMON_APP_DELAY_MS       7000
  ```
