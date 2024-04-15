#pragma once

UENUM(BlueprintType)
enum class EGround_Locomotion_State : uint8
{
    EGLS_Idle UMETA (DisplayName = "Idle"),
    EGLS_Walking UMETA (DisplayName = "Walking"),
    EGLS_Jogging UMETA (DisplayName = "Jogging"),
    EGLS_Sprinting UMETA (DisplayName = "Sprinting"),

    ELS_Max UMETA(DisplayName = "DefaultMAX")
};