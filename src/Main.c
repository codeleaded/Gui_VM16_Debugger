#if defined(__linux__)
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Assembler.h"
#include "/home/codeleaded/System/Static/Library/VM16_Linker.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#elif defined(_WINE)
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Assembler.h"
#include "/home/codeleaded/System/Static/Library/VM16_Linker.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#elif defined(_WIN32)
#include "F:/home/codeleaded/System/Static/Library/VM16.h"
#include "F:/home/codeleaded/System/Static/Library/VM16_Assembler.h"
#include "F:/home/codeleaded/System/Static/Library/VM16_Linker.h"
#include "F:/home/codeleaded/System/Static/Library/VM16_Devices.h"
#include "F:/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#elif defined(__APPLE__)
#error "Apple not supported!"
#else
#error "Platform not supported!"
#endif


VM16 vm;

void Setup(AlxWindow* w){
	vm = VM16_New();
    VM16_ConnectDevice(&vm,(VM16_Ram[]){    VM16_Ram_New(VM16_MEMORY_LOW,   VM16_MEMORY_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_Kernel[]){ VM16_Kernel_New(VM16_KERNEL_LOW,VM16_KERNEL_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_TTY[]){    VM16_TTY_New(VM16_TTY_LOW,      VM16_TTY_HIGH)      });
    //VM16_ConnectDevice(&vm,(VM16_MPU[]){    VM16_MPU_New(VM16_MPU_LOW,      VM16_MPU_HIGH)      });
    //VM16_ConnectDevice(&vm,(VM16_DSP[]){    VM16_DSP_New(VM16_DSP_LOW,      VM16_DSP_HIGH)      });
    //VM16_ConnectDevice(&vm,(VM16_GPU[]){    VM16_GPU_New(VM16_GPU_LOW,      VM16_GPU_HIGH)      });
    VM16_ConnectDevice(&vm,(VM16_SSD[]){    VM16_SSD_New(VM16_SSD_LOW,      VM16_SSD_HIGH)      });
    
    VM16_Comp(&vm,"./code/Main.svm16","./asm/Main.lvm16");
    VM16_Comp(&vm,"./code/Lib.svm16","./asm/Lib.lvm16");
    VM16_Link(&vm,VM16_MEMORY_LOW,"_start",(CStr[]){ "./asm/Main.lvm16","./asm/Lib.lvm16",NULL },"./asm/Main.vm16");
    VM16_Load(&vm,VM16_MEMORY_LOW,"./asm/Main.vm16");
    VM16_Setup(&vm,VM16_MEMORY_LOW);
}

void Update(AlxWindow* w){
    if(vm.running){
        VM16_Step(&vm);
    }

	Clear(BLACK);

    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0.0f,0.0f,WHITE,"R0: %u(%x)",vm.registers[0],vm.registers[0]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0.0f,GetAlxFont()->CharSizeY,WHITE,"R1: %u(%x)",vm.registers[1],vm.registers[1]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0.0f,GetAlxFont()->CharSizeY * 2,WHITE,"R2: %u(%x)",vm.registers[2],vm.registers[2]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0.0f,GetAlxFont()->CharSizeY * 3,WHITE,"R3: %u(%x)",vm.registers[3],vm.registers[3]);

    if(vm.running){
        
    }else{
        String str = String_Format("Exitcode: %d",vm.exitcode);
	    RenderCStrSize(str.Memory,str.size,GetWidth() - str.size * GetAlxFont()->CharSizeX,0,RED);
	    String_Free(&str);
    }
}

void Delete(AlxWindow* w){
    VM16_Free(&vm);
}

int main(){
    if(Create("Voxel3D",2500,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}