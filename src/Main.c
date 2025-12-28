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

#define MEMORY_INST_COUNT   35
#define MEMORY_STACK_COUNT  35


VM16 vm;

void Setup(AlxWindow* w){
    SetAlxFont(ALXFONT_HIGH);
    ResizeAlxFont(16,32);

	vm = VM16_New();
    VM16_ConnectDevice(&vm,(VM16_Ram[]){    VM16_Ram_New(VM16_MEMORY_LOW,   VM16_MEMORY_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_Kernel[]){ VM16_Kernel_New(VM16_KERNEL_LOW,VM16_KERNEL_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_TTY[]){    VM16_TTY_New(VM16_TTY_LOW,      VM16_TTY_HIGH)      });
    VM16_ConnectDevice(&vm,(VM16_DSP[]){    VM16_DSP_New(VM16_DSP_LOW,      VM16_DSP_HIGH)      });
    VM16_ConnectDevice(&vm,(VM16_GPU[]){    VM16_GPU_New(VM16_GPU_LOW,      VM16_GPU_HIGH)      });
    
    //VM16_Comp(&vm,"./code/Main.svm16","./asm/Main.lvm16");
    //VM16_Comp(&vm,"./code/Lib.svm16","./asm/Lib.lvm16");
    //VM16_Link(&vm,VM16_MEMORY_LOW,"_start",(CStr[]){ "./asm/Main.lvm16","./asm/Lib.lvm16",NULL },"./asm/Main.vm16");
    VM16_Load(&vm,VM16_MEMORY_LOW,"./asm/Main.vm16");
    VM16_Setup(&vm,VM16_MEMORY_LOW);
}

void Update(AlxWindow* w){
    if(Stroke(ALX_KEY_SPACE).PRESSED){
        if(vm.flags.running){
            VM16_Step(&vm);
        }
    }
    if(Stroke(ALX_KEY_ENTER).PRESSED){
        VM16_Reset(&vm,VM16_MEMORY_LOW,"./asm/Main.vm16");
    }

	Clear(BLACK);

    int inst_offset = 0;
    for(int i = 0;i<MEMORY_INST_COUNT;i++){
        const VM16_Addr addr = vm.pc + inst_offset;
        
        VM16_Device* dev = VM16_GetRam(&vm);
        if(dev && addr >= dev->low && addr < dev->high){
            VM16_Imm count;
            CStr dis_inst = VM16_DisassembleInst(&vm,(char*)dev->data + (addr - dev->low),&count);
            CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),10.0f,i * GetAlxFont()->CharSizeY,inst_offset == 0 ? BLUE : WHITE,"%d: %s",(Number)addr,dis_inst);
            inst_offset += count;
        }else{
            break;
        }
    }

    int heap_offset = 0;
    for(int i = 0;i<MEMORY_STACK_COUNT;i++){
        const VM16_Addr addr = VM16_MEMORY_LOW + vm.segsize + heap_offset;
        
        VM16_Device* dev = VM16_GetRam(&vm);
        if(dev && addr >= dev->low && addr < dev->high){
            char* ptr = (char*)dev->data + (addr - dev->low);
            VM16_Byte byte = *(VM16_Byte*)ptr;
            VM16_Imm word = *(VM16_Imm*)ptr;

            CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),30 * GetAlxFont()->CharSizeX,i * GetAlxFont()->CharSizeY,heap_offset == 0 ? BLUE : WHITE,"%d (%x): %d ('%c' W: %d)",(Number)addr,(Number)addr,(Number)byte,(Number)byte,(Number)word);
            heap_offset++;
        }else{
            break;
        }
    }

    int stack_offset = -10;
    for(int i = 0;i<MEMORY_STACK_COUNT;i++){
        const VM16_Addr addr = vm.sp + stack_offset;
        
        VM16_Device* dev = VM16_GetRam(&vm);
        if(dev && addr >= dev->low && addr < dev->high){
            char* ptr = (char*)dev->data + (addr - dev->low);
            VM16_Byte byte = *(VM16_Byte*)ptr;
            VM16_Imm word = *(VM16_Imm*)ptr;

            CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),65 * GetAlxFont()->CharSizeX,i * GetAlxFont()->CharSizeY,stack_offset == 0 ? BLUE : WHITE,"%d (%x): %d ('%c' W: %d)",(Number)addr,(Number)addr,(Number)byte,(Number)byte,(Number)word);
            stack_offset++;
        }else{
            break;
        }
    }

    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,0.0f,WHITE,"R0: %d(%x)",vm.registers[0],vm.registers[0]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY,WHITE,"R1: %d(%x)",vm.registers[1],vm.registers[1]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY * 2,WHITE,"R2: %d(%x)",vm.registers[2],vm.registers[2]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY * 3,WHITE,"R3: %d(%x)",vm.registers[3],vm.registers[3]);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY * 4,WHITE,"FG: %d(%x)",vm.flags.value,vm.flags.value);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY * 5,WHITE,"SP: %d(%x)",vm.sp,vm.sp);
    CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),GetWidth() - 24 * GetAlxFont()->CharSizeX,GetAlxFont()->CharSizeY * 6,WHITE,"PC: %d(%x)",vm.pc,vm.pc);
    
    if(vm.flags.running){
        
    }else{
        String str = String_Format("Exitcode: %d",vm.exitcode);
	    RenderCStrSize(str.Memory,str.size,GetWidth() - str.size * GetAlxFont()->CharSizeX,GetHeight() - GetAlxFont()->CharSizeY,RED);
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