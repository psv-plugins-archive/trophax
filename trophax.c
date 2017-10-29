//TropHax by SilicaServer (https://discord.gg/j4eGHhF)
//I can say with great certainty this will not brick your vita

/// SilicaAndPina - Lead Project Manager and Recon
/// @Pocxki - Idea/Trophy merchant/Lead Tester
/// @dots_tb - Lead Developer, Slave & Silica Victim
/// @notzecoxao - Former Lead Developer, Free man
/// @Nkekev - Lead Update Developer

///Extreme Testing Team: JustMulti, wosley
///Testing Team: Levi

//further credits: theFlow for amphetamin, frangarcj for oClock
#include <vitasdk.h>
#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include "blit.h"

#define SCE_NP_TROPHY_NAME_MAX_SIZE 128
#define SCE_NP_TROPHY_DESCR_MAX_SIZE 1024

typedef int32_t SceNpTrophyHandle;
typedef int32_t SceNpTrophyContext;
typedef int32_t SceNpTrophyId;

int sceNpTrophyUnlockTrophy(SceNpTrophyContext context, SceNpTrophyHandle handle, SceNpTrophyId trophyId, SceNpTrophyId *platinumId);
int sceNpTrophyCreateHandle(SceNpTrophyHandle *handle);

SceNpTrophyContext *context = NULL;
SceNpTrophyHandle handle = -1;


char display[128];
int hook[10] = {[0 ... 9] = -1};
int isInit = 0, isCxt = 0, isExt = 0, isExtFound = 0, isCredits = 0,  isHidden = 1, menu = 0, trophyIdChoice = 0;
char mod_name_buf[28];
char *mod_name = TAI_MAIN_MODULE;

void drawScreen() {
        blit_set_color(0x00FFFFFF, 0x00FF0000);
        if(!isHidden && (!isCredits)){
                blit_string(20, 40, "SilicaServer presents: Trophax 0.2");
                blit_string(20, 60, display);
                blit_string(20, 80, "Press L+R to unlock all or L+START to unlock one");
        }
        switch(menu){
        case 1:
                /*blit_clear(0x00FF0000);
                  blit_string(20, 80, "Unlocking all the trophies");*/
                blit_string(20, 260, display);
                break;
        case 2:
                if(!isCredits){
                        blit_string(20, 80, "Select trophy by ID you want to unlock with L+UP / L+DOWN");
                        blit_stringf(20, 100, "Trophy ID: %d", trophyIdChoice);
                        blit_string(20, 120, "Press L+CIRCLE to confirm and unlock");
                }
                blit_string(20, 260, display);
                break;
        }
        if(isCxt&&isInit&&isCredits) {
                blit_string(20, 60, "Trophax 0.2 by Silica Server, please wait...");
                blit_string(20, 80,"@SilicaAndPina - Lead Project Manager and Recon");
                blit_string(20, 100,"@Pocxki - Idea/Trophy merchant/Lead Tester");
                blit_string(20, 120,"@dots_tb - Lead Developer, Slave & Silica Victim");
                blit_string(20, 140,"@notzecoxao - Former Lead Developer, Free man");
                blit_string(20, 160,"@Nkekev - Update Lead Developer");
                blit_string(20, 180,"Extreme Testing Team: Multi-MoDz, wosley");
                blit_string(20, 200,"Testing Team: Levi");
        }
}


static tai_hook_ref_t trophy_cxt_hook;

SceUID sceNpTrophyCreateContext_patched(SceNpTrophyContext *c, uint32_t commId,  uint32_t commSign, uint64_t options) {
	sceClibPrintf("\nContext called");
	int ret = TAI_CONTINUE(SceUID, trophy_cxt_hook, c, commId, commSign, options);
	context = c;
	sceClibPrintf("\nContext found: %x %x", *context, *c);
	if(ret<0) {
		sceClibSnprintf(display,128, "Error: SceNpTrophy cxt has ret: %x", ret);
	} else {
		isCxt = 1;
	}	

	return ret;
}
static tai_hook_ref_t trophy_init_hook;

