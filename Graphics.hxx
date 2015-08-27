#ifndef GRAPHICS_HXX_INCLUDED
#define GRAPHICS_HXX_INCLUDED


#include <windows.h>
#include <d3d9.h>
#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <fstream>
#include "Libraries/LodePNG.hxx"

#include <d3dx9.h>



#define VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define VERTEX_FVF_TEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef union BGRA
{
    struct {std::uint8_t B, G, R, A;};
    std::uint32_t Colour;
} *PBGRA;

typedef union RGBA
{
    struct {std::uint8_t R, G, B, A;};
    std::uint32_t Colour;
} *PRGBA;

typedef union ARGB
{
    struct{std::uint8_t A, R, G, B;};
    std::uint32_t Colour;
} *PARGB;

typedef union ABGR
{
    struct {std::uint8_t A, B, G, R;};
    std::uint32_t Colour;
} *PABGR;

typedef struct
{
    float X, Y, Z, RHW;
    unsigned int Colour;
    float U, V;
} D3DVertex;




class Font
{
private:
    HFONT hFont;

public:
    Font(int height, int weight, bool italic, bool underline, const wchar_t* lpszFace);
    ~Font();

    operator HFONT();
    operator WPARAM();
};


class Image
{
private:
    void* pixels;
    unsigned int width, height;
    void process_pixels(void* out, void* in);
    void unprocess_pixels(void* out, void* in);

public:
    Image() : pixels(nullptr), width(0), height(0) {}
    Image(const char* Path);
    Image(void* pixels, unsigned int width, unsigned int height);
    ~Image() {free(pixels); pixels = nullptr;}

    Image(const Image &other) = delete;
    Image(Image &&other) = delete;
    Image& operator = (const Image &other) = delete;

    unsigned int GetWidth() const {return width;}
    unsigned int GetHeight() const {return height;}
    void* GetPixels() const {return pixels;}

    bool Save(const char* path);
};

class Texture
{
private:
    IDirect3DTexture9* tex;
    IDirect3DDevice9* pDevice;

public:
    Texture(IDirect3DDevice9* pDevice, const char* path, const D3DCOLOR colourkey = 0xFF000000);
    Texture(IDirect3DDevice9* pDevice, int width, int height, const D3DFORMAT format = D3DFMT_A8R8G8B8);
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) : tex(other.tex), pDevice(other.pDevice) {other.tex = nullptr; other.pDevice = nullptr;}
    ~Texture() {if(tex) tex->Release();}

    inline operator IDirect3DTexture9*() const {return tex;}
    std::uint8_t* Lock();
    void Unlock();
    void GetDescription(D3DSURFACE_DESC* desc);

    void Draw(float X1, float Y1, float X2, float Y2);
};

#endif // GRAPHICS_HXX_INCLUDED
