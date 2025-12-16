/*
 * Copyright (c) 2024 Zephyr Console Example
 * Target: nRF52832
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/console/console.h> /* 引入 Console 子系統 */

/* 設定執行緒堆疊大小與優先權 */
#define STACK_SIZE 1024
#define THREAD_PRIORITY 7

/* 定義執行緒堆疊區與結構 */
K_THREAD_STACK_DEFINE(input_stack_area, STACK_SIZE);
struct k_thread input_thread_data;

/* * 輸入處理執行緒
 * 負責透過 console_getline 讀取資料並處理
 */
void input_thread_entry(void *p1, void *p2, void *p3)
{
	printk("[Input Thread] 啟動，等待輸入...\n");

	/* 初始化 console_getline 功能 */
	console_getline_init();

	while (1) {
		/* 提示符號 */
		printk("Zephyr> ");

		/* * console_getline() 是一個阻塞函數 (Blocking Call)
		 * 它會讓此執行緒進入休眠，直到使用者輸入一行文字並按下 Enter
		 * 這樣不會佔用 CPU 資源
		 */
		char *s = console_getline();

		/* 收到輸入後的處理 */
		printk("\n[Input Thread] 您輸入了: %s\n", s);
		
		/* 簡單的指令判斷範例 */
		if (strcmp(s, "hello") == 0) {
			printk("-> World!\n");
		} else if (strcmp(s, "help") == 0) {
			printk("-> 支援指令: hello, help\n");
		}
	}
}

int main(void)
{
	printk("[Main] 系統啟動\n");

	/* * 建立並啟動輸入處理執行緒
	 * 這樣輸入邏輯就與主程式邏輯分開了
	 */
	k_tid_t my_tid = k_thread_create(&input_thread_data, input_stack_area,
					 K_THREAD_STACK_SIZEOF(input_stack_area),
					 input_thread_entry,
					 NULL, NULL, NULL,
					 THREAD_PRIORITY, 0, K_NO_WAIT);

	/* * 主執行緒 (Main Thread) 迴圈
	 * 這裡模擬系統的其他工作（例如讀取感測器、閃爍 LED 等）
	 * 證明 console_getline 不會卡住整個系統
	 */
	int count = 0;
	while (1) {
		/* 每 5 秒印出一條訊息，證明主執行緒仍在運作 */
		// printk("[Main] 系統運作中... (%d)\n", ++count);
		k_sleep(K_SECONDS(5));
	}
	
	return 0;
}
