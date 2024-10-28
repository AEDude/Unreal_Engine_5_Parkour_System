
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"

#pragma region Character_Region


#pragma region Character_State

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Free_Roam, "Character.State.Free.Roam");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Jump, "Character.State.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Landing_Impact,"Character.State.Landing.Impact");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Mantle, "Character.State.Mantle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Vault, "Character.State.Vault");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Ready_To_Climb, "Character.State.Ready.To.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Climb,"Character.State.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Exiting_Climb,"Character.State.Exiting.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Wall_Run_Initialize, "Character.State.Initialize.Wall.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Wall_Run, "Character.State.Wall.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Initialize_Wall_Pipe_Climb, "Character.State.Initialize.Wall.Pipe.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Wall_Pipe_Climb, "Character.State.Wall.Pipe.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Initialize_Balance_Walk, "Character.State.Initialize.Balance.Walk");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Balance_Walk, "Character.State.Balance.Walk");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Tic_Tac, "Character.State.Tic.Tac");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Stairs, "Character.State.Stairs");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Initialize_Slide, "Character.State.Initialize.Slide");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Slide, "Character.State.Slide");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Roll, "Character.State.Roll");

#pragma endregion


#pragma region Character_Action

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_No_Action, "Character.Action.No.Action");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Free_Roam_Accelerating_Drop, "Character.Action.Free.Roam.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb, "Character.Action.Braced.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Accelerating_Drop, "Character.Action.Braced.Climb.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Normal_Drop, "Character.Action.Braced.Climb.Normal.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Falling_Climb, "Character.Action.Braced.Climb.Falling.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Falling_Climb_Slipped, "Character.Action.Braced.Climb.Falling.Climb.Slipped");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Climb_Up, "Character.Action.Braced.Climb.Climb.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Up, "Character.Action.Braced.Climb.Hop.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Left, "Character.Action.Braced.Climb.Hop.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Right, "Character.Action.Braced.Climb.Hop.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Left_Up, "Character.Action.Braced.Climb.Hop.Left.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Right_Up, "Character.Action.Braced.Climb.Hop.Right.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Down, "Character.Action.Braced.Climb.Hop.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Left_Down, "Character.Action.Braced.Climb.Hop.Left.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Hop_Right_Down, "Character.Action.Braced.Climb.Hop.Right.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Shimmy_180_Shimmy, "Character.Action.Braced.Climb.Shimmy.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Exit_Jump_Forward, "Character.Action.Braced.Climb.Exit.Jump.Forward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Braced_Climb_Exit_Jump_Backward, "Character.Action.Braced.Climb.Exit.Jump.Backward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang, "Character.Action.FreeHang");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Accelerating_Drop, "Character.Action.FreeHang.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Normal_Drop, "Character.Action.FreeHang.Normal.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Falling_Climb, "Character.Action.FreeHang.Falling.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Falling__Climb_Hanging_Jump, "Character.Action.FreeHang.Falling.Climb.Hanging.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Climb_Up, "Character.Action.FreeHang.Climb.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Hop_Left, "Character.Action.FreeHang.Hop.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Hop_Right, "Character.Action.FreeHang.Hop.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Hop_Down, "Character.Action.FreeHang.Hop.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Shimmy_180_Shimmy, "Character.Action.FreeHang.Shimmy.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_FreeHang_Exit_Jump, "Character.Action.FreeHang.Exit.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Corner_Move, "Character.Action.Corner.Move");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_Start_Left, "Character.Action.Wall.Run.Start.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_Start_Right, "Character.Action.Wall.Run.Start.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_L_Jump_F, "Character.Action.Wall.Run.L.Jump.F");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_R_Jump_F, "Character.Action.Wall.Run.R.Jump.F");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_Left_Jump_90_R, "Character.Action.Wall.Run.Left.Jump.90.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_Right_Jump_90_L, "Character.Action.Wall.Run.Right.Jump.90.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_L_Finish, "Character.Action.Wall.Run.L.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Run_R_Finish, "Character.Action.Wall.Run.R.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Attach_Grounded, "Character.Action.Wall.Pipe.Attach.Grounded");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Attach_Airborne, "Character.Action.Wall.Pipe.Attach.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Jump_Up, "Character.Action.Wall.Pipe.Jump.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Jump_Down, "Character.Action.Wall.Pipe.Jump.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Jump_Left, "Character.Action.Wall.Pipe.Jump.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Jump_Right, "Character.Action.Wall.Pipe.Jump.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Fall_Down, "Character.Action.Wall.Pipe.Fall.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Wall_Pipe_Climb_Up_2_Hand, "Character.Action.Wall.Pipe.Climb.Up.2.Hand");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Mantle, "Character.Action.Mantle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Up, "Character.Action.Jump.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Accurate_Jump_Start_L, "Character.Action.Accurate.Jump.Start.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Accurate_Jump_Start_R, "Character.Action.Accurate.Jump.Start.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Accurate_Jump_Start_L_Warp, "Character.Action.Accurate.Jump.Start.L.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Accurate_Jump_Start_R_Warp, "Character.Action.Accurate.Jump.Start.R.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Accurate_Jump_Finish, "Character.Action.Accurate.Jump.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Front_L_Start, "Character.Action.Jump.Front.L.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Front_R_Start, "Character.Action.Jump.Front.R.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Front_L_Start_Warp, "Character.Action.Jump.Front.L.Start.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Front_R_Start_Warp, "Character.Action.Jump.Front.R.Start.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_Finish, "Character.Action.Jump.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_One_L, "Character.Action.Jump.One.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Jump_One_R, "Character.Action.Jump.One.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Walk_90_L, "Character.Action.Balance.Walk.90.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Walk_90_R, "Character.Action.Balance.Walk.90.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Walk_180, "Character.Action.Balance.Walk.180");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Walk_Automatic_Hop, "Character.Action.Balance.Walk.Automatic.Hop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Drop_L_Hanging, "Character.Action.Balance.Drop.L.Hanging");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Balance_Drop_R_Hanging, "Character.Action.Balance.Drop.R.Hanging");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Hanging_Climb_Up_To_Balanced_Walk_L, "Character.Action.Hanging.Climb.Up.To.Balanced.Walk.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Hanging_Climb_Up_To_Balanced_Walk_R, "Character.Action.Hanging.Climb.Up.To.Balanced.Walk.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Wall_Vault_On, "Character.Action.Idle.Wall.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Wall_Vault_Over, "Character.Action.Idle.Wall.Vault.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Wall_Vault_On, "Character.Action.Running.Wall.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Wall_Vault_Over, "Character.Action.Running.Wall.Vault.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Wall_Vault_Under_Bar, "Character.Action.Running.Wall.Vault.Under.Bar");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Wall_Vault_Over_180_Shimmy, "Character.Action.Running.Wall.Vault.Over.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_L_Over_Front_Wall, "Character.Action.Tic.Tac.L.Over.Front.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_R_Over_Front_Wall, "Character.Action.Tic.Tac.R.Over.Front.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_L_On_Front_Wall_To_Idle, "Character.Action.Tic.Tac.L.On.Front.Wall.To.Idle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_L_On_Front_Wall_To_Run, "Character.Action.Tic.Tac.L.On.Front.Wall.To.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_R_On_Front_Wall_To_Idle, "Character.Action.Tic.Tac.R.On.Front.Wall.To.Idle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_R_On_Front_Wall_To_Run, "Character.Action.Tic.Tac.R.On.Front.Wall.To.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_L_Over_Right_Wall, "Character.Action.Tic.Tac.L.Over.Right.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Tic_Tac_R_Over_Left_Wall, "Character.Action.Tic.Tac.R.Over.Left.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_L_Start, "Character.Action.Slide.L.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_R_Start, "Character.Action.Slide.R.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_L_Finish, "Character.Action.Slide.L.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_R_Finish, "Character.Action.Slide.R.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_L_To_R, "Character.Action.Slide.L.To.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Slide_R_To_L, "Character.Action.Slide.R.To.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Unarmed_Crouch_Entry, "Character.Action.Unarmed.Crouch.Entry");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Unarmed_Crouch_Exit, "Character.Action.Unarmed.Crouch.Exit");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_L_Small_Failed, "Character.Action.Vertical.Wall.Run.L.Small.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_R_Small_Failed, "Character.Action.Vertical.Wall.Run.R.Small.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_L_Large_Failed, "Character.Action.Vertical.Wall.Run.L.Large.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_R_Large_Failed, "Character.Action.Vertical.Wall.Run.R.Large.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_L_Large, "Character.Action.Vertical.Wall.Run.L.Large");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_L_Small, "Character.Action.Vertical.Wall.Run.L.Small");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_R_Large, "Character.Action.Vertical.Wall.Run.R.Large");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Vertical_Wall_Run_R_Small, "Character.Action.Vertical.Wall.Run.L.Small");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_L_On, "Character.Action.Walking.Step.L.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_R_On, "Character.Action.Walking.Step.R.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_L_On, "Character.Action.Running.Step.L.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_R_On, "Character.Action.Running.Step.R.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_L_On_Off_To_Ground, "Character.Action.Walking.Step.L.On.Off.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_R_On_Off_To_Ground, "Character.Action.Walking.Step.R.On.Off.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_L_On_Off_To_Ground, "Character.Action.Running.Step.L.On.Off.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_R_On_Off_To_Ground, "Character.Action.Running.Step.R.On.Off.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_L_On_Off_To_Airborne, "Character.Action.Walking.Step.L.On.Off.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Walking_Step_R_On_Off_To_Airborne, "Character.Action.Walking.Step.R.On.Off.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_L_On_Off_To_Airborne, "Character.Action.Running.Step.L.On.Off.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Step_R_On_Off_To_Airborne, "Character.Action.Running.Step.R.On.Off.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Vault_L_To_Ground, "Character.Action.Idle.Vault.L.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Vault_R_To_Ground, "Character.Action.Idle.Vault.R.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Jump_Step_On_L, "Character.Action.Running.Jump.Step.On.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Running_Jump_Step_On_R, "Character.Action.Running.Jump.Step.On.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Medium_Vault_L_To_Ground, "Character.Action.Medium.Vault.L.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Medium_Vault_R_To_Ground, "Character.Action.Medium.Vault.R.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_High_Vault_L_To_Ground, "Character.Action.High.Vault.L.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_High_Vault_R_To_Ground, "Character.Action.High.Vault.R.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Long_Vault_L_To_Ground, "Character.Action.Long.Vault.L.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Long_Vault_R_To_Ground, "Character.Action.Long.Vault.R.To.Ground");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Vault_L_To_Airborne, "Character.Action.Idle.Vault.L.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Idle_Vault_R_To_Airborne, "Character.Action.Idle.Vault.R.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Medium_Vault_L_To_Airborne, "Character.Action.Medium.Vault.L.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Medium_Vault_R_To_Airborne, "Character.Action.Medium.Vault.R.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_High_Vault_L_To_Airborne, "Character.Action.High.Vault.L.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_High_Vault_R_To_Airborne, "Character.Action.High.Vault.R.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Long_Vault_L_To_Airborne, "Character.Action.Long.Vault.L.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Long_Vault_R_To_Airborne, "Character.Action.Long.Vault.R.To.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Roll_Forward_L, "Character.Action.Roll.Forward.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Roll_Forward_R, "Character.Action.Roll.Forward.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Dive_Roll_Forward_L, "Character.Action.Dive.Roll.Forward.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Action_Dive_Roll_Forward_R, "Character.Action.Dive.Roll.Forward.R");



