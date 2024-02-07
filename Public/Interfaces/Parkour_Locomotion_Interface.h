#pragma once

#include "Parkour_Locomotion_Interface.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UParkour_Locomotion_Interface : public UInterface
{
    GENERATED_BODY()
};

class IParkour_Locomotion_Interface
{    
    GENERATED_BODY()

public:
    /** Add interface function declarations here */

    //Used to set new Parkour State withing the Animation Blueprint
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_State(const FGameplayTag& New_Parkour_State);

    //Used to set the new Parkour Action within the Animation Blueprint.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);

    //Used to set the new Climb Style within the Animation Blueprint.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Style(const FGameplayTag& New_Climb_Style);
    
    //Used to set the new Climb Direction within the Animation Blueprint.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Direction(const FGameplayTag& New_Climb_Direction);

};