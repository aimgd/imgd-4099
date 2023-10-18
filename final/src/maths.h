#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#include <math.h>
constexpr f32 PI = 3.14159265358979;

struct Vec2 {
    f32 x;
    f32 y;
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Vec4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

Vec2 v2(f32 x, f32 y);
Vec3 v3(f32 x, f32 y, f32 z);
Vec4 v4(f32 x, f32 y, f32 z, f32 w);

Vec3 v3(Vec2 xy, f32 z);
Vec3 v3(f32 x, Vec2 yz);

Vec4 v4(Vec2 xy, f32 z, f32 w);
Vec4 v4(f32 x, f32 y, Vec2 zw);
Vec4 v4(f32 x, Vec2 yz, f32 w);
Vec4 v4(Vec2 xy, Vec2 zw);
Vec4 v4(Vec3 xyz, f32 w);
Vec4 v4(f32 x, Vec3 yzw);

Vec2 v2(f32 x);
Vec3 v3(f32 x);
Vec4 v4(f32 x);

// Downscale conversions
// 2
Vec2 v2(Vec3 xy);
Vec2 v2(Vec4 xy);
//3
Vec3 v3(Vec4 xyz);

// Arithmetic
Vec2 add(Vec2 a, Vec2 b);
Vec3 add(Vec3 a, Vec3 b);
Vec4 add(Vec4 a, Vec4 b);

Vec2 operator+(Vec2 a, Vec2 b);
Vec3 operator+(Vec3 a, Vec3 b);
Vec4 operator+(Vec4 a, Vec4 b);

inline Vec2 operator+=(Vec2& a, Vec2 b) { a = a + b; return a; }
inline Vec3 operator+=(Vec3& a, Vec3 b) { a = a + b; return a; }
inline Vec4 operator+=(Vec4& a, Vec4 b) { a = a + b; return a; }


Vec2 sub(Vec2 a, Vec2 b);
Vec3 sub(Vec3 a, Vec3 b);
Vec4 sub(Vec4 a, Vec4 b);

Vec2 operator-(Vec2 a, Vec2 b);
Vec3 operator-(Vec3 a, Vec3 b);
Vec4 operator-(Vec4 a, Vec4 b);

inline Vec2 operator-=(Vec2& a, Vec2 b) { a = a - b; return a; }
inline Vec3 operator-=(Vec3& a, Vec3 b) { a = a - b; return a; }
inline Vec4 operator-=(Vec4& a, Vec4 b) { a = a - b; return a; }


Vec2 mul(Vec2 a, Vec2 b);
Vec3 mul(Vec3 a, Vec3 b);
Vec4 mul(Vec4 a, Vec4 b);

Vec2 operator*(Vec2 a, Vec2 b);
Vec3 operator*(Vec3 a, Vec3 b);
Vec4 operator*(Vec4 a, Vec4 b);

inline Vec2 operator*=(Vec2& a, Vec2 b) { a = a * b; return a; }
inline Vec3 operator*=(Vec3& a, Vec3 b) { a = a * b; return a; }
inline Vec4 operator*=(Vec4& a, Vec4 b) { a = a * b; return a; }


Vec2 div(Vec2 a, Vec2 b);
Vec3 div(Vec3 a, Vec3 b);
Vec4 div(Vec4 a, Vec4 b);

Vec2 operator/(Vec2 a, Vec2 b);
Vec3 operator/(Vec3 a, Vec3 b);
Vec4 operator/(Vec4 a, Vec4 b);

inline Vec2 operator/=(Vec2& a, Vec2 b) { a = a / b; return a; }
inline Vec3 operator/=(Vec3& a, Vec3 b) { a = a / b; return a; }
inline Vec4 operator/=(Vec4& a, Vec4 b) { a = a / b; return a; }

Vec2 operator-(Vec2 a) { return { -a.x, -a.y }; }
Vec3 operator-(Vec3 a) { return { -a.x, -a.y, -a.z }; }
Vec4 operator-(Vec4 a) { return { -a.x, -a.y, -a.z, -a.w }; }


f32 dot(Vec2 a, Vec2 b);
f32 dot(Vec3 a, Vec3 b);
f32 dot(Vec4 a, Vec4 b);

Vec3 cross(Vec3 lhs, Vec3 rhs);

f32 length(Vec2 a);
f32 length(Vec3 a);
f32 length(Vec4 a);

Vec2 normalize(Vec2 a);
Vec3 normalize(Vec3 a);
Vec4 normalize(Vec4 a);

bool equal(Vec2 a, Vec2 b);
bool equal(Vec3 a, Vec3 b);
bool equal(Vec4 a, Vec4 b);

////

struct Mat4 {
    union { 
        f32 m[4 * 4];
        Vec4 rows[4];
    };
};

Mat4 m4(f32 trace);

Mat4 perspMat(f32 fov, f32 aspect, f32 start, f32 end);
Mat4 orthoMat(f32 left, f32 right, f32 bottom, f32 top, f32 start, f32 end);

Mat4 lookAt(Vec3 dir, Vec3 up);

Mat4 transMat(Vec3 translation);
Mat4 axisAngleMat(Vec3 axis, f32 angle);
Mat4 scaleMat(Vec3 scale);

Mat4 mul(Mat4 a, Mat4 b);
Mat4 transpose(Mat4 mat);
Mat4 inverse(Mat4 mat);

Vec4 mul(Vec4 v, Mat4 mat);
Vec3 mul(Vec3 v, f32 w, Mat4 mat);

Vec3 project(Vec3 v, Mat4 projectionMatrix);
Vec3 unproject(Vec3 ndc, Mat4 projectionMatrix);

Vec3 forward(Mat4 mat);
Vec3 right(Mat4 mat);
Vec3 up(Mat4 mat);

i32 sign(f32 x)
{
    if (x < 0) return -1;
    if (x > 0) return  1;
    return 0;
}

f32 fract(f32 x)
{
    return x - floorf(x);
}

f32 Sin(f32 x) { return sinf(x * 2 * PI); }
f32 Cos(f32 x) { return cosf(x * 2 * PI); }
f32 Tan(f32 x) { return tanf(x * 2 * PI); }

template<typename T, typename R>
T lerp(T a, T b, R t)
{
	return a - a * t + b * t;
}

template<typename T>
bool isPowOfTwo(T x)
{
	return (((x) != 0) && (((x) & ((x) - 1)) == 0));
}

template<typename T>
T min(T a, T b)
{
	if (a < b) 	return a;
	else 		return b;
}

template<typename T>
T max(T a, T b)
{
	if (a > b) 	return a;
	else 		return b;
}

template<typename T>
T clamp(T val, T min_val, T max_val)
{
	return min(max(val, min_val), max_val);
}


// template<typename T>
// f32 angleBetween(T a, T b)
// {
// 	f32 ang = Clamp(dot(a, b), 0, 1);
// 	return acos(ang) / (2 * PI); // @Todo wrap this function
// }

// Impl
// @Todo inline these!

Vec2 v2(f32 x, f32 y) { return Vec2 { x, y }; }
Vec3 v3(f32 x, f32 y, f32 z) { return Vec3 { x, y, z }; }
Vec4 v4(f32 x, f32 y, f32 z, f32 w) { return Vec4 { x, y, z, w }; }

Vec2 v2(f32 x) { return Vec2 { x, x }; }
Vec3 v3(f32 x) { return Vec3 { x, x, x }; }
Vec4 v4(f32 x) { return Vec4 { x, x, x, x }; }


Vec3 v3(Vec2 xy, f32 z) { return Vec3{ xy.x, xy.y, z }; }
Vec3 v3(f32 x, Vec2 yz) { return Vec3{ x, yz.x, yz.y }; }

Vec4 v4(Vec2 xy, f32 z, f32 w) { return Vec4 { xy.x, xy.y, z, w }; }
Vec4 v4(f32 x, f32 y, Vec2 zw) { return Vec4 { x, y, zw.x, zw.y }; }
Vec4 v4(f32 x, Vec2 yz, f32 w) { return Vec4 { x, yz.x, yz.y, w }; }
Vec4 v4(Vec2 xy, Vec2 zw) { return Vec4 { xy.x, xy.y, zw.x, zw.y }; }
Vec4 v4(Vec3 xyz, f32 w) { return Vec4 { xyz.x, xyz.y, xyz.z, w  }; }
Vec4 v4(f32 x, Vec3 yzw) { return Vec4 { x, yzw.x, yzw.y, yzw.z }; }


Vec2 v2(Vec3 xy) { return Vec2 { xy.x, xy.y }; }
Vec2 v2(Vec4 xy) { return Vec2 { xy.x, xy.y }; };
Vec3 v3(Vec4 xyz) { return Vec3 { xyz.x, xyz.y, xyz.z }; };



Vec2 add(Vec2 a, Vec2 b) {
    return Vec2 { a.x + b.x, a.y + b.y };
}

Vec3 add(Vec3 a, Vec3 b) {
    return Vec3 { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec4 add(Vec4 a, Vec4 b) {
    return Vec4 { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vec2 operator+(Vec2 a, Vec2 b)
{
	return add(a, b);
}

Vec3 operator+(Vec3 a, Vec3 b)
{
	return add(a, b);
}

Vec4 operator+(Vec4 a, Vec4 b)
{
	return add(a, b);
}




Vec2 sub(Vec2 a, Vec2 b) {
    return Vec2 { a.x - b.x, a.y - b.y };
}

Vec3 sub(Vec3 a, Vec3 b) {
    return Vec3 { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vec4 sub(Vec4 a, Vec4 b) {
    return Vec4 { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}


Vec2 operator-(Vec2 a, Vec2 b)
{
	return sub(a, b);
}

Vec3 operator-(Vec3 a, Vec3 b)
{
	return sub(a, b);
}

Vec4 operator-(Vec4 a, Vec4 b)
{
	return sub(a, b);
}





Vec2 mul(Vec2 a, Vec2 b) {
    return Vec2 { a.x * b.x, a.y * b.y };
}

Vec3 mul(Vec3 a, Vec3 b) {
    return Vec3 { a.x * b.x, a.y * b.y, a.z * b.z };
}

Vec4 mul(Vec4 a, Vec4 b) {
    return Vec4 { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}



Vec2 operator*(Vec2 a, Vec2 b) 
{
	return mul(a, b);
}

Vec3 operator*(Vec3 a, Vec3 b) 
{
	return mul(a, b);
}

Vec4 operator*(Vec4 a, Vec4 b) 
{
	return mul(a, b);
}

// @Todo @Temp generalize
Vec4 operator*(Vec4 a, f32 b) 
{
	return mul(a, v4(b));
}



Vec2 div(Vec2 a, Vec2 b) {
    return Vec2 { a.x / b.x, a.y / b.y };
}

Vec3 div(Vec3 a, Vec3 b) {
    return Vec3 { a.x / b.x, a.y / b.y, a.z / b.z };
}

Vec4 div(Vec4 a, Vec4 b) {
    return Vec4 { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
}


Vec2 operator/(Vec2 a, Vec2 b)
{
	return div(a, b);
}

Vec3 operator/(Vec3 a, Vec3 b)
{
	return div(a, b);
}

Vec4 operator/(Vec4 a, Vec4 b)
{
	return div(a, b);
}



f32 dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

f32 dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}



Vec3 cross(Vec3 lhs, Vec3 rhs) { return v3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x); }


f32 length(Vec2 a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

f32 length(Vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32 length(Vec4 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

// TODO: optimize lol
Vec2 normalize(Vec2 a) {
    f32 len = length(a);
    if (len == 0) len = 1;
    return div(a, v2(len));
}

Vec3 normalize(Vec3 a) {
    f32 len = length(a);
    if (len == 0) len = 1;
    return div(a, v3(len));
}

Vec4 normalize(Vec4 a) {
    f32 len = length(a);
    if (len == 0) len = 1;
    return div(a, v4(len));
}



bool equal(Vec2 a, Vec2 b)
{
	return (a.x == b.x) && (a.y == b.y);
}

bool equal(Vec3 a, Vec3 b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool equal(Vec4 a, Vec4 b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}


Vec2 abs(Vec2 v) { return { fabsf(v.x), fabsf(v.y) }; }
Vec3 abs(Vec3 v) { return { fabsf(v.x), fabsf(v.y), fabsf(v.z) }; }
Vec4 abs(Vec4 v) { return { fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w) }; }

// Matrices




Mat4 m4(f32 trace) {
    Mat4 m;
    
    m.m[0 ] = trace;
    m.m[1 ] = 0;
    m.m[2 ] = 0;
    m.m[3 ] = 0;
    
    m.m[4 ] = 0;
    m.m[5 ] = trace;
    m.m[6 ] = 0;
    m.m[7 ] = 0;
    
    m.m[8 ] = 0;
    m.m[9 ] = 0;
    m.m[10] = trace;
    m.m[11] = 0;
    
    m.m[12] = 0;
    m.m[13] = 0;
    m.m[14] = 0;
    m.m[15] = trace;
    
    return m;
}


Mat4 m4(f32 m00, f32 m10, f32 m20, f32 m30,
                  f32 m01, f32 m11, f32 m21, f32 m31,
                  f32 m02, f32 m12, f32 m22, f32 m32,
                  f32 m03, f32 m13, f32 m23, f32 m33) {
    Mat4 mat;
    
    mat.m[0 + 0 * 4] = m00;
	mat.m[1 + 0 * 4] = m10;
	mat.m[2 + 0 * 4] = m20;
	mat.m[3 + 0 * 4] = m30;
    
	mat.m[0 + 1 * 4] = m01;
	mat.m[1 + 1 * 4] = m11;
	mat.m[2 + 1 * 4] = m21;
	mat.m[3 + 1 * 4] = m31;
    
	mat.m[0 + 2 * 4] = m02;
	mat.m[1 + 2 * 4] = m12;
	mat.m[2 + 2 * 4] = m22;
	mat.m[3 + 2 * 4] = m32;
    
	mat.m[0 + 3 * 4] = m03;
	mat.m[1 + 3 * 4] = m13;
	mat.m[2 + 3 * 4] = m23;
	mat.m[3 + 3 * 4] = m33;
    
    return mat;
}



Mat4 mul(Mat4 a, Mat4 b) {
    Mat4 product;
    
    for (int row = 0; row < 4; row++) {
        Vec4 rowVector = a.rows[row];
        for (int col = 0; col < 4; col++) {
            Vec4 colVector = v4(b.m[col], b.m[col + 4], b.m[col + 8], b.m[col + 12]);
            
            product.m[4 * row + col] = dot(rowVector, colVector);
        }
    }
    
    return product;
}



Mat4 perspMat(f32 fov, f32 aspect, f32 start, f32 end) 
{
	f32 tfov2 = Tan(fov / 2.0);
    return m4(1.0 / (aspect * tfov2), 0, 0, 0,
                        0, 1.0 / (tfov2), 0, 0,
                        0, 0, (end) / (end - start), -(end * start) / (end - start),
                        0, 0, 1, 0);
}

Mat4 orthoMat(f32 left, f32 right, f32 bottom, f32 top, f32 start, f32 end)
{
	return m4(2 / (right - left), 0, 0, -(right + left) / (right - left),
						0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
						0, 0, 1 / (end - start),  -start / (end - start),
						0, 0, 0, 1
						);
}


Mat4 lookAt(Vec3 dir, Vec3 up)
{
	Vec3 right = normalize(cross(up, dir));
	Vec3 actualUp = normalize(cross(dir, right));
	Vec3 dirNorm = normalize(dir);
    
	return m4(
                        right.x, actualUp.x, dirNorm.x, 0,
                        right.y, actualUp.y, dirNorm.y, 0,
                        right.z, actualUp.z, dirNorm.z, 0,
                        0,		 0, 		 0,			1
                        );
}

Mat4 transMat(Vec3 translation) {
    return m4(
                        1, 0, 0, translation.x,
                        0, 1, 0, translation.y,
                        0, 0, 1, translation.z,
                        0, 0, 0, 1);
}

Mat4 axisAngleMat(Vec3 axis, f32 angle)
{
    axis = normalize(axis);
    f32 s = Sin(angle);
    f32 c = Cos(angle);
    f32 oc = 1.0 - c;
    
    return m4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
					    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
					    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
					    0.0,                                0.0,                                0.0,                                1.0);
}

Mat4 scaleMat(Vec3 scale) {
    return m4(scale.x, 0, 0, 0,
                        0, scale.y, 0, 0,
                        0, 0, scale.z, 0,
                        0, 0, 0, 1);
}






Mat4 transpose(Mat4 mat) {
	Mat4 result;
	for (u8 col = 0; col < 4; col++) {
		for (u8 row = 0; row < 4; row++) {
			result.m[col + row * 4] = mat.m[row + col * 4];
		}
	}
	return result;
}

Mat4 inverse(Mat4 mat) {
	// NOTE:
	// we have to transpose the matrix because this function is based off
	// a stackoverflow result, and for some reason, everyone likes to use column major matrices
	// be we use row major, so we have to transpose it, which essentially makes our
	// matrix column major, because i am wayyy to lazy to go change the array indices to make the calculation row major
	// -- 2020-02-08
	Mat4 input = transpose(mat);
    
	f32* m = input.m;
	f32 inv[16], det;
	
	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];
    
	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];
    
	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];
    
	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];
    
	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];
    
	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];
    
	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];
    
	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];
    
	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];
    
	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];
    
	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];
    
	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];
    
	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];
    
	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];
    
	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];
    
	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];
    
	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    
	assert(det != 0); // Divide by zero
	det = 1.0 / det;
    
    Mat4 result = {};
	for (i32 i = 0; i < ARRAY_SIZE(result.m); i++)
		result.m[i] = inv[i] * det;
    
	// make our matrix back into row major
	return transpose(result);
}