SceUID sceNpTrophyInit_patched(uint32_t r1, uint32_t r2, uint32_t r3, uint64_t r4) {
	sceClibPrintf("\nSceNpTrophy has init");
	int ret = TAI_CONTINUE(SceUID, trophy_init_hook, r1, r2, r3, r4);
	if(ret<0) {
		sceClibSnprintf(display,128, "Error: SceNpTrophy init has ret: %x", ret);
	} else {
		isInit = 1;
	}

	return ret;
}

static tai_hook_ref_t fame_hook;
SceUID thid = -1;

int sceDisplaySetFrameBuf_patched(const SceDisplayFrameBuf *pParam, int sync) {
	blit_set_frame_buf(pParam);
	drawScreen();
	return TAI_CONTINUE(int, fame_hook, pParam, sync);
}


static tai_hook_ref_t load_hook;

int hook_sysmodule_load(uint16_t id) {
  int ret;
  ret = TAI_CONTINUE(int, load_hook, id);
  if (ret >= 0) { // load successful
	switch (id) {
	  case SCE_SYSMODULE_NP_TROPHY:
		if(hook[1] < 0) hook[1] = taiHookFunctionImport(
				&trophy_cxt_hook,
				mod_name,
				TAI_ANY_LIBRARY,
				0xC49FD33F,
				sceNpTrophyCreateContext_patched);
		if(hook[2] < 0) hook[2] = taiHookFunctionImport(&fame_hook, 
			TAI_MAIN_MODULE,
			TAI_ANY_LIBRARY,
			0x7A410B64, // sceDisplaySetFrameBuf
			sceDisplaySetFrameBuf_patched);
			sceClibPrintf("buf hook: %x\n", hook[2]);

		if(hook[3] < 0) hook[3] = taiHookFunctionImport(
				&trophy_init_hook,
				mod_name,
				TAI_ANY_LIBRARY,
				0x34516838,
				 sceNpTrophyInit_patched);
		sceClibPrintf("cxt hook: %x\n", hook[1]);
		break;
	  default:
		break;
	}
  }
  return ret;
}
void unhookAll() {
	if (hook[1] >= 0) taiHookRelease(hook[1],trophy_cxt_hook);
	if (hook[2] >= 0) taiHookRelease(hook[2],fame_hook);
	if (hook[3] >= 0) taiHookRelease(hook[3],trophy_init_hook);
	for(int i = 1; i < 4; i++)
		hook[i] = -1;
}
void UnlockAllTrophies() {
	sceClibPrintf("\nhandle ret %x",sceNpTrophyCreateHandle(&handle));
	int ret = -1;
    SceNpTrophyId trophyId = 0;
    
    isCredits = 1;
	do {		
		blit_setup();
		blit_clear(0x00FF0000);
		blit_set_color(0x00FF0000, 0x00FFFFFF);
		blit_string_ctr(20,".-. BLUE SCREEN OF SILICA .-.");
		drawScreen();
		SceNpTrophyId plat;
		ret = sceNpTrophyUnlockTrophy(*context, handle, trophyId++, &plat);
		sceClibSnprintf(display, 128, "Unlocked Trophy %d (ret: %x)", trophyId, ret);
		sceClibPrintf("\nUnlocked Trophy %lx (ret: %x)", trophyId, ret);
		
	} while(trophyId == 1 || ret >= 0 || ret == 0x8055160f );
	if(ret == 0x8055160e){
		sceClibSnprintf(display,128, "All trophies have been unlocked.");
		sceKernelDelayThread(3 * 1000 * 1000);
		unhookAll();
		isCxt = 0;
		isInit = 0;
	}
	//sceClibPrintf("\nhandle unret %x", sceNpTrophyDestroyHandle(&handle));	
}

