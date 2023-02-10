#include "tgaimage.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <cmath>
#include "geometry.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

using namespace std;
vector<vector<float>> v,vt,vn, f;
vector<float> camera = {0,0,3};

Matrix mTempo(vector<float> v){
    Matrix identite = Matrix::identity(4);
    Matrix mTempo;
    mTempo[0][0] = v[0];
    mTempo[1][0] = v[1];
    mTempo[2][0] = v[2];
    mTempo[3][0] = 1.0;
    identite[3][2] = static_cast<float>(-1.0/camera[2]);
    mTempo = identite*mTempo;
    return mTempo;
}

vector<float> resize(const int width, const int height, Matrix m)
{
    int w = width / 2;
    int h = height / 2;
    float vx = (m[0][0] * w*0.9) + w;
    float vy = (m[1][0] * h*0.9) + h;
    float vz = m[2][0];
    vector<float> v = {vx,vy,vz};
    return v;
}

Matrix m(Matrix mTempo){
    Matrix m;
    m[0][0]= mTempo[0][0]/mTempo[3][0];
    m[1][0]= mTempo[1][0]/mTempo[3][0];
    m[2][0]= mTempo[2][0]/mTempo[3][0];
    return m;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror3 = 2 * std::abs(dy);
    int error3 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error3 += derror3;
        if (error3 > dx)
        {
            y += dy > 0 ? 1 : -1;
            error3 -= 2 * dx;
        }
    }
}

void parserfile(TGAImage framebuffer)
{
    std::ifstream myfile;
    myfile.open("main.obj");
    std::string myline;
    if (myfile.is_open())
    {
        while (myfile)
        { // equiv[a]ent to myfile.good()
            std::getline(myfile, myline);
            std::string delimiter = " ";
            std::string delimiter2 = "/";
            size_t pos = 0;
            std::string token;
            std::vector<float> temp;
            if (myline.substr(0, 2) == "v ")
            {
                myline = myline.substr(2);
                for (int i = 0; i <= 2; i++)
                {
                    pos = myline.find(delimiter);
                    token = myline.substr(0, pos);
                    myline.erase(0, pos + delimiter.length());
                    temp.push_back(std::stof(token));
                    // std::cout << temp.at(i) << std::endl;
                }
                v.push_back(temp);
            }

            if (myline.substr(0, 4) == "vt  ")
            {
                myline = myline.substr(4);
                for (int i = 0; i <= 1; i++)
                {
                    pos = myline.find(delimiter);
                    token = myline.substr(0, pos);
                    myline.erase(0, pos + delimiter.length());
                    temp.push_back(std::stof(token));
                    //std::cout << temp.at(i) << std::endl;
                }
                vt.push_back(temp);
            }

            if (myline.substr(0, 4) == "vn  ")
            {
                myline = myline.substr(4);
                for (int i = 0; i <= 1; i++)
                {
                    pos = myline.find(delimiter);
                    token = myline.substr(0, pos);
                    myline.erase(0, pos + delimiter.length());
                    temp.push_back(std::stof(token));
                    //std::cout << temp.at(i) << std::endl;
                }
                vn.push_back(temp);
            }

            if (myline.substr(0, 2) == "f ")
            {
                std::string tempoLine = myline;
                for (int i = 0; i <= 2; i++)
                {
                    // Séparer de la ligne f en fonction des espaces
                    pos = tempoLine.find(delimiter);
                    token = tempoLine.substr(0, pos);
                    tempoLine.erase(0, pos + delimiter.length());
                    // Séparer de la ligne f en fonction des / et prise en compte uniquement de la première valeur
                    pos = tempoLine.find(delimiter2);
                    token = tempoLine.substr(0, pos);
                    temp.push_back(std::stof(token)); //Insère la valeur dans f
                    // Prise en compte de chaque 2e valeur dans f
                    tempoLine = tempoLine.substr(pos+1);
                    pos = tempoLine.find(delimiter2);
                    token = tempoLine.substr(0, pos);
                    temp.push_back(std::stof(token)); //Insère la valeur dans f
                    // Prise en compte de chaque 3e valeur dans f
                    tempoLine = tempoLine.substr(pos+1);
                    pos = tempoLine.find(delimiter2);
                    token = tempoLine.substr(0, pos);
                    temp.push_back(std::stof(token)); //Insère la valeur dans f
                }
                f.push_back(temp);
            }
        }
    }
}

