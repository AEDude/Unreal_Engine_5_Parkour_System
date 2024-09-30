
#include "Native_Gameplay_Tags/Native_Gameplay_Tags.h"

#pragma region Parkour_Region


#pragma region Parkour_State

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Free_Roam, "Parkour.State.Free.Roam");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Jump, "Parkour.State.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Mantle, "Parkour.State.Mantle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Vault, "Parkour.State.Vault");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Ready_To_Climb, "Parkour.State.Ready.To.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Climb,"Parkour.State.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Wall_Run_Initialize, "Parkour.State.Initialize.Wall.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Wall_Run, "Parkour.State.Wall.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Initialize_Wall_Pipe_Climb, "Parkour.State.Initialize.Wall.Pipe.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Wall_Pipe_Climb, "Parkour.State.Wall.Pipe.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Initialize_Balance_Walk, "Parkour.State.Initialize.Balance.Walk");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Balance_Walk, "Parkour.State.Balance.Walk");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Tic_Tac, "Parkour.State.Tic.Tac");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Stairs, "Parkour.State.Stairs");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Initialize_Slide, "Parkour.State.Initialize.Slide");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_State_Slide, "Parkour.State.Slide");

#pragma endregion


#pragma region Parkour_Action

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_No_Action, "Parkour.Action.No.Action");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Free_Roam_Accelerating_Drop, "Parkour.Action.Free.Roam.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb, "Parkour.Action.Braced.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Accelerating_Drop, "Parkour.Action.Braced.Climb.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Normal_Drop, "Parkour.Action.Braced.Climb.Normal.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Falling_Climb, "Parkour.Action.Braced.Climb.Falling.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Falling_Climb_Slipped, "Parkour.Action.Braced.Climb.Falling.Climb.Slipped");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Climb_Up, "Parkour.Action.Braced.Climb.Climb.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Up, "Parkour.Action.Braced.Climb.Hop.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Left, "Parkour.Action.Braced.Climb.Hop.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Right, "Parkour.Action.Braced.Climb.Hop.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Left_Up, "Parkour.Action.Braced.Climb.Hop.Left.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Right_Up, "Parkour.Action.Braced.Climb.Hop.Right.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Down, "Parkour.Action.Braced.Climb.Hop.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Left_Down, "Parkour.Action.Braced.Climb.Hop.Left.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Hop_Right_Down, "Parkour.Action.Braced.Climb.Hop.Right.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Shimmy_180_Shimmy, "Parkour.Action.Braced.Climb.Shimmy.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Exit_Jump_Forward, "Parkour.Action.Braced.Climb.Exit.Jump.Forward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Braced_Climb_Exit_Jump_Backward, "Parkour.Action.Braced.Climb.Exit.Jump.Backward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang, "Parkour.Action.FreeHang");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Accelerating_Drop, "Parkour.Action.FreeHang.Accelerating.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Normal_Drop, "Parkour.Action.FreeHang.Normal.Drop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Falling_Climb, "Parkour.Action.FreeHang.Falling.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Falling__Climb_Hanging_Jump, "Parkour.Action.FreeHang.Falling.Climb.Hanging.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Climb_Up, "Parkour.Action.FreeHang.Climb.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Hop_Left, "Parkour.Action.FreeHang.Hop.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Hop_Right, "Parkour.Action.FreeHang.Hop.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Hop_Down, "Parkour.Action.FreeHang.Hop.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Shimmy_180_Shimmy, "Parkour.Action.FreeHang.Shimmy.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_FreeHang_Exit_Jump, "Parkour.Action.FreeHang.Exit.Jump");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Corner_Move, "Parkour.Action.Corner.Move");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_Start_Left, "Parkour.Action.Wall.Run.Start.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_Start_Right, "Parkour.Action.Wall.Run.Start.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_L_Jump_F, "Parkour.Action.Wall.Run.L.Jump.F");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_R_Jump_F, "Parkour.Action.Wall.Run.R.Jump.F");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_Left_Jump_90_R, "Parkour.Action.Wall.Run.Left.Jump.90.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_Right_Jump_90_L, "Parkour.Action.Wall.Run.Right.Jump.90.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_L_Finish, "Parkour.Action.Wall.Run.L.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Run_R_Finish, "Parkour.Action.Wall.Run.R.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Attach_Grounded, "Parkour.Action.Wall.Pipe.Attach.Grounded");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Attach_Airborne, "Parkour.Action.Wall.Pipe.Attach.Airborne");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Jump_Up, "Parkour.Action.Wall.Pipe.Jump.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Jump_Down, "Parkour.Action.Wall.Pipe.Jump.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Jump_Left, "Parkour.Action.Wall.Pipe.Jump.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Jump_Right, "Parkour.Action.Wall.Pipe.Jump.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Fall_Down, "Parkour.Action.Wall.Pipe.Fall.Down");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Wall_Pipe_Climb_Up_2_Hand, "Parkour.Action.Wall.Pipe.Climb.Up.2.Hand");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Mantle, "Parkour.Action.Mantle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Low_Vault, "Parkour.Action.Low.Vault");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_High_Vault, "Parkour.Action.High.Vault");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Up, "Parkour.Action.Jump.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Accurate_Jump_Start_L, "Parkour.Action.Accurate.Jump.Start.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Accurate_Jump_Start_R, "Parkour.Action.Accurate.Jump.Start.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Accurate_Jump_Start_L_Warp, "Parkour.Action.Accurate.Jump.Start.L.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Accurate_Jump_Start_R_Warp, "Parkour.Action.Accurate.Jump.Start.R.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Accurate_Jump_Finish, "Parkour.Action.Accurate.Jump.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Front_L_Start, "Parkour.Action.Jump.Front.L.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Front_R_Start, "Parkour.Action.Jump.Front.R.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Front_L_Start_Warp, "Parkour.Action.Jump.Front.L.Start.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Front_R_Start_Warp, "Parkour.Action.Jump.Front.R.Start.Warp");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_Finish, "Parkour.Action.Jump.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_One_L, "Parkour.Action.Jump.One.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Jump_One_R, "Parkour.Action.Jump.One.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Walk_90_L, "Parkour.Action.Balance.Walk.90.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Walk_90_R, "Parkour.Action.Balance.Walk.90.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Walk_180, "Parkour.Action.Balance.Walk.180");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Walk_Automatic_Hop, "Parkour.Action.Balance.Walk.Automatic.Hop");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Drop_L_Hanging, "Parkour.Action.Balance.Drop.L.Hanging");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Balance_Drop_R_Hanging, "Parkour.Action.Balance.Drop.R.Hanging");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Hanging_Climb_Up_To_Balanced_Walk_L, "Parkour.Action.Hanging.Climb.Up.To.Balanced.Walk.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Hanging_Climb_Up_To_Balanced_Walk_R, "Parkour.Action.Hanging.Climb.Up.To.Balanced.Walk.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Idle_Wall_Vault_On, "Parkour.Action.Idle.Wall.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Idle_Wall_Vault_Over, "Parkour.Action.Idle.Wall.Vault.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Running_Wall_Vault_On, "Parkour.Action.Running.Wall.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Running_Wall_Vault_Over, "Parkour.Action.Running.Wall.Vault.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Running_Wall_Vault_Under_Bar, "Parkour.Action.Running.Wall.Vault.Under.Bar");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Running_Wall_Vault_Over_180_Shimmy, "Parkour.Action.Running.Wall.Vault.Over.180.Shimmy");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_L_Over_Front_Wall, "Parkour.Action.Tic.Tac.L.Over.Front.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_R_Over_Front_Wall, "Parkour.Action.Tic.Tac.R.Over.Front.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_L_On_Front_Wall_To_Idle, "Parkour.Action.Tic.Tac.L.On.Front.Wall.To.Idle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_L_On_Front_Wall_To_Run, "Parkour.Action.Tic.Tac.L.On.Front.Wall.To.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_R_On_Front_Wall_To_Idle, "Parkour.Action.Tic.Tac.R.On.Front.Wall.To.Idle");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_R_On_Front_Wall_To_Run, "Parkour.Action.Tic.Tac.R.On.Front.Wall.To.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_L_Over_Right_Wall, "Parkour.Action.Tic.Tac.L.Over.Right.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Tic_Tac_R_Over_Left_Wall, "Parkour.Action.Tic.Tac.R.Over.Left.Wall");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_L_Start, "Parkour.Action.Slide.L.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_R_Start, "Parkour.Action.Slide.R.Start");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_L_Finish, "Parkour.Action.Slide.L.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_R_Finish, "Parkour.Action.Slide.R.Finish");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_L_To_R, "Parkour.Action.Slide.L.To.R");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Slide_R_To_L, "Parkour.Action.Slide.R.To.L");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Unarmed_Crouch_Entry, "Parkour.Action.Unarmed.Crouch.Entry");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Unarmed_Crouch_Exit, "Parkour.Action.Unarmed.Crouch.Exit");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_L_Small_Failed, "Parkour.Action.Vertical.Wall.Run.L.Small.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_R_Small_Failed, "Parkour.Action.Vertical.Wall.Run.R.Small.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_L_Large_Failed, "Parkour.Action.Vertical.Wall.Run.L.Large.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_R_Large_Failed, "Parkour.Action.Vertical.Wall.Run.R.Large.Failed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_L_Large, "Parkour.Action.Vertical.Wall.Run.L.Large");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_L_Small, "Parkour.Action.Vertical.Wall.Run.L.Small");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_R_Large, "Parkour.Action.Vertical.Wall.Run.R.Large");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Action_Vertical_Wall_Run_R_Small, "Parkour.Action.Vertical.Wall.Run.L.Small");


