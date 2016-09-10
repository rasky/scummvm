/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bladerunner/script/script.h"

namespace BladeRunner {

void ScriptKP06::InitializeScene() {
	if (Game_Flag_Query(579) ) {
		Setup_Scene_Information(-755.0f, 8.26f, -665.0f, 640);
	} else {
		Setup_Scene_Information(-868.0f, 8.26f, -8.0f, 0);
	}
	Scene_Exit_Add_2D_Exit(0, 270, 445, 639, 479, 2);
	Scene_Exit_Add_2D_Exit(1, 320, 158, 352, 220, 0);
	Ambient_Sounds_Add_Looping_Sound(464, 34, 1, 1);
	Ambient_Sounds_Add_Looping_Sound(383, 27, 1, 1);
	Ambient_Sounds_Add_Looping_Sound(384, 90, 1, 1);
	Ambient_Sounds_Add_Sound(440, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(441, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(442, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(375, 5, 180, 50, 100, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(376, 5, 180, 50, 100, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(377, 5, 180, 50, 100, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(443, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(444, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	Ambient_Sounds_Add_Sound(445, 2, 100, 25, 33, -100, 100, -101, -101, 0, 0);
	if (Game_Flag_Query(576) ) {
		Scene_Loop_Start_Special(0, 0, 0);
		Scene_Loop_Set_Default(1);
		Game_Flag_Reset(576);
	} else {
		Scene_Loop_Set_Default(1);
	}
}

void ScriptKP06::SceneLoaded() {
	Obstacle_Object("TRASH CAN WITH FIRE", true);
	Obstacle_Object("MOONBUS", true);
	Obstacle_Object("STAIR 1", true);
	Obstacle_Object("COCKPIT FRONT", true);
	Unobstacle_Object("OBSTACLEBOX28", true);
	Unobstacle_Object("OBSTACLEBOX32", true);
	Unclickable_Object("TRASH CAN WITH FIRE");
}

bool ScriptKP06::MouseClick(int x, int y) {
	return false;
}

bool ScriptKP06::ClickedOn3DObject(const char *objectName, bool a2) {
	return false;
}

bool ScriptKP06::ClickedOnActor(int actorId) {
	if (actorId == 8 && !Game_Flag_Query(714)) {
		if (Actor_Clue_Query(8, 145)) {
			Actor_Face_Actor(0, 8, true);
			Actor_Says(0, 8610, 15);
			Actor_Says(8, 290, 3);
		} else if (Actor_Clue_Query(0, 145) ) {
			Actor_Says(8, 280, 3);
			Actor_Says(8, 290, 3);
			Actor_Clue_Acquire(8, 145, 1, 0);
		} else {
			Actor_Says(0, 2320, 3);
			Actor_Says(0, 2325, 3);
			Actor_Says(8, 300, 3);
			Actor_Says(8, 310, 3);
		}
	}
	return false;
}

bool ScriptKP06::ClickedOnItem(int itemId, bool a2) {
	return false;
}

bool ScriptKP06::ClickedOnExit(int exitId) {
	if (exitId == 0) {
		if (!Loop_Actor_Walk_To_XYZ(0, -868.0f, 8.26f, -68.0f, 0, 1, false, 0)) {
			Ambient_Sounds_Remove_All_Non_Looping_Sounds(1);
			Ambient_Sounds_Remove_All_Looping_Sounds(1);
			Game_Flag_Set(577);
			Set_Enter(9, 45);
		}
		return true;
	}
	if (exitId == 1) {
		if (Actor_Clue_Query(8, 145) || Actor_Query_Goal_Number(8) != 416) {
			if (!Loop_Actor_Walk_To_XYZ(0, -731.0f, 8.26f, -657.0f, 0, 1, false, 0)) {
				if (Game_Flag_Query(653)) {
					if (!Game_Flag_Query(714)) {
						Player_Set_Combat_Mode(false);
					}
				} else if (Actor_Query_Goal_Number(1) == 433) {
					Actor_Set_Goal_Number(1, 499);
				} else {
					Actor_Set_Goal_Number(53, 499);
				}
				Ambient_Sounds_Remove_All_Non_Looping_Sounds(1);
				Ambient_Sounds_Remove_All_Looping_Sounds(1);
				Game_Flag_Set(578);
				Set_Enter(48, 47);
			}
		} else if (Actor_Clue_Query(0, 145) ) {
			Actor_Says(8, 280, 3);
			Actor_Says(8, 290, 3);
			Actor_Clue_Acquire(8, 145, 1, 0);
			Loop_Actor_Walk_To_XYZ(0, -731.0f, 8.26f, -657.0f, 0, 0, true, 0);
			Player_Set_Combat_Mode(false);
			Ambient_Sounds_Remove_All_Non_Looping_Sounds(1);
			Ambient_Sounds_Remove_All_Looping_Sounds(1);
			Game_Flag_Set(578);
			Set_Enter(48, 47);
		} else {
			Actor_Set_Goal_Number(8, 417);
		}
		return true;
	}
	return false;
}

bool ScriptKP06::ClickedOn2DRegion(int region) {
	return false;
}

void ScriptKP06::SceneFrameAdvanced(int frame) {
}

void ScriptKP06::ActorChangedGoal(int actorId, int newGoal, int oldGoal, bool currentSet) {
}

void ScriptKP06::PlayerWalkedIn() {
	if (!Game_Flag_Query(653) && Game_Flag_Query(579)) {
		Game_Flag_Reset(579);
		if (Actor_Query_Goal_Number(1) == 499) {
			Actor_Face_Actor(1, 0, true);
			Actor_Says(1, 2530, 13);
			Actor_Face_Actor(0, 1, true);
			Actor_Says(0, 6200, 11);
			Actor_Says(1, 2540, 15);
			Actor_Says(1, 2550, 12);
			Actor_Says(0, 6205, 14);
			if (Actor_Query_Friendliness_To_Other(1, 0) < 50) {
				Actor_Says(1, 2560, 12);
				Actor_Says(0, 6210, 14);
				Actor_Says(1, 2570, 13);
				Actor_Says(0, 6215, 14);
				Actor_Says(1, 2580, 15);
				Actor_Says(1, 2590, 12);
			}
			Async_Actor_Walk_To_Waypoint(0, 551, 0, 0);
			Delay(1000);
			Actor_Says(0, 6220, -1);
			Delay(3000);
			Ambient_Sounds_Remove_All_Non_Looping_Sounds(1);
			Ambient_Sounds_Remove_All_Looping_Sounds(1);
			Outtake_Play(25, 0, -1);
			Game_Over();
			//return true;
			return;
		} else {
			Actor_Set_Goal_Number(53, 499);
			Actor_Face_Actor(53, 0, true);
			Actor_Says(53, 220, 13);
			Actor_Face_Actor(0, 53, true);
			Actor_Says(0, 6245, 11);
			Actor_Says(53, 230, 14);
			if (Game_Flag_Query(714)) {
				Actor_Says(0, 6250, 15);
				Actor_Says(53, 240, 13);
				Delay(1000);
				Actor_Says(0, 6255, 17);
				Actor_Says(53, 250, 14);
				Delay(1000);
			}
			Actor_Says(53, 260, 12);
			Actor_Says(0, 6260, 15);
			Actor_Says(53, 270, 13);
			Actor_Says(53, 280, 15);
			Actor_Says(0, 6265, 14);
			Actor_Says(53, 290, 14);
			Actor_Says(53, 300, 15);
			Actor_Says(0, 6270, 11);
			Async_Actor_Walk_To_Waypoint(0, 550, 0, 0);
			Async_Actor_Walk_To_Waypoint(53, 551, 0, 0);
			Actor_Says(53, 310, -1);
			Delay(3000);
			Outtake_Play(26, 0, -1);
			Game_Over();
			//return true;
			return;
		}
	} else {
		if (Actor_Query_Goal_Number(8) == 414) {
			Loop_Actor_Walk_To_XYZ(0, -809.0f, 8.26f, -619.0f, 0, 0, false, 0);
			Actor_Face_Actor(0, 8, true);
			Actor_Set_Goal_Number(8, 415);
		}
		if (Actor_Query_Goal_Number(1) == 431) {
			Actor_Set_Goal_Number(1, 432);
		}
		//return false;
		return;
	}
}

void ScriptKP06::PlayerWalkedOut() {
	if (Game_Flag_Query(578) && Actor_Query_Goal_Number(1) == 433) {
		Actor_Set_Goal_Number(1, 499);
	}
}

void ScriptKP06::DialogueQueueFlushed(int a1) {
}

} // End of namespace BladeRunner