// Fonction dessinant les segments du triangle
void triangle(const int width, const int height,TGAImage &framebuffer, TGAImage &texture)
{
    float zbuffer[width * height];
    for (int i = 0; i < width * height; i++)
    {
        zbuffer[i] = -1000;
    }

    const TGAColor white = {255, 255, 255, 255};
    const TGAColor red = {0, 0, 255, 255};
    const TGAColor blue = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    for (int h = 0; h <= f.size() - 1; h++)
    {
        int widthTexture = texture.width();
        int heigthTexture = texture.height();
        const TGAColor alea = {std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), 255};
        int a = f[h][0] - 1;
        int b = f[h][3] - 1;
        int c = f[h][6] - 1;        
        int at = f[h][1] - 1;
        int bt = f[h][4] - 1;
        int ct = f[h][7] - 1;

        vector<float> vmTempomA = {v[a][0], v[a][1], v[a][2]};
        vector<float> vmTempomB = {v[b][0], v[b][1], v[b][2]};
        vector<float> vmTempomC = {v[c][0], v[c][1], v[c][2]};
        Matrix mTempoA = mTempo(vmTempomA);
        Matrix mTempoB = mTempo(vmTempomB);
        Matrix mTempoC = mTempo(vmTempomC);
        Matrix mA = m(mTempoA);
        Matrix mB = m(mTempoB);
        Matrix mC = m(mTempoC);
        vector<float> vmA = resize(width,height,mA);
        vector<float> vmB = resize(width,height,mB);
        vector<float> vmC = resize(width,height,mC);    

        //Vecteur pour les textures

        vector<float> vTextureA = {vt[at][0], vt[at][1]};
        vector<float> vTextureB = {vt[bt][0], vt[bt][1]};
        vector<float> vTextureC = {vt[ct][0], vt[ct][1]};

        // Co du centre de gravité du triangle
        
        vector<float> N = {(vmB[1] - vmA[1]) * (vmC[2] - vmA[2]) - (vmB[2] - vmA[2]) * (vmC[1] - vmA[1]),
                           -(vmB[0] - vmA[0]) * (vmC[2] - vmA[2]) + (vmB[2] - vmA[2]) * (vmC[0] - vmA[0]),
                           (vmB[0] - vmA[0]) * (vmC[1] - vmA[1]) - (vmB[1] - vmA[1]) * (vmC[0] - vmA[0])};
        
        // Vecteurs pour la lumière

        vector<float> lum = {0, 0, -1};

        // Produit scalaire
        float normeN = sqrtf(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
        float normeLim = sqrtf(lum[0] * lum[0] + lum[1] * lum[1] + lum[2] * lum[2]);
        float scalaireNLim = N[0] * lum[0] + N[1] * lum[1] + N[2] * lum[2];
        const std::uint8_t intensite = -(scalaireNLim / normeN * normeLim) * 255;
        const TGAColor back = {intensite, intensite, intensite, 255};
        int minX = std::min({vmA[0], vmB[0], vmC[0]});
        int maxX = std::max({vmA[0], vmB[0], vmC[0]});
        int minY = std::min({vmA[1], vmB[1], vmC[1]});
        int maxY = std::max({vmA[1], vmB[1], vmC[1]});
        for (int i = minX; i <= maxX; i++)
        {
            for (int j = minY; j <= maxY; j++)
            {
                if(minX <0 || maxX > width || minY <0 || maxY > height ) continue;
                // Point P correspond à i et j
                float alpha = (i - vmB[0]) * (j - vmC[1]) - (j - vmB[1]) * (i - vmC[0]);
                float beta = (i - vmC[0]) * (j - vmA[1]) - (j - vmC[1]) * (i - vmA[0]);
                float gamma = (i - vmA[0]) * (j - vmB[1]) - (j - vmA[1]) * (i - vmB[0]);
                float tot = alpha + beta + gamma;
                alpha /= tot;
                beta /= tot;
                gamma /= tot;
                widthTexture = (vTextureA[0]*alpha + vTextureB[0]*beta + vTextureC[0]*gamma)*texture.width();
                heigthTexture = (1- (vTextureA[1]*alpha + vTextureB[1]*beta + vTextureC[1]*gamma))*texture.height();
                TGAColor color = texture.get(widthTexture,heigthTexture);
                if (scalaireNLim < 0)
                {
                    if (alpha > -0.01 && beta > -0.01 && gamma > -0.01)
                    {
                        float k = 0;
                        k = (vmA[2]) * alpha + (vmB[2]) * beta + (vmC[2]) * gamma;
                        if (zbuffer[int(i + j * width)] < k)
                        {
                            zbuffer[int(i + j * width)] = k;
                            framebuffer.set(i, j, color);
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    constexpr int width = 1024;
    constexpr int height = 1024;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage texture(width, height, TGAImage::RGB);
    texture.read_tga_file("texture.tga");
    parserfile(framebuffer);
    triangle(width, height, framebuffer, texture);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}