#pragma once

UENUM(BlueprintType)
enum class EGround_Locomotion_Starting_Direction : uint8
{
    EGLSD_Forward UMETA (DisplayName = "Forward"),
    EGLSD_Left UMETA (DisplayName = "Left"),
    //EGLSD_Forward_Left UMETA (DisplayName = "Forward_Left"),
    EGLSD_Right UMETA (DisplayName = "Right"),
    //EGLSD_Forward_Right UMETA (DisplayName = "Forward_Right"),
    //EGLSD_Backward UMETA (DisplayName = "Backward"),
    EGLSD_Backward_Left UMETA (DisplayName = "BackWard_Left"),
    EGLSD_Backward_Right UMETA (DisplayName = "Backward_Right"),

    EGLSD_Max UMETA (DisplayName = DefaultMAX)
};