Vec4 mul(Vec4 v, Mat4 mat)
{
	// [1, 0, 0, 0] [x]     [x']
	// [0, 1, 0, 0] [y]     [y']
	// [0, 0, 1, 0] [z]  =  [z']
	// [0, 0, 0, 1] [w]     [w']
    
	Vec4 res;
	res.x = dot(mat.rows[0], v);
	res.y = dot(mat.rows[1], v);
	res.z = dot(mat.rows[2], v);
	res.w = dot(mat.rows[3], v);
	return res;
}

Vec3 mul(Vec3 v, f32 w, Mat4 mat)
{
	Vec4 vec4 = v4(v, w);
	return v3(mul(vec4, mat));
}


Vec3 project(Vec3 v, Mat4 projectionMatrix)
{
	Vec4 proj = mul(v4(v, 1), projectionMatrix);
	proj = div(proj, v4(proj.w));
	proj.x = proj.x / 2.0 + 0.5;
	proj.y = proj.y / 2.0 + 0.5;
	return v3(proj);
}

Vec3 unproject(Vec3 ndc, Mat4 projectionMatrix)
{
	ndc.x = ndc.x * 2 - 1;
	ndc.y = ndc.y * 2 - 1;
	Vec4 mv = mul(v4(ndc, 1), inverse(projectionMatrix));
	mv = div(mv, v4(mv.w));
	return v3(mv);
}

Vec3 forward(Mat4 mat) {
	Vec3 col = v3(
                  mat.m[2 + 4 * 0],
                  mat.m[2 + 4 * 1],
                  mat.m[2 + 4 * 2]
                  );
	return col;
}

Vec3 up(Mat4 mat) {
	Vec3 col = v3(
                  mat.m[1 + 4 * 0],
                  mat.m[1 + 4 * 1],
                  mat.m[1 + 4 * 2]
                  );
	return col;
}

Vec3 right(Mat4 mat) {
	Vec3 col = v3(
                  mat.m[0 + 4 * 0],
                  mat.m[0 + 4 * 1],
                  mat.m[0 + 4 * 2]
                  );
	return col;
}

