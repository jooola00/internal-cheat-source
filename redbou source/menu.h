#include "sdk.h"
#include "sdk.h"
#include "sdk.h"
#include "sdk.h"
#include "sdk.h"

void fn::keys()
{
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		settings.menu = !settings.menu;
	}
}

bool fn::actorloop(ImGuiWindow& window)
{
	uintptr_t MyTeamIndex;
	uintptr_t EnemyTeamIndex;
	float FOVmax = 200.f;

	X = SDK::Utilities::SpoofCall(GetSystemMetrics, SM_CXSCREEN);
	Y = SDK::Utilities::SpoofCall(GetSystemMetrics, SM_CYSCREEN);

	uintptr_t GWorld = SDK::Utilities::read<uintptr_t>(Details.UWORLD);
	if (!GWorld) return false;

	uintptr_t Gameinstance = SDK::Utilities::read<uint64_t>(GWorld + SDK::Classes::StaticOffsets::OwningGameInstance);
	if (!Gameinstance) return false;

	uintptr_t LocalPlayer = SDK::Utilities::read<uint64_t>(Gameinstance + SDK::Classes::StaticOffsets::LocalPlayers);
	if (!LocalPlayer) return false;

	uintptr_t LocalPlayers = SDK::Utilities::read<uint64_t>(LocalPlayer);
	if (!LocalPlayers) return false;

	PlayerController = SDK::Utilities::read<uint64_t>(LocalPlayers + SDK::Classes::StaticOffsets::PlayerController);
	if (!PlayerController) return false;

	PlayerCameraManager = SDK::Utilities::read<uintptr_t>(PlayerController + SDK::Classes::StaticOffsets::PlayerCameraManager);
	if (!PlayerCameraManager) return false;

	LocalPawn = SDK::Utilities::read<uint64_t>(PlayerController + SDK::Classes::StaticOffsets::AcknowledgedPawn);
	if (!LocalPawn) return false;

	uintptr_t Ulevel = SDK::Utilities::read<uintptr_t>(GWorld + SDK::Classes::StaticOffsets::PersistentLevel);
	if (!Ulevel) return false;

	uintptr_t AActors = SDK::Utilities::read<uintptr_t>(Ulevel + SDK::Classes::StaticOffsets::AActors);
	if (!AActors) return false;

	uintptr_t ActorCount = SDK::Utilities::read<int>(Ulevel + SDK::Classes::StaticOffsets::ActorCount);
	if (!ActorCount) return false;

	GetPlayerViewPoint(PlayerCameraManager, &SDK::Utilities::CamLoc, &SDK::Utilities::CamRot);

	for (int i = 0; i < ActorCount; i++) {

		uintptr_t CurrentActor = SDK::Utilities::read<uint64_t>(AActors + i * sizeof(uintptr_t));

		std::string NameCurrentActor = GetObjectName(CurrentActor);

		if (strstr(NameCurrentActor.c_str(), xorthis("PlayerPawn_")) || strstr(NameCurrentActor.c_str(), xorthis("PlayerPawn_Athena_Phoebe_C")) || strstr(NameCurrentActor.c_str(), xorthis("WeakSpot")))
		{
			uintptr_t MyState = SDK::Utilities::read<uintptr_t>(LocalPawn + SDK::Classes::StaticOffsets::PlayerState);
			if (!MyState) continue;

			MyTeamIndex = SDK::Utilities::read<uintptr_t>(MyState + SDK::Classes::StaticOffsets::TeamIndex);
			if (!MyTeamIndex) continue;

			uintptr_t EnemyState = SDK::Utilities::read<uintptr_t>(CurrentActor + SDK::Classes::StaticOffsets::PlayerState);
			if (!EnemyState) continue;

			EnemyTeamIndex = SDK::Utilities::read<uintptr_t>(EnemyState + SDK::Classes::StaticOffsets::TeamIndex);
			if (!EnemyTeamIndex) continue;

			SDK::Structs::Vector3 Headbox, bottom, pelviss;

			SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &Headbox);
			SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 2, &pelviss);
			SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &bottom);

			SDK::Classes::AController::WorldToScreen(SDK::Structs::Vector3(Headbox.x, Headbox.y, Headbox.z + 20), &Headbox);
			SDK::Classes::AController::WorldToScreen(SDK::Structs::Vector3(pelviss.x, pelviss.y, pelviss.z + 70), &pelviss);
			SDK::Classes::AController::WorldToScreen(bottom, &bottom);

			if (Headbox.x == 0 && Headbox.y == 0) continue;
			if (bottom.x == 0 && bottom.y == 0) continue;
			if (MyTeamIndex == EnemyTeamIndex) continue;
			if (CurrentActor == LocalPawn) continue;

			float Height = Headbox.y - bottom.y;
			if (Height < 0)
				Height = Height * (-1.f);
			float Width = Height * 0.65;
			Headbox.x = Headbox.x - (Width / 2);

			float Height1 = Headbox.y - bottom.y;

			if (Height1 < 0)
				Height1 = Height1 * (-1.f);
			float Width1 = Height1 * 0.35;

			SDK::Structs::Vector3 head2, neck, pelvis, chest, leftShoulder, rightShoulder, leftElbow, rightElbow, leftHand, rightHand, leftLeg, rightLeg, leftThigh, rightThigh, leftFoot, rightFoot, leftFeet, rightFeet, leftFeetFinger, rightFeetFinger;

			SDK::Structs::Vector3 viewPoint;
			bool IsVisible;
			ImColor col = { 255, 255, 255, 220 };
			ImColor SkelColor = { 255, 255, 255, 220};
			if (settings.vischeck) {
				IsVisible = SDK::Classes::APlayerCameraManager::LineOfSightTo((PVOID)PlayerController, (PVOID)CurrentActor, &viewPoint);
				if (IsVisible) {
					col = { 200, 25, 25 };
				}
				else {
					col = { 200, 200, 200 };
				}
				if (IsVisible)
				{
					SkelColor = {200, 25, 25 };
				}
				else
				{
					SkelColor = { 200, 20, 20 };
				}
			}

			if (SDK::Utilities::DiscordHelper::IsAiming())
			{
				if (SDK::Utilities::CheckIfInFOV(CurrentActor, FOVmax)) {

					if (settings.vischeck and IsVisible)
					{
						if (settings.memory)
						{
							SDK::Structs::Vector3 NewAngle = SDK::Utilities::GetRotation(CurrentActor);

							if (NewAngle.x == 0 && NewAngle.y == 0) continue;

							if (settings.smoothness > 0)
								NewAngle = SDK::Utilities::SmoothAngles(SDK::Utilities::CamRot, NewAngle);

							NewAngle.z = 0;

							SDK::Classes::AController::ValidateClientSetRotation(NewAngle, false);
							SDK::Classes::AController::ClientSetRotation(NewAngle, false);
						}
					}
					else if (!settings.vischeck)
					{
						SDK::Structs::Vector3 NewAngle = SDK::Utilities::GetRotation(CurrentActor);

						if (NewAngle.x == 0 && NewAngle.y == 0) continue;

						if (settings.smoothness > 0)
							NewAngle = SDK::Utilities::SmoothAngles(SDK::Utilities::CamRot, NewAngle);

						NewAngle.z = 0;

						SDK::Classes::AController::ValidateClientSetRotation(NewAngle, false);
						SDK::Classes::AController::ClientSetRotation(NewAngle, false);

					}
				}
			}

			if (settings.corner && SDK::Classes::Utils::CheckInScreen(CurrentActor, X, Y))
			{
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &head2);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 65, &neck);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 2, &pelvis);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 36, &chest);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 9, &leftShoulder);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 37, &rightShoulder);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 10, &leftElbow);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 38, &rightElbow);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 11, &leftHand);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 39, &rightHand);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 67, &leftLeg);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 74, &rightLeg);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 73, &leftThigh);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 80, &rightThigh);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 68, &leftFoot);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 75, &rightFoot);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 71, &leftFeet);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 78, &rightFeet);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 72, &leftFeetFinger);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 79, &rightFeetFinger);
				SDK::Classes::AController::WorldToScreen(head2, &head2);
				SDK::Classes::AController::WorldToScreen(neck, &neck);
				SDK::Classes::AController::WorldToScreen(pelvis, &pelvis);
				SDK::Classes::AController::WorldToScreen(chest, &chest);
				SDK::Classes::AController::WorldToScreen(leftShoulder, &leftShoulder);
				SDK::Classes::AController::WorldToScreen(rightShoulder, &rightShoulder);
				SDK::Classes::AController::WorldToScreen(leftElbow, &leftElbow);
				SDK::Classes::AController::WorldToScreen(rightElbow, &rightElbow);
				SDK::Classes::AController::WorldToScreen(leftHand, &leftHand);
				SDK::Classes::AController::WorldToScreen(rightHand, &rightHand);
				SDK::Classes::AController::WorldToScreen(leftLeg, &leftLeg);
				SDK::Classes::AController::WorldToScreen(rightLeg, &rightLeg);
				SDK::Classes::AController::WorldToScreen(leftThigh, &leftThigh);
				SDK::Classes::AController::WorldToScreen(rightThigh, &rightThigh);
				SDK::Classes::AController::WorldToScreen(leftFoot, &leftFoot);
				SDK::Classes::AController::WorldToScreen(rightFoot, &rightFoot);
				SDK::Classes::AController::WorldToScreen(leftFeet, &leftFeet);
				SDK::Classes::AController::WorldToScreen(rightFeet, &rightFeet);
				SDK::Classes::AController::WorldToScreen(leftFeetFinger, &leftFeetFinger);
				SDK::Classes::AController::WorldToScreen(rightFeetFinger, &rightFeetFinger);

				int array[20] = { head2.x, neck.x, pelvis.x, chest.x, leftShoulder.x, rightShoulder.x, leftElbow.x, rightElbow.x, leftHand.x, rightHand.x, leftLeg.x, rightLeg.x, leftThigh.x, rightThigh.x, leftFoot.x, rightFoot.x, leftFeet.x, rightFeet.x, leftFeetFinger.x, rightFeetFinger.x };
				int mostright = array[0];
				int mostleft = array[0];

				for (int mostrighti = 0; mostrighti < 20; mostrighti++)
				{
					if (array[mostrighti] > mostright)
						mostright = array[mostrighti];
				}

				for (int mostlefti = 0; mostlefti < 20; mostlefti++)
				{
					if (array[mostlefti] < mostleft)
						mostleft = array[mostlefti];
				}

				if (settings.outlined)
				{
					fn::general_overlay::Outline(pelviss.x - ((mostright - mostleft) / 2), pelviss.y, (mostright - mostleft), Height1, ImColor(0, 0, 0, 220));
				}
				fn::general_overlay::DrawCorneredBox(pelviss.x - ((mostright - mostleft) / 2), pelviss.y, (mostright - mostleft), Height1, ImColor(col), 1.5);
			}
			if (settings.distance && SDK::Classes::Utils::CheckInScreen(CurrentActor, X, Y))
			{
				fn::general_overlay::OutlinedRBGText(head2.x - 30, head2.y - 50, ImColor(col), "");
			}
			if (settings.lines && SDK::Classes::Utils::CheckInScreen(CurrentActor, X, Y))
			{
				fn::general_overlay::DrawLine(X / 2, Y, bottom.x, bottom.y, ImColor(0, 0, 0), 0.5f);
				fn::general_overlay::DrawLine(X / 2, Y, bottom.x, bottom.y, ImColor(0, 0, 0), 0.7f);
				fn::general_overlay::DrawLine(X / 2, Y, bottom.x, bottom.y, ImColor(0, 0, 0), 0.9f);
			}
			if (settings.debug)
			{
				for (int youareretardedmonkey = 0; youareretardedmonkey < 87; youareretardedmonkey++) {
					SDK::Structs::Vector3 out;
					SDK::Structs::Vector3 outw2s;

					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, youareretardedmonkey, &out);
					SDK::Classes::AController::WorldToScreen(out, &outw2s);

					fn::general_overlay::OutlinedString(window, std::to_string(youareretardedmonkey), ImVec2(outw2s.x, outw2s.y), ImColor(255, 255, 255, 255));
				}
			}
			if (settings.skeleton)
			{
				SDK::Structs::Vector3 head2, neck, pelvis, chest, leftShoulder, rightShoulder, leftElbow, rightElbow, leftHand, rightHand, leftLeg, rightLeg, leftThigh, rightThigh, leftFoot, rightFoot, leftFeet, rightFeet, leftFeetFinger, rightFeetFinger;

				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &head2);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 65, &neck);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 2, &pelvis);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 36, &chest);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 9, &leftShoulder);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 62, &rightShoulder);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 10, &leftElbow);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 38, &rightElbow);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 11, &leftHand);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 39, &rightHand);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 67, &leftLeg);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 74, &rightLeg);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 73, &leftThigh);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 80, &rightThigh);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 68, &leftFoot);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 75, &rightFoot);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 71, &leftFeet);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 78, &rightFeet);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 72, &leftFeetFinger);
				SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 79, &rightFeetFinger);

				SDK::Classes::AController::WorldToScreen(head2, &head2);
				SDK::Classes::AController::WorldToScreen(neck, &neck);
				SDK::Classes::AController::WorldToScreen(pelvis, &pelvis);
				SDK::Classes::AController::WorldToScreen(chest, &chest);
				SDK::Classes::AController::WorldToScreen(leftShoulder, &leftShoulder);
				SDK::Classes::AController::WorldToScreen(rightShoulder, &rightShoulder);
				SDK::Classes::AController::WorldToScreen(leftElbow, &leftElbow);
				SDK::Classes::AController::WorldToScreen(rightElbow, &rightElbow);
				SDK::Classes::AController::WorldToScreen(leftHand, &leftHand);
				SDK::Classes::AController::WorldToScreen(rightHand, &rightHand);
				SDK::Classes::AController::WorldToScreen(leftLeg, &leftLeg);
				SDK::Classes::AController::WorldToScreen(rightLeg, &rightLeg);
				SDK::Classes::AController::WorldToScreen(leftThigh, &leftThigh);
				SDK::Classes::AController::WorldToScreen(rightThigh, &rightThigh);
				SDK::Classes::AController::WorldToScreen(leftFoot, &leftFoot);
				SDK::Classes::AController::WorldToScreen(rightFoot, &rightFoot);
				SDK::Classes::AController::WorldToScreen(leftFeet, &leftFeet);
				SDK::Classes::AController::WorldToScreen(rightFeet, &rightFeet);
				SDK::Classes::AController::WorldToScreen(leftFeetFinger, &leftFeetFinger);
				SDK::Classes::AController::WorldToScreen(rightFeetFinger, &rightFeetFinger);

				fn::general_overlay::DrawLine(head2.x, head2.y, neck.x, neck.y, SkelColor, 1);
				fn::general_overlay::DrawLine(neck.x, neck.y, pelvis.x, pelvis.y, SkelColor, 1);
				fn::general_overlay::DrawLine(chest.x, chest.y, leftShoulder.x, leftShoulder.y, SkelColor, 1);
				fn::general_overlay::DrawLine(chest.x, chest.y, rightShoulder.x, rightShoulder.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftShoulder.x, leftShoulder.y, leftElbow.x, leftElbow.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightShoulder.x, rightShoulder.y, rightElbow.x, rightElbow.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftElbow.x, leftElbow.y, leftHand.x, leftHand.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightElbow.x, rightElbow.y, rightHand.x, rightHand.y, SkelColor, 1);
				fn::general_overlay::DrawLine(pelvis.x, pelvis.y, leftLeg.x, leftLeg.y, SkelColor, 1);
				fn::general_overlay::DrawLine(pelvis.x, pelvis.y, rightLeg.x, rightLeg.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftLeg.x, leftLeg.y, leftThigh.x, leftThigh.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightLeg.x, rightLeg.y, rightThigh.x, rightThigh.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftThigh.x, leftThigh.y, leftFoot.x, leftFoot.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightThigh.x, rightThigh.y, rightFoot.x, rightFoot.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftFoot.x, leftFoot.y, leftFeet.x, leftFeet.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightFoot.x, rightFoot.y, rightFeet.x, rightFeet.y, SkelColor, 1);
				fn::general_overlay::DrawLine(leftFeet.x, leftFeet.y, leftFeetFinger.x, leftFeetFinger.y, SkelColor, 1);
				fn::general_overlay::DrawLine(rightFeet.x, rightFeet.y, rightFeetFinger.x, rightFeetFinger.y, SkelColor, 1);
			}
		}
	}
	return true;
}


