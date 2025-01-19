#pragma once
#include <vector>
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

#define ROW_MAJOR

struct Matrix4x4 {
	const float matrix[4][4];

	const float* operator[](int index) const
	{
		return matrix[index];
	}
};

struct Vector4 {
	union {
		struct {
			float x, y, z, w;
		};
		float values[4];
	};

	Vector4(float x = 0, float y = 0, float z = 0, float w = 0): x(x), y(y), z(z), w(w) {}

	const float& operator[] (int index) const {
		return values[index];
	}
};

struct Vector3 {
	union {
		struct {
			float x, y, z;
		};
		float values[3];
	};

	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

	const float& operator[] (int index) const {
		return values[index];
	}
};

struct Vector2 {
	union {
		struct {
			float x, y;
		};
		float values[2];
	};

	Vector2(float x = 0, float y = 0) : x(x), y(y) {}

	const float& operator[] (int index) const {
		return values[index];
	}
};


Vector4 operator*(const Matrix4x4& m, const Vector4& v)
{
#ifdef ROW_MAJOR
	return Vector4(
		m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
		m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
		m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
		m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]
	);
#else
	return Vector4(
		m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
		m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
		m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
		m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
	);
#endif
}

Vector3 worldToScreen(const Vector3& world_pos, const Matrix4x4& view_matrix) {
	auto clip_coords = view_matrix * Vector4(world_pos.x, world_pos.y, world_pos.z, 1);

	if (clip_coords.w < 0.01f)
		return { 0.f, 0.f, 0.f };

	auto ndc_coords = Vector3(
		clip_coords.x / clip_coords.w,
		clip_coords.y / clip_coords.w,
		clip_coords.z / clip_coords.w
	);

	auto pixel_x = (ndc_coords.x + 1) / 2 * screenWidth + 0.5f;
	auto pixel_y = screenHeight - ((ndc_coords.y + 1) / 2 * screenHeight) + 0.5f;

	return { pixel_x, pixel_y, 1.f };
}

typedef Matrix4x4 view_matrix_t;