void UnlockSpecificTrophy(int TrophyId){
        sceClibPrintf("\nhandle ret %x", sceNpTrophyCreateHandle(&handle));
        SceNpTrophyId speTrophyId = TrophyId;
        SceNpTrophyId plat;
        int ret;

        isCredits = 1;
        blit_clear(0x00FF0000);
        drawScreen();
        
        ret = sceNpTrophyUnlockTrophy(*context, handle, speTrophyId, &plat); 
        switch(ret){
        case 0x8055160e:
                sceClibSnprintf(display, 128, "Invalid Trophy Id %d (ret:%x)", TrophyId, ret);
                sceClibPrintf("\nInvalid Trophy Id %d (ret:%x)", TrophyId, ret);
                break;
        case 0x8055160f:
                sceClibSnprintf(display, 128, "Trophy %d already unlocked (ret:%x)", TrophyId, ret);
                sceClibPrintf("\nTrophy %d already unlocked (ret:%x)", TrophyId, ret);
                break;
        case 0x80551610:
                sceClibSnprintf(display, 128, "Cant unlock Platinum trophy %d (ret:%x)", TrophyId, ret);
                sceClibPrintf("\nCant unlock Platinum trophy %d (ret:%x)", TrophyId, ret);
                break;
        default:
                sceClibSnprintf(display, 128, "Unlocked Trophy %d (ret:%x)", TrophyId, ret);
                sceClibPrintf("\nUnlocked Trophy %d (ret:%x)", TrophyId, ret);
                break;
  }
        drawScreen();
        sceKernelDelayThread(5 * 1000 * 1000);
        menu = 0;
        isCredits = 0;
        blit_clear(0x00FF0000);
        sceClibSnprintf(display, 128, "");
}

//nid searcher, maybe someone could use it for sceioopen;)
static tai_hook_ref_t start_mod_child_hook;

SceUID sceKernelLoadStartModule_child_patched(char *path, SceSize args, void *argp, int flags, SceKernelLMOption *option, int *status) {
	int modid = sceKernelLoadModule(path, flags, option);
	SceKernelModuleInfo info;
	info.size = sizeof(info);
	sceKernelGetModuleInfo(modid, &info);
	sceClibPrintf("\nloaded: %s", path);
	int ret = -1;
	if(!isExtFound) {
		unhookAll();
		hook[3] = taiHookFunctionImport(
				&trophy_init_hook,
				info.module_name,
				TAI_ANY_LIBRARY,
				0x34516838,
				sceNpTrophyInit_patched);
		sceClibPrintf("hook[3]: %x", hook[3]);	
		if(hook[3]>=0) {
			if (hook[0] >= 0) taiHookRelease(hook[0],load_hook);
			unhookAll();
			sceKernelUnloadModule(modid, 0, NULL);
			ret = TAI_CONTINUE(int, start_mod_child_hook, path, args, argp, flags, option, status);
			hook[0] = taiHookFunctionImport(&load_hook,			 // Output a reference
					info.module_name,			  // Name of module being hooked
					0x03FCF19D,			 // NID specifying SceSysmodule
					0x79A0160A,			 // NID specifying sceSysmoduleLoadModule
					hook_sysmodule_load);   // Name of the hook function	
			strcpy(mod_name_buf, info.module_name);
			mod_name = &mod_name_buf[0];
			isExtFound = 1;
		}
	}

	if(ret < 0) {
		sceKernelUnloadModule(modid, 0, NULL);
		ret = TAI_CONTINUE(int, start_mod_child_hook, path, args, argp, flags, option, status);
	}
	return ret;
}

static tai_hook_ref_t start_mod_par_hook;

