#include "Gameplay_Tags/Gameplay_Tags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"



// F_Gameplay_Tags F_Gameplay_Tags::Gameplay_Tags;


// void F_Gameplay_Tags::Initialize_Native_Tags()
// {
//     UGameplayTagsManager& Gameplay_Tags_Manager{UGameplayTagsManager::Get()};

//     Gameplay_Tags.Add_All_Tags(Gameplay_Tags_Manager);

//     Gameplay_Tags_Manager.DoneAddingNativeTags();
// }

// void F_Gameplay_Tags::Add_All_Tags(UGameplayTagsManager& Gameplay_Tag_Manager)
// {
//     Add_Tag(Parkour_State_Free_Roam, "Parkour.State.Free.Roam");
//     Add_Tag(Parkour_State_Ready_To_Climb, "Parkour.State.Ready.To.Climb");
//     Add_Tag(Parkour_State_Climb,"Parkour.State.Climb");
//     Add_Tag(Parkour_State_Mantle, "Parkour.State.Mantle");
//     Add_Tag(Parkour_State_Vault, "Parkour.State.Vault");
//     Add_Tag(Parkour_Action_No_Action, "Parkour.Action.No.Action");
//     Add_Tag(Parkour_Action_Braced_Climb, "Parkour.Action.Braced.Climb");
//     Add_Tag(Parkour_Action_Braced_Climb_Falling_Climb, "Parkour.Action.Braced.Climb.Falling.Climb");
//     Add_Tag(Parkour_Action_Braced_Climb_Climb_Up, "Parkour.Action.Braced.Climb.Climb.Up");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Up, "Parkour.Action.Braced.Climb.Hop.Up");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Left, "Parkour.Action.Braced.Climb.Hop.Left");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Right, "Parkour.Action.Braced.Climb.Hop.Right");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Left_Up, "Parkour.Action.Braced.Climb.Hop.Left.Up");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Right_Up, "Parkour.Action.Braced.Climb.Hop.Right.Up");
//     Add_Tag(Parkour_Action_Braced_Climb_Hop_Down, "Parkour.Action.Braced.Climb.Hop.Down" );
//     Add_Tag(Parkour_Action_Free_Hang, "Parkour.Action.Free.Hang");
//     Add_Tag(Parkour_Action_Free_Hang_Falling_Climb, "Parkour.Action.Free.Hang.Falling.Climb");
//     Add_Tag(Parkour_Action_Free_Hang_Climb_Up, "Parkour.Action.FreeHang.Climb.Up");
//     Add_Tag(Parkour_Action_Free_Hang_Hop_Left, "Parkour.Action.Free_Hang.Hop.Left");
//     Add_Tag(Parkour_Action_Free_Hang_Hop_Right, "Parkour.Action.FreeHang.Hop.Right");
//     Add_Tag(Parkour_Action_Free_Hang_Hop_Down, "Parkour.Action.FreeHang.Hop.Down");
//     Add_Tag(Parkour_Action_Corner_Move, "Parkour.Action.Corner.Move");
//     Add_Tag(Parkour_Action_Mantle, "Parkour.Action.Mantle");
//     Add_Tag(Parkour_Action_Low_Vault, "Parkour.Action.Low.Vault");
//     Add_Tag(Parkour_Action_High_Vault, "Parkour.Action.High.Vault");
//     Add_Tag(Parkour_Direction_No_Direction, "Parkour.Direction.None");
//     Add_Tag(Parkour_Direction_Left, "Parkour.Direction.Left");
//     Add_Tag(Parkour_Direction_Right, "Parkour.Direction.Right");
//     Add_Tag(Parkour_Direction_Forward, "Parkour.Direction.Forward");
//     Add_Tag(Parkour_Direction_Backward, "Parkour.Direction.Backward");
//     Add_Tag(Parkour_Direction_Forward_Left, "Parkour.Direction.Forward.Left");
//     Add_Tag(Parkour_Direction_Forward_Right, "Parkour.Direction.Forward.Right");
//     Add_Tag(Parkour_Direction_Backward_Left, "Parkour.Direction.Backward.Left");
//     Add_Tag(Parkour_Direction_Backward_Right, "Parkour.Direction.Backward.Right");
//     Add_Tag(Parkour_Climb_Style_Braced_Climb, "Parkour.Climb.Style.Braced.Climb");
//     Add_Tag(Parkour_Climb_Style_Free_Hang, "Parkour.Climb.Style.Free.Hang");
// }

// void F_Gameplay_Tags::Add_Tag(FGameplayTag& Out_Tag, const ANSICHAR* Tag_Name)
// {
//     Out_Tag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(Tag_Name), FString(TEXT("(Native)")));
// }