#pragma endregion


#pragma region Character_Direction

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_No_Direction, "Character.Direction.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Left, "Character.Direction.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Right, "Character.Direction.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Forward, "Character.Direction.Forward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Backward, "Character.Direction.Backward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Forward_Left, "Character.Direction.Forward.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Forward_Right, "Character.Direction.Forward.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Backward_Left, "Character.Direction.Backward.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Direction_Backward_Right, "Character.Direction.Backward.Right");


#pragma endregion


#pragma region Character_Stairs_Direction

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Stairs_Direction_None, "Character.Stairs.Direction.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Stairs_Direction_Up, "Character.Stairs.Direction.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Stairs_Direction_Down, "Character.Stairs.Direction.Down");

#pragma endregion


#pragma region Character_Climb_Style

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Climb_Style_None, "Character.Climb.Style.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Climb_Style_Braced_Climb, "Character.Climb.Style.Braced.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Climb_Style_FreeHang, "Character.Climb.Style.FreeHang");

#pragma endregion


#pragma region Character_Wall_Run

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Wall_Run_Side_None, "Character.Wall.Run.Side.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Wall_Run_Side_Left, "Character.Wall.Run.Side.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Wall_Run_Side_Right, "Character.Wall.Run.Side.Right");

#pragma endregion


#pragma region Character_Landing

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Landing_Down_Light, "Character.Landing.Down.Light");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Landing_Down_Impact, "Character.Landing.Down.Impact");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Landing_Down_Front_L, "Character.Landing.Down.Front");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Landing_Down_Roll, "Character.Landing.Down.Roll");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Landing_Down_Back_Roll, "Character.Landing.Down.Back.Roll");

