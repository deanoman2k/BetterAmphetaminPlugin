#include "threads.h"
/* This contains the KERNEL PANIC PREVENTION SYSTEMs "blacklist",
 * that means on certain games the game thread will not be frozen
 */
#define WHITE_LISTS 1
const char* white_list[] = {"PCSG00622"};
/*
 * Tricky way to freeze main thread, we set our plugin priority to 0 (max)
 * and we start two threads with 0 priority in order to get VITA scheduler
 * to always reschedule our threads instead of main one
 */
volatile int term_stubs = 0;
int stub_thread(SceSize args, void *argp) {
	for (;;) {if (term_stubs) sceKernelExitDeleteThread(0);}	
}
void pauseMainThread(char* titleid) {
    sceKernelChangeThreadPriority(0, 0x0);
    term_stubs = 0;
    int thread_num = 2;
    int i;
    for (i = 0; i < WHITE_LISTS; i++)
    {
        if (strcmp(white_list[i], titleid) == 0) {
            thread_num = 1; // or 0 ? Just disable pause thread feature(For PCSG00622 1 is OK)
        }
    }
    for (i = 0; i < thread_num; i++) {
        SceUID thid = sceKernelCreateThread("thread", stub_thread, 0x0, 0x10000, 0, 0, NULL);
        if (thid >= 0)
            sceKernelStartThread(thid, 0, NULL);
    }
}
void resumeMainThread() {
	term_stubs = 1;
	sceKernelChangeThreadPriority(0, 0x40);
}
/*
 * END OF PAUSE/RESUME THREADS TRICK
*/
 
