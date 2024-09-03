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

    #pragma region Parkour_Locomotion

    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_State_Implementation(const FGameplayTag& New_Parkour_State) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_State(const FGameplayTag& New_Parkour_State);


    //Used to set new Parkour Action within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Action(const FGameplayTag& New_Parkour_Action);


    //Used to set new Parkour State within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Climb_Style(const FGameplayTag& New_Climb_Style) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Climb_Style(const FGameplayTag& New_Climb_Style);
    
     //Used to set new Parkour Wall Ride Side within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Wall_Run_Side(const FGameplayTag& New_Wall_Run_Side);

    //Used to set new Parkour Direction within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Direction(const FGameplayTag& New_Climb_Direction) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Direction(const FGameplayTag& Parkour_Direction);

    //Used to set new Parkour Direction within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Direction(const FGameplayTag& New_Climb_Direction) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //will only be defined within the animation blueprint within the editor.
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Parkour_Stairs_Direction(const FGameplayTag& New_Parkour_Stairs_Direction);

    #pragma endregion


    #pragma region Limbs_Location_And_Rotations

    #pragma region Left_Limbs

    //Used to set new "Left_Hand_Shimmy_Location" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Hand_Shimmy_Location(const FVector& New_Left_Hand_Shimmy_Location);

    //Used to set new "Left_Hand_Shimmy_Rotation" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Hand_Shimmy_Rotation(const FRotator& New_Left_Hand_Shimmy_Rotation);


    //Used to set new "Left_Foot_Shimmy_Location" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Foot_Shimmy_Location(const FVector& New_Left_Foot_Shimmy_Location);


    //Used to set new "Set_Left_Foot_Shimmy_Rotation" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Left_Foot_Shimmy_Rotation(const FRotator& New_Left_Foot_Shimmy_Rotation);

    #pragma endregion

    #pragma region Right_Limbs


    //Used to set new "Set_Right_Hand_Shimmy_Location" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Hand_Shimmy_Location(const FVector& New_Right_Hand_Shimmy_Location);


    //Used to set new "Set_Right_Hand_Shimmy_Rotation" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Hand_Shimmy_Rotation(const FRotator& New_Right_Hand_Shimmy_Rotation);


    //Used to set new "Set_Right_Foot_Shimmy_Location" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Foot_Shimmy_Location(const FVector& New_Right_Foot_Shimmy_Location);


    //Used to set new "Set_Right_Foot_Shimmy_Rotation" within the Animation Blueprint. This function will be called from UCustom_Movemement_Component using
    //the interface pointer "Parkour_Interface". Said pointer will be initialized with a cast from the pointer of the "Anim_Instance" 
    //(the class which will use this interface) to "IParkour_Locomotion_Interface". Instead of calling this function directly via the pointer 
    //previously mentioned, a version of this function prefixed with "Execute_" will be called.

    //The function will be declared (just as it is here) in the animation instance followed by a declaration of its implementable version 
    //"virtual bool Set_Parkour_Action(const FGameplayTag& New_Parkour_Action) override;". The implementable version will be defined 
    //in the animation instance cpp file, however, the version of the function with the "UFUNCTION(BlueprintCallable, BlueprintNativeEvent)" macro
    //may only be defined within the animation blueprint within the editor (overrides cpp definition).
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Set_Right_Foot_Shimmy_Rotation(const FRotator& New_Right_Foot_Shimmy_Rotation);


    #pragma endregion

    #pragma endregion

};