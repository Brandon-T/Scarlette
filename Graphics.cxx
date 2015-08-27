#include "Graphics.hxx"

Font::Font(int height, int weight, bool italic, bool underline, const wchar_t* lpszFace)
{
    hFont = CreateFontW(height, 0, 0, 0, weight, italic, underline, false, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, lpszFace);
}

Font::~Font()
{
    DeleteObject(hFont);
}

Font::operator HFONT()
{
    return hFont;
}

Font::operator WPARAM()
{
    return reinterpret_cast<WPARAM>(hFont);
}



void Image::process_pixels(void* out, void* in)
{
    unsigned int i, j;
    BGRA* pOut = (BGRA*)out;
    unsigned char* pIn = (unsigned char*)in;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            pOut[(height - 1 - i) * width + j].B = *(pIn++);
            pOut[(height - 1 - i) * width + j].G = *(pIn++);
            pOut[(height - 1 - i) * width + j].R = *(pIn++);
            pOut[(height - 1 - i) * width + j].A = *(pIn++);
        }
    }
}

void Image::unprocess_pixels(void* out, void* in)
{
    unsigned int i, j;
    unsigned char* pOut = (unsigned char*)out;
    BGRA* pIn = (BGRA*)in;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            *(pOut++) = pIn[(height - 1 - i) * width + j].B;
            *(pOut++) = pIn[(height - 1 - i) * width + j].G;
            *(pOut++) = pIn[(height - 1 - i) * width + j].R;
            *(pOut++) = pIn[(height - 1 - i) * width + j].A;
        }
    }
}

Image::Image(const char* Path)
{
    unsigned char Header[30] = {0};
    std::ifstream hFile(Path, std::ios::in | std::ios::binary);
    hFile.read(reinterpret_cast<char*>(&Header), sizeof(Header));

    if(Header[1] == 0x50 && Header[2] == 0x4E && Header[3] == 0x47)
    {
        hFile.close();
        std::vector<std::uint8_t> pixels;
        lodepng::decode(pixels, width, height, Path);
        this->pixels = malloc(pixels.size());
        memcpy(this->pixels, &pixels[0], pixels.size());
    }
    else if(Header[0] == 0x42 && Header[1] == 0x4D)
    {
        if(Header[28] == 32)
        {
            width = Header[18] + (Header[19] << 8);
            height = Header[22] + (Header[23] << 8);
            unsigned int PixelsOffset = Header[10] + (Header[11] << 8);
            unsigned int size = ((width * 32 + 31) / 32) * 4 * height;
            pixels = malloc(size);

            hFile.seekg(PixelsOffset, std::ios::beg);
            hFile.read(reinterpret_cast<char*>(pixels), size);
            hFile.close();
        }
    }
}

Image::Image(void* pixels, unsigned int width, unsigned int height) : pixels(width > 0 && height > 0 ? malloc(((width * 32 + 31) / 32) * 4 * height) : nullptr), width(width > 0 ? width : 0), height(height > 0 ? height : 0)
{
    if (width > 0 && height > 0 && pixels)
    {
        process_pixels(this->pixels, pixels);
    }
}

bool Image::Save(const char* path)
{
    FILE* file = fopen(path, "wb");

    if (file)
    {
        unsigned short bpp = 32;
        unsigned int trash = 0;
        unsigned short planes = 1;
        unsigned int biSize = 108;
        unsigned int offset = 122;
        unsigned int compression = 3;
        unsigned short type = 0x4D42;
        unsigned int size = ((width * bpp + 31) / 32) * 4 * height;
        unsigned int bfSize = offset + size;
        unsigned int masks[4] = {0xFF0000, 0xFF00, 0xFF, 0xFF000000};
        unsigned int csType = 0x73524742;
        unsigned int epts[9] = {0};

        fwrite(&type, sizeof(type), 1, file);
        fwrite(&bfSize, sizeof(bfSize), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&offset, sizeof(offset), 1, file);
        fwrite(&biSize, sizeof(biSize), 1, file);
        fwrite(&width, sizeof(width), 1, file);
        fwrite(&height, sizeof(height), 1, file);
        fwrite(&planes, sizeof(planes), 1, file);
        fwrite(&bpp, sizeof(bpp), 1, file);
        fwrite(&compression, sizeof(compression), 1, file);
        fwrite(&size, sizeof(size), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&trash, sizeof(trash), 1, file);
        fwrite(&masks, sizeof(masks), 1, file);
        fwrite(&csType, sizeof(csType), 1, file);
        fwrite(&epts, sizeof(epts), 1, file);
        fwrite(&epts, sizeof(trash), 1, file);
        fwrite(&epts, sizeof(trash), 1, file);
        fwrite(&epts, sizeof(trash), 1, file);

        void* buffer = malloc(size);
        if (!buffer)
        {
            perror("Memory allocation failure.");
            return false;
        }

        unprocess_pixels(buffer, pixels);
        fwrite(pixels, 1, size, file);
        free(buffer);
        return true;
    }
    return false;
}

Texture::Texture(IDirect3DDevice9* pDevice, const char* path, const D3DCOLOR colourkey) : tex(nullptr), pDevice(pDevice)
{
    HRESULT res = D3DXCreateTextureFromFileEx(pDevice, path, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, colourkey, nullptr, nullptr, &tex);
}

Texture::Texture(IDirect3DDevice9* pDevice, int width, int height, const D3DFORMAT format) : tex(nullptr), pDevice(pDevice)
{
    pDevice->CreateTexture(width, height, 1, 0, format, D3DPOOL_MANAGED, &tex, 0);
}

std::uint8_t* Texture::Lock()
{
    D3DLOCKED_RECT rect;
    tex->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);
    return static_cast<std::uint8_t*>(rect.pBits);
}

void Texture::Unlock()
{
    tex->UnlockRect(0);
}

void Texture::GetDescription(D3DSURFACE_DESC* desc)
{
    tex->GetLevelDesc(0, desc);
}

void Texture::Draw(float X1, float Y1, float X2, float Y2)
{
    float UOffset = 0.5f / static_cast<float>(X2 - X1);
    float VOffset = 0.5f / static_cast<float>(Y2 - Y1);

    D3DVertex Vertices[] =
    {
        {X1, Y1, 1.0f, 1.0f, D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF), 0.0f + UOffset, 0.0f + VOffset},
        {X2, Y1, 1.0f, 1.0f, D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF), 1.0f + UOffset, 0.0f + VOffset},
        {X1, Y2, 1.0f, 1.0f, D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF), 0.0f + UOffset, 1.0f + VOffset},
        {X2, Y2, 1.0f, 1.0f, D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF), 1.0f + UOffset, 1.0f + VOffset}
    };

    pDevice->SetFVF(VERTEX_FVF_TEX);
    pDevice->SetTexture(0, tex);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, sizeof(D3DVertex));
    pDevice->SetTexture(0, nullptr);
}
