
struct Vector4
{
    float x, y, z, w;
    Vector4(): x(0), y(0), z(0), w(0){};
    Vector4 (float _x, float _y, float _z, float _w): x(_x), y(_y), z(_z), w(_w){}

    Vector4& operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;
        return *this;
    }
};

struct Vector3
{
    float x, y, z;
    Vector3(): x(0), y(0), z(0){};
    Vector3 (float _x, float _y, float _z): x(_x), y(_y), z(_z){}

    Vector3& operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        return *this;
    }
};

struct Vector2
{
    float x, y;
    Vector2(): x(0), y(0) {};
    Vector2(float _a, float _b): x(_a), y(_b) {};
};