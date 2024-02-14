#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"

class UGameplay_Tags_Manager;

/**
 * GameplayTags
 *
 *	On compile this will be filled with the native gameplay tags.
*/

struct F_Gameplay_Tags
{
    public:

        //Getter function to retrieve a GameplayTags.
        static const F_Gameplay_Tags& Get() {return Gameplay_Tags; }

        //Initialize the Gameplay Tags in the GameplayTagsManager.
        static void Initialize_Native_Tags();

        #pragma region Parkour_Tags

        FGameplayTag(Parkour_State_Free_Roam);
        FGameplayTag(Parkour_State_Ready_To_Climb);
        FGameplayTag(Parkour_State_Climb);
        FGameplayTag(Parkour_State_Mantle);
        FGameplayTag(Parkour_State_Vault);
        FGameplayTag(Parkour_Action_No_Action);
        FGameplayTag(Parkour_Action_Braced_Climb);
        FGameplayTag(Parkour_Action_Braced_Climb_Falling_Climb);
        FGameplayTag(Parkour_Action_Braced_Climb_Climb_Up);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Up);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Left);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Right);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Left_Up);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Right_Up);
        FGameplayTag(Parkour_Action_Braced_Climb_Hop_Down);
        FGameplayTag(Parkour_Action_Free_Hang);
        FGameplayTag(Parkour_Action_Free_Hang_Falling_Climb);
        FGameplayTag(Parkour_Action_Free_Hang_Climb_Up);
        FGameplayTag(Parkour_Action_Free_Hang_Hop_Left);
        FGameplayTag(Parkour_Action_Free_Hang_Hop_Right);
        FGameplayTag(Parkour_Action_Free_Hang_Hop_Down);
        FGameplayTag(Parkour_Action_Corner_Move);
        FGameplayTag(Parkour_Action_Mantle);
        FGameplayTag(Parkour_Action_Low_Vault);
        FGameplayTag(Parkour_Action_High_Vault);
        FGameplayTag(Parkour_Direction_No_Direction);
        FGameplayTag(Parkour_Direction_Left);
        FGameplayTag(Parkour_Direction_Right);
        FGameplayTag(Parkour_Direction_Forward);
        FGameplayTag(Parkour_Direction_Backward);
        FGameplayTag(Parkour_Direction_Forward_Left);
        FGameplayTag(Parkour_Direction_Forward_Right);
        FGameplayTag(Parkour_Direction_Backward_Left);
        FGameplayTag(Parkour_Direction_Backward_Right);
        FGameplayTag(Parkour_Climb_Style_Braced_Climb);
        FGameplayTag(Parkour_Climb_Style_Free_Hang);

        #pragma endregion


    protected:

        //Registers all of the tags with the GameplayTags Manager.
        void Add_All_Tags(UGameplayTagsManager& Gameplay_Tag_Manager);

        //Helper function used by "Add_All_Tags" to register a single tag with the GameplayTags Manager.
        void Add_Tag(FGameplayTag& Out_Tag, const ANSICHAR* Tag_Name);

    private:

        //Used by the getter function "F_Gameplay_Tags& Get()" to retrieve GameplayTags.
        static F_Gameplay_Tags Gameplay_Tags;

};