#pragma endregion


#pragma region Character_Tic_Tac


UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Jump_On, "Character.Tic.Tac.L.Jump.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Jump_On_Run, "Character.Tic.Tac.L.Jump.On.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Vault_On, "Character.Tic.Tac.L.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Jump_On, "Character.Tic.Tac.R.Jump.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Jump_On_Run, "Character.Tic.Tac.R.Jump.On.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Vault_On, "Character.Tic.Tac.R.Vault.On");


UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Jump_Over, "Character.Tic.Tac.L.Jump.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Reverse_Over, "Character.Tic.Tac.L.Reverse.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Speed_Over, "Character.Tic.Tac.L.Speed.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Jump_Over, "Character.Tic.Tac.R.Jump.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Reverse_Over, "Character.Tic.Tac.R.Reverse.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Speed_Over, "Character.Tic.Tac.R.Speed.Over");


UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_L_Jump_Side_Over, "Character.Tic.Tac.L.Jump.Side.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Tic_Tac_R_Jump_Side_Over, "Character.Tic.Tac.R.Jump.Side.Over");


#pragma endregion


#pragma region Character_Slide

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Slide_Side_None, "Character.Slide.Side.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Slide_Side_Left, "Character.Slide.Side.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Slide_Side_Right, "Character.Slide.Side.Right");

#pragma endregion


#pragma endregion
