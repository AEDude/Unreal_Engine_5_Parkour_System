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

    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_State_Implementation(const FGameplayTag& New_Parkour_State) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_State(const FGameplayTag& New_Parkour_State);


    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);


    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Climb_Style(const FGameplayTag& New_Climb_Style) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Style(const FGameplayTag& New_Climb_Style);
    

    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Climb_Direction(const FGameplayTag& New_Climb_Direction) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Set_Climb_Direction(const FGameplayTag& New_Climb_Direction);

};