#pragma endregion


#pragma region Parkour_Direction

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_No_Direction, "Parkour.Direction.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Left, "Parkour.Direction.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Right, "Parkour.Direction.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Forward, "Parkour.Direction.Forward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Backward, "Parkour.Direction.Backward");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Forward_Left, "Parkour.Direction.Forward.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Forward_Right, "Parkour.Direction.Forward.Right");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Backward_Left, "Parkour.Direction.Backward.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Direction_Backward_Right, "Parkour.Direction.Backward.Right");


#pragma endregion


#pragma region Parkour_Direction

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Stairs_Direction_None, "Parkour.Stairs.Direction.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Stairs_Direction_Up, "Parkour.Stairs.Direction.Up");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Stairs_Direction_Down, "Parkour.Stairs.Direction.Down");

#pragma endregion


#pragma region Parkour_Climb_Style

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Climb_Style_None, "Parkour.Climb.Style.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Climb_Style_Braced_Climb, "Parkour.Climb.Style.Braced.Climb");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Climb_Style_FreeHang, "Parkour.Climb.Style.FreeHang");

#pragma endregion


#pragma region Parkour_Wall_Run

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Wall_Run_Side_None, "Parkour.Wall.Run.Side.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Wall_Run_Side_Left, "Parkour.Wall.Run.Side.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Wall_Run_Side_Right, "Parkour.Wall.Run.Side.Right");

