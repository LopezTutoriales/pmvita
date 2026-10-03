#include "common.h"
#include <string.h>

// the N64 reloaded each map's data on entry; these intro maps rely on it when the title loop replays them

extern f32 hos_04_TargetBoomLengthPre;
extern u16* hos_04_ColorBufferPtr;
extern s32 hos_04_TargetBoomLengthPost;
extern s32 hos_04_TargetViewPitch;

extern s32 hos_05_D_802495DC_A3381C;
extern s32 hos_05_D_802495E0_A33820;
extern f32 hos_05_StoryCameraAngle;
extern u16* hos_05_ColorBufPtr;
extern f32 IntroCamStateA_BoomLength;
extern f32 IntroCamStateA_BoomPitch;
extern f32 IntroCamStateA_ViewPitch;
extern f32 IntroCamStateA_Vfov;
extern f32 IntroCamStateB_BoomLength;
extern f32 IntroCamStateB_BoomPitch;
extern f32 IntroCamStateB_ViewPitch;
extern f32 IntroCamStateB_Vfov;
extern s32 hos_05_D_802498F8_A33B38;
extern s32 hos_05_D_802498FC_A33B3C;
extern s32 hos_05_StoryCameraShake1Angle;
extern f32 hos_05_StoryCameraShake1Scale;
extern s32 hos_05_StoryCameraShake2Angle;
extern f32 hos_05_StoryCameraShake2Scale;
extern s32 hos_05_UnusedBowserLeapTime;
extern s32 hos_05_BowserHoverTime;
extern f32 hos_05_UnusedStoryCameraZoomAmt;
extern s32 hos_05_UnusedKammyMoveTime;
extern s32 hos_05_KammyHoverTime;
extern f32 hos_05_BoomLengthInhale;
extern s32 hos_05_CamMoveInhaleTime;
extern f32 hos_05_BoomLengthExhale;
extern s32 hos_05_CamMoveExhaleTime;
extern s32 hos_05_FlyToStarRodTime;
extern s32 hos_05_HoldStarRodTime;
extern f32 hos_05_PanAcrossRoomCamX;
extern f32 hos_05_PanAcrossRoomCamZ;
extern f32 hos_05_PanAcrossRoomAngle;
extern s32 hos_05_PanAcrossRoomTime;
extern f32 hos_05_OrbitKammyFov;
extern f32 hos_05_OrbitKammyBoomLength;
extern f32 hos_05_OrbitKammyCamY;
extern f32 hos_05_OrbitKammyAngle;
extern s32 hos_05_OrbitKammyTime;
extern f32 hos_05_FinalCamMoveBoomLength;
extern s32 hos_05_FlyToBowserTime;
extern s32 hos_05_StoryPageState;
extern s32 hos_05_CurrentStoryPageIdx;
extern s32 hos_05_CurrentStoryPageTime;
extern u32 hos_05_BowserSilhouetteTime;
extern s32 hos_05_FadeAwayTapeTime;
extern s32 hos_05_D_8024ACBC_A34EFC;
extern f32 hos_05_AnimBowser_FlyOff_Time;
extern f32 hos_05_AnimKammy_FlyOff_Time;
extern s32 hos_05_StarshipShimmerAmt;

static void reset_hos_04(void) {
    hos_04_TargetBoomLengthPre = 700.0f;
    hos_04_ColorBufferPtr = NULL;
    hos_04_TargetBoomLengthPost = 0;
    hos_04_TargetViewPitch = 0;
}

static void reset_hos_05(void) {
    hos_05_D_802495DC_A3381C = 0;
    hos_05_D_802495E0_A33820 = 0;
    hos_05_StoryCameraAngle = 240.0f;
    hos_05_ColorBufPtr = NULL;
    IntroCamStateA_BoomLength = 130.4f;
    IntroCamStateA_BoomPitch = 12.4f;
    IntroCamStateA_ViewPitch = -16.8f;
    IntroCamStateA_Vfov = 62.0f;
    IntroCamStateB_BoomLength = 130.4f;
    IntroCamStateB_BoomPitch = 12.4f;
    IntroCamStateB_ViewPitch = -16.8f;
    IntroCamStateB_Vfov = 62.0f;
    hos_05_D_802498F8_A33B38 = 0;
    hos_05_D_802498FC_A33B3C = 0;
    hos_05_StoryCameraShake1Angle = 0;
    hos_05_StoryCameraShake1Scale = 1.0f;
    hos_05_StoryCameraShake2Angle = 0;
    hos_05_StoryCameraShake2Scale = 12.0f;
    hos_05_UnusedBowserLeapTime = 0;
    hos_05_BowserHoverTime = 0;
    hos_05_UnusedStoryCameraZoomAmt = 30.0f;
    hos_05_UnusedKammyMoveTime = 0;
    hos_05_KammyHoverTime = 0;
    hos_05_BoomLengthInhale = 121.6f;
    hos_05_CamMoveInhaleTime = 0;
    hos_05_BoomLengthExhale = 90.0f;
    hos_05_CamMoveExhaleTime = 0;
    hos_05_FlyToStarRodTime = 0;
    hos_05_HoldStarRodTime = 0;
    hos_05_PanAcrossRoomCamX = 40.0f;
    hos_05_PanAcrossRoomCamZ = -40.0f;
    hos_05_PanAcrossRoomAngle = 45.0f;
    hos_05_PanAcrossRoomTime = 0;
    hos_05_OrbitKammyFov = 50.0f;
    hos_05_OrbitKammyBoomLength = 246.1f;
    hos_05_OrbitKammyCamY = 200.0f;
    hos_05_OrbitKammyAngle = 25.0f;
    hos_05_OrbitKammyTime = 0;
    hos_05_FinalCamMoveBoomLength = 130.0f;
    hos_05_FlyToBowserTime = 0;
    hos_05_StoryPageState = 0;
    hos_05_CurrentStoryPageIdx = 0;
    hos_05_CurrentStoryPageTime = 0;
    hos_05_BowserSilhouetteTime = 0;
    hos_05_FadeAwayTapeTime = 30;
    hos_05_D_8024ACBC_A34EFC = 0x00010019;
    hos_05_AnimBowser_FlyOff_Time = 0.0f;
    hos_05_AnimKammy_FlyOff_Time = 0.0f;
    hos_05_StarshipShimmerAmt = 255;
}

void port_reset_map_overlay_data(const char* mapName) {
    if (mapName == NULL) {
        return;
    }
    if (strcmp(mapName, "hos_04") == 0) {
        reset_hos_04();
    } else if (strcmp(mapName, "hos_05") == 0) {
        reset_hos_05();
    }
}
