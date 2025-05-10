#pragma once

#include <cassert>

#include "vector_math.h"
#include "cs_defines.h"

typedef Vector2 BoneData_data_t;

struct BoneData {
	BoneData_data_t hip;		// 0
	BoneData_data_t spine1;		// 1
	BoneData_data_t spine2;		// 2
	BoneData_data_t spine3;		// 3
	BoneData_data_t spine4;		// 4
	BoneData_data_t neck;		// 5
	BoneData_data_t head;		// 6

	BoneData_data_t l_shoulder; // 8
	BoneData_data_t l_arm;		// 9
	BoneData_data_t l_wrist;	// 10
	BoneData_data_t l_hand;		// 11

	BoneData_data_t r_shoulder;	// 13
	BoneData_data_t r_arm;		// 14
	BoneData_data_t r_wrist;		// 15
	BoneData_data_t r_hand;		// 16

	BoneData_data_t l_hip;		// 22
	BoneData_data_t l_knee;		// 23
	BoneData_data_t l_foot;		// 24

	BoneData_data_t r_hip;		// 25
	BoneData_data_t r_knee;		// 26
	BoneData_data_t r_foot;		// 27

	BoneData_data_t unknown1;	// 7
	BoneData_data_t unknown2;	// 12
	BoneData_data_t unknown3;		// 17
	BoneData_data_t unknown4;		// 18
	BoneData_data_t unknown5;		// 19
	BoneData_data_t unknown6;		// 20
	BoneData_data_t unknown7;		// 21
	BoneData_data_t unknown8;		// 28
	BoneData_data_t unknown9;		// 29
public:

	static BoneData fromVector(const CBoneArray raw_data, const Matrix4x4 view_matrix) {
		assert(raw_data.size() == 30);

		return {
			.hip = worldToScreen(raw_data[0].location, view_matrix).xy(),
			.spine1 = worldToScreen(raw_data[1].location, view_matrix).xy(),
			.spine2 = worldToScreen(raw_data[2].location, view_matrix).xy(),
			.spine3 = worldToScreen(raw_data[3].location, view_matrix).xy(),
			.spine4 = worldToScreen(raw_data[4].location, view_matrix).xy(),
			.neck = worldToScreen(raw_data[5].location, view_matrix).xy(),
			.head = worldToScreen(raw_data[6].location, view_matrix).xy(),
			.l_shoulder = worldToScreen(raw_data[8].location, view_matrix).xy(),
			.l_arm = worldToScreen(raw_data[9].location, view_matrix).xy(),
			.l_wrist = worldToScreen(raw_data[10].location, view_matrix).xy(),
			.l_hand = worldToScreen(raw_data[11].location, view_matrix).xy(),
			.r_shoulder = worldToScreen(raw_data[13].location, view_matrix).xy(),
			.r_arm = worldToScreen(raw_data[14].location, view_matrix).xy(),
			.r_wrist = worldToScreen(raw_data[15].location, view_matrix).xy(),
			.r_hand = worldToScreen(raw_data[16].location, view_matrix).xy(),
			.l_hip = worldToScreen(raw_data[22].location, view_matrix).xy(),
			.l_knee = worldToScreen(raw_data[23].location, view_matrix).xy(),
			.l_foot = worldToScreen(raw_data[24].location, view_matrix).xy(),
			.r_hip = worldToScreen(raw_data[25].location, view_matrix).xy(),
			.r_knee = worldToScreen(raw_data[26].location, view_matrix).xy(),
			.r_foot = worldToScreen(raw_data[27].location, view_matrix).xy(),
			.unknown1 = worldToScreen(raw_data[7].location, view_matrix).xy(),
			.unknown2 = worldToScreen(raw_data[12].location, view_matrix).xy(),
			.unknown3 = worldToScreen(raw_data[17].location, view_matrix).xy(),
			.unknown4 = worldToScreen(raw_data[18].location, view_matrix).xy(),
			.unknown5 = worldToScreen(raw_data[19].location, view_matrix).xy(),
			.unknown6 = worldToScreen(raw_data[20].location, view_matrix).xy(),
			.unknown7 = worldToScreen(raw_data[21].location, view_matrix).xy(),
			.unknown8 = worldToScreen(raw_data[28].location, view_matrix).xy(),
			.unknown9 = worldToScreen(raw_data[29].location, view_matrix).xy()
		};
	}
};