SceUID sceKernelLoadStartModule_parent_patched(char *path, SceSize args, void *argp, int flags, SceKernelLMOption *option, int *status) {
	int modid = sceKernelLoadModule(path, flags, option);
	int ret = -1;
	SceKernelModuleInfo info;
	info.size = sizeof(info);
	sceKernelGetModuleInfo(modid, &info);
	sceClibPrintf("\nloaded: %s", path);	
	if (isExt==0) {
		unhookAll();
		sceClibPrintf("\nModname: %s", info.module_name);
		hook[3] = taiHookFunctionImport(
			&trophy_init_hook,
			info.module_name,
			TAI_ANY_LIBRARY,
			0x34516838,
			sceNpTrophyInit_patched);
		sceClibPrintf("hook[3]: %x", hook[3]);
		
		if(hook[3] >= 0) {
			unhookAll();
			if (hook[0] >= 0) taiHookRelease(hook[0],load_hook);
			sceKernelUnloadModule(modid, 0, NULL);
			ret = TAI_CONTINUE(int, start_mod_par_hook, path, args, argp, flags, option, status);
			hook[0] = taiHookFunctionImport(&load_hook,			 // Output a reference
					info.module_name,			  // Name of module being hooked
					0x03FCF19D,			 // NID specifying SceSysmodule
					0x79A0160A,			 // NID specifying sceSysmoduleLoadModule
				hook_sysmodule_load);   // Name of the hook function	
			if(hook[0] >= 0) {
				strcpy(mod_name_buf, info.module_name);
				mod_name = &mod_name_buf[0];
				isExt = 1;
			}
			sceClibPrintf("hook[0]: %x", hook[0]);

		} else {
			unhookAll();
			if(hook[5] < 0)  hook[5] = taiHookFunctionImport(
						&start_mod_child_hook,
						info.module_name,
						TAI_ANY_LIBRARY,
						0x2DCC4AFA,
						sceKernelLoadStartModule_child_patched);

			if(hook[5] >= 0) {
				isExt = 1;
				unhookAll();
				if (hook[5] >= 0) taiHookRelease(hook[5],start_mod_child_hook);
				sceKernelUnloadModule(modid, 0, NULL);
				ret = TAI_CONTINUE(int, start_mod_par_hook, path, args, argp, flags, option, status);
				hook[5] = taiHookFunctionImport(
							&start_mod_child_hook,
							info.module_name,
							TAI_ANY_LIBRARY,
							0x2DCC4AFA,
							sceKernelLoadStartModule_child_patched);
				sceClibPrintf("hook[5]: %x", hook[5]);
			}
		}
	}
	if(ret < 0) {
		sceKernelUnloadModule(modid, 0, NULL);
		ret = TAI_CONTINUE(int, start_mod_par_hook, path, args, argp, flags, option, status);
	}
	sceClibPrintf("loaded");
	return ret;
}

int checkButtons(int port, tai_hook_ref_t ref_hook, SceCtrlData *ctrl, int count) {
	SceCtrlData pad;

    memset(&pad, 0, sizeof(SceCtrlData));
    sceCtrlPeekBufferPositive2(0, &pad, 1);
    
	if(ref_hook!=0&&!isHidden&&isCxt&&isInit){
            if((pad.buttons & (SCE_CTRL_R1|SCE_CTRL_L1)) == (SCE_CTRL_R1|SCE_CTRL_L1)) {
                    menu = 1;
                    blit_setup();
                    blit_clear(0x00FF0000);
                    blit_set_color(0x00FF0000, 0x00FFFFFF);
                    blit_string_ctr(20,".-. BLUE SCREEN OF SILICA .-.");
                    drawScreen();
                    sceClibPrintf("\nUnlocking trophies");
                    UnlockAllTrophies();
            }
            else if ((pad.buttons & (SCE_CTRL_START|SCE_CTRL_L1)) == (SCE_CTRL_START|SCE_CTRL_L1)){
                    menu = 2;
            }
            else if(menu == 2){
                    if(((pad.buttons & (SCE_CTRL_UP|SCE_CTRL_L1)) == (SCE_CTRL_UP|SCE_CTRL_L1)) && (trophyIdChoice < 256)){
							trophyIdChoice++;
                            sceKernelDelayThread(200000);
					} else if(((pad.buttons & (SCE_CTRL_DOWN|SCE_CTRL_L1)) == (SCE_CTRL_DOWN|SCE_CTRL_L1)) && (trophyIdChoice > 0)){
							trophyIdChoice--;
                            sceKernelDelayThread(200000);
					} else if((pad.buttons & (SCE_CTRL_CIRCLE|SCE_CTRL_L1)) == (SCE_CTRL_CIRCLE|SCE_CTRL_L1)){
                            sceClibPrintf("\nUnlocking specific trophy");
                            UnlockSpecificTrophy(trophyIdChoice);
                    }
            }
    } else if (isHidden&&((pad.buttons & (SCE_CTRL_START|SCE_CTRL_SELECT)) == (SCE_CTRL_START|SCE_CTRL_SELECT)))
            isHidden = 0;
    
    return TAI_CONTINUE(int, ref_hook, port, ctrl, count);
}
    