#pragma endregion


#pragma region Parkour_Landing

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Landing_Down_Light, "Parkour.Landing.Down.Light");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Landing_Down_Impact, "Parkour.Landing.Down.Impact");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Landing_Down_Front_L, "Parkour.Landing.Down.Front");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Landing_Down_Roll, "Parkour.Landing.Down.Roll");

#pragma endregion


#pragma region Parkour_Tic_Tac


UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Jump_On, "Parkour.Tic.Tac.L.Jump.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Jump_On_Run, "Parkour.Tic.Tac.L.Jump.On.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Vault_On, "Parkour.Tic.Tac.L.Vault.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Jump_On, "Parkour.Tic.Tac.R.Jump.On");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Jump_On_Run, "Parkour.Tic.Tac.R.Jump.On.Run");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Vault_On, "Parkour.Tic.Tac.R.Vault.On");


UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Jump_Over, "Parkour.Tic.Tac.L.Jump.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Reverse_Over, "Parkour.Tic.Tac.L.Reverse.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Speed_Over, "Parkour.Tic.Tac.L.Speed.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Jump_Over, "Parkour.Tic.Tac.R.Jump.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Reverse_Over, "Parkour.Tic.Tac.R.Reverse.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Speed_Over, "Parkour.Tic.Tac.R.Speed.Over");


UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_L_Jump_Side_Over, "Parkour.Tic.Tac.L.Jump.Side.Over");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Tic_Tac_R_Jump_Side_Over, "Parkour.Tic.Tac.R.Jump.Side.Over");


#pragma endregion


#pragma region Parkour_Slide

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Slide_Side_None, "Parkour.Slide.Side.None");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Slide_Side_Left, "Parkour.Slide.Side.Left");

UE_DEFINE_GAMEPLAY_TAG(TAG_Parkour_Slide_Side_Right, "Parkour.Slide.Side.Right");

#pragma endregion


#pragma endregion