void fn::render(ImGuiWindow& window)
{
	// begin drawing

	fn::general_overlay::OutlinedString(window, "your text", ImVec2(100, 100), ImColor(255, 255, 255, 255), false);
}
float X1 = GetSystemMetrics(0) / 2 - 1;
float Y1 = GetSystemMetrics(1) / 2 - 1;
float width;
float height;
void fn::menu()
{            // put expoilts here

















































	fn::keys();

	float ScreenCenterX = X1;
	float ScreenCenterY = Y1;

	if (settings.crosshair)
	{

		ImGui::GetOverlayDrawList()->AddLine(ImVec2(ScreenCenterX - 8.f, ScreenCenterY), ImVec2((ScreenCenterX - 8.f) + (8.f * 2), ScreenCenterY), ImColor(0, 0, 15));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(ScreenCenterX, ScreenCenterY - 8.f), ImVec2(ScreenCenterX, (ScreenCenterY - 8.f) + (8.f * 2)), ImColor(0, 0, 15));
	}

	if (settings.aimbot_fov)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(X1, Y1), settings.radius, ImColor(0, 0, 0), 20);
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(X1, Y1), settings.radius + 0.5f, ImColor(0, 0, 0), 20);
	}

	if (settings.menu)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(15, 15);
		style.WindowRounding = 20.0f;
		style.FramePadding = ImVec2(2, 2);
		style.FrameRounding = 20.0f;
		style.ItemSpacing = ImVec2(20, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 0.0f;
		style.ScrollbarRounding = 40.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 5.0f;
		style.ChildRounding = 5.f;

		style.WindowTitleAlign.x = 0.50f;
		style.FrameRounding = 20.0f;

		style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.90f, 0.90f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(3.30f, 0.30f, 0.30f, 0.84f);
		style.Colors[ImGuiCol_Button] = ImVec4(5.48f, 3.72f, 0.89f, 0.49f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(5.50f, 0.69f, 0.99f, 0.68f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(2.80f, 1.50f, 4.50f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.85f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(2.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(3.70f, 0.70f, 0.90f, 0.60f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.20f, 0.20f, 0.20f, 0.35f);

		ImGui::SetNextWindowSize({ 300.f,300.f });
		ImGui::Begin("your text here", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
		{
			ImGui::Checkbox("memory", &settings.memory);
			ImGui::Checkbox("fov", &settings.aimbot_fov);
			ImGui::Checkbox("vischeck", &settings.vischeck);
			ImGui::SliderFloat("fov slider", &settings.radius, 0, 200);
			ImGui::SliderFloat("smooth", &settings.smoothness, 0, 100);
			ImGui::Checkbox("corners", &settings.corner);
			ImGui::Checkbox("snaplines", &settings.lines);
			ImGui::Checkbox("box outline", &settings.outlined);
			ImGui::Checkbox("distance", &settings.distance);
			ImGui::Checkbox("box", &settings.box);
			ImGui::Checkbox("crosshair", &settings.crosshair);
		}
		ImGui::End();
	}
}