static tai_hook_ref_t ref_hook1;
static int keys_patched1(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook1, ctrl, count);
}   

static tai_hook_ref_t ref_hook2;
static int keys_patched2(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook2, ctrl, count);
}   

static tai_hook_ref_t ref_hook3;
static int keys_patched3(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook3, ctrl, count);
}   

static tai_hook_ref_t ref_hook4;
static int keys_patched4(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook4, ctrl, count);
}   
void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	hook[6] = taiHookFunctionImport(&ref_hook1, 
									  TAI_MAIN_MODULE,
									  TAI_ANY_LIBRARY,
									  0xA9C3CED6, // sceCtrlPeekBufferPositive
									  keys_patched1);

	hook[7] = taiHookFunctionImport(&ref_hook2, 
									  TAI_MAIN_MODULE,
									  TAI_ANY_LIBRARY,
									  0x15F81E8C, // sceCtrlPeekBufferPositive2
									  keys_patched2);

	hook[8] = taiHookFunctionImport(&ref_hook3, 
									  TAI_MAIN_MODULE,
									  TAI_ANY_LIBRARY,
									  0x67E7AB83, // sceCtrlReadBufferPositive
									  keys_patched3);

	hook[9] = taiHookFunctionImport(&ref_hook4, 
									  TAI_MAIN_MODULE,
									  TAI_ANY_LIBRARY,
									  0xC4226A3E, // sceCtrlReadBufferPositive2
									  keys_patched4);	

	hook[4] = taiHookFunctionImport(
		&start_mod_par_hook,
		TAI_MAIN_MODULE,
		TAI_ANY_LIBRARY,
		0x2DCC4AFA,
		sceKernelLoadStartModule_parent_patched);
	if(hook[0] < 0) hook[0] = taiHookFunctionImport(&load_hook,			 // Output a reference
		TAI_MAIN_MODULE,			  // Name of module being hooked
		0x03FCF19D,			 // NID specifying SceSysmodule
		0x79A0160A,			 // NID specifying sceSysmoduleLoadModule
		hook_sysmodule_load);   // Name of the hook function
		sceClibPrintf("sys hook: %x\n", hook[0]);
	return SCE_KERNEL_START_SUCCESS;

}

int module_stop(SceSize argc, const void *args) {
	unhookAll();
	if (hook[4] >= 0) taiHookRelease(hook[4],start_mod_par_hook);
	if (hook[5] >= 0) taiHookRelease(hook[5],start_mod_child_hook);
	if (hook[6] >= 0) taiHookRelease(hook[6], ref_hook1);
	if (hook[7] >= 0) taiHookRelease(hook[7], ref_hook2);
	if (hook[8] >= 0) taiHookRelease(hook[8], ref_hook3);
	if (hook[9] >= 0) taiHookRelease(hook[9], ref_hook4);
	return SCE_KERNEL_STOP_SUCCESS;
}

