#pragma once

#include <cassert>

#include "vector_math.h"
#include "cs_defines.h"

typedef Vector2 BoneData_data_t;

struct BoneData
{
    BoneData_data_t hip;    // 0
    BoneData_data_t spine1; // 1
    BoneData_data_t spine2; // 2
    BoneData_data_t spine3; // 3
    BoneData_data_t spine4; // 4
    BoneData_data_t neck;   // 5
    BoneData_data_t head;   // 6

    BoneData_data_t l_clavicle; // 7
    BoneData_data_t l_shoulder; // 8
    BoneData_data_t l_arm;      // 9
    BoneData_data_t l_wrist;    // 10
    BoneData_data_t l_hand;     // 11

    BoneData_data_t r_clavicle; // 12
    BoneData_data_t r_shoulder; // 13
    BoneData_data_t r_arm;      // 14
    BoneData_data_t r_wrist;    // 15
    BoneData_data_t r_hand;     // 16

    BoneData_data_t jiggle_primary;        // 17
    BoneData_data_t chesthier_offset;      // 18
    BoneData_data_t weaponhier_jnt;        // 19
    BoneData_data_t weaponhier_r_iktarget; // 20
    BoneData_data_t weaponhier_l_iktarget; // 21

    BoneData_data_t l_hip;  // 22
    BoneData_data_t l_knee; // 23
    BoneData_data_t l_foot; // 24

    BoneData_data_t r_hip;  // 25
    BoneData_data_t r_knee; // 26
    BoneData_data_t r_foot; // 27

    BoneData_data_t root_motion; // 28
public:
    static BoneData fromVector(const CBoneArray raw_data, const Matrix4x4 view_matrix)
    {
        return {
            .hip = worldToScreen(raw_data.pelvis.location, view_matrix).xy(),
            .spine1 = worldToScreen(raw_data.spine_1.location, view_matrix).xy(),
            .spine2 = worldToScreen(raw_data.spine_2.location, view_matrix).xy(),
            .spine3 = worldToScreen(raw_data.spine_3.location, view_matrix).xy(),
            .spine4 = worldToScreen(raw_data.spine_4.location, view_matrix).xy(),
            .neck = worldToScreen(raw_data.neck.location, view_matrix).xy(),
            .head = worldToScreen(raw_data.head.location, view_matrix).xy(),

            .l_clavicle = worldToScreen(raw_data.l_clavicle.location, view_matrix).xy(),
            .l_shoulder = worldToScreen(raw_data.l_shoulder.location, view_matrix).xy(),
            .l_arm = worldToScreen(raw_data.l_arm.location, view_matrix).xy(),
            .l_wrist = worldToScreen(raw_data.l_wrist.location, view_matrix).xy(),
            .l_hand = worldToScreen(raw_data.l_hand.location, view_matrix).xy(),

            .r_clavicle = worldToScreen(raw_data.r_clavicle.location, view_matrix).xy(),
            .r_shoulder = worldToScreen(raw_data.r_shoulder.location, view_matrix).xy(),
            .r_arm = worldToScreen(raw_data.r_arm.location, view_matrix).xy(),
            .r_wrist = worldToScreen(raw_data.r_wrist.location, view_matrix).xy(),
            .r_hand = worldToScreen(raw_data.r_hand.location, view_matrix).xy(),

            .jiggle_primary = worldToScreen(raw_data.jiggle_primary.location, view_matrix).xy(),
            .chesthier_offset = worldToScreen(raw_data.chesthier_offset.location, view_matrix).xy(),
            .weaponhier_jnt = worldToScreen(raw_data.weaponhier_jnt.location, view_matrix).xy(),
            .weaponhier_r_iktarget = worldToScreen(raw_data.weaponhier_r_iktarget.location, view_matrix).xy(),
            .weaponhier_l_iktarget = worldToScreen(raw_data.weaponhier_l_iktarget.location, view_matrix).xy(),

            .l_hip = worldToScreen(raw_data.l_hip.location, view_matrix).xy(),
            .l_knee = worldToScreen(raw_data.l_knee.location, view_matrix).xy(),
            .l_foot = worldToScreen(raw_data.l_foot.location, view_matrix).xy(),

            .r_hip = worldToScreen(raw_data.r_hip.location, view_matrix).xy(),
            .r_knee = worldToScreen(raw_data.r_knee.location, view_matrix).xy(),
            .r_foot = worldToScreen(raw_data.r_foot.location, view_matrix).xy(),

            .root_motion = worldToScreen(raw_data.root_motion.location, view_matrix).xy(),
        };
    }
};
