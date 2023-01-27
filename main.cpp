#include "tgaimage.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

using namespace std;
vector<vector<float>> v,vt,vn, f;
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

void parserfile(const int width, const int height, TGAImage framebuffer)
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
void triangle(const int width, const int height, TGAImage &framebuffer, TGAImage &texture)
{
    float zbuffer[width * height];
    for (int i = 0; i < width * height; i++)
    {
        zbuffer[i] = -1000;
    }
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    for (int h = 0; h <= f.size() - 1; h++)
    {
        int widthTexture = texture.width();
        int heigthTexture = texture.height();
        const TGAColor alea = {std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), 255};
        int a = f[h][0] - 1;
        int b = f[h][3] - 1;
        int c = f[h][6] - 1;        
        int a2 = f[h][1] - 1;
        int b2 = f[h][4] - 1;
        int c2 = f[h][7] - 1;
        vector<int> vA = {static_cast<int>((v[a][0] + 1) * width / 2), static_cast<int>((v[a][1] + 1) * height / 2)}; // Coordonnées x et y du premier sommet du triangle
        vector<int> vB = {static_cast<int>((v[b][0] + 1) * width / 2), static_cast<int>((v[b][1] + 1) * height / 2)};
        vector<int> vC = {static_cast<int>((v[c][0] + 1) * width / 2), static_cast<int>((v[c][1] + 1) * height / 2)};
        //vector<int> vtA = {static_cast<int>((vt[a][0] + 1) * widthTexture / 2), static_cast<int>((vt[a][1] + 1) * heigthTexture / 2)};
        //vector<int> vtB = {static_cast<int>((vt[b][0] + 1) * widthTexture / 2), static_cast<int>((vt[b][1] + 1) * heigthTexture / 2)};
        //vector<int> vtC = {static_cast<int>((vt[c][0] + 1) * widthTexture / 2), static_cast<int>((vt[c][1] + 1) * heigthTexture / 2)};
        vector<float> vtA = {static_cast<float>(vt[a2][0]), static_cast<float>(vt[a2][1])};
        vector<float> vtB = {static_cast<float>(vt[b2][0]), static_cast<float>(vt[b2][1])};
        vector<float> vtC = {static_cast<float>(vt[c2][0]), static_cast<float>(vt[c2][1])};
        // Co du centre de gravité du triangle
        vector<int> vAB = {vA[0] - vB[0], vA[1] - vB[1]}, vBC = {vB[0] - vC[0], vB[1] - vC[1]}, vAC = {vA[0] - vC[0], vA[1] - vC[1]};
        // Vecteurs pour la lumière
        vector<float> N = {(v[b][1] - v[a][1]) * (v[c][2] - v[a][2]) - (v[b][2] - v[a][2]) * (v[c][1] - v[a][1]),
                           -(v[b][0] - v[a][0]) * (v[c][2] - v[a][2]) + (v[b][2] - v[a][2]) * (v[c][0] - v[a][0]),
                           (v[b][0] - v[a][0]) * (v[c][1] - v[a][1]) - (v[b][1] - v[a][1]) * (v[c][0] - v[a][0])};
        vector<float> lum = {0, 0, -1};

        // Produit scalaire
        float normeN = sqrtf(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
        float normeLim = sqrtf(lum[0] * lum[0] + lum[1] * lum[1] + lum[2] * lum[2]);
        float scalaireNLim = N[0] * lum[0] + N[1] * lum[1] + N[2] * lum[2];
        const std::uint8_t intensite = -(scalaireNLim / normeN * normeLim) * 255;
        const TGAColor back = {intensite, intensite, intensite, 255};
        int minX = std::min({vA[0], vB[0], vC[0]});
        int maxX = std::max({vA[0], vB[0], vC[0]});
        int minY = std::min({vA[1], vB[1], vC[1]});
        int maxY = std::max({vA[1], vB[1], vC[1]});
        for (int i = minX; i <= maxX; i++)
        {
            for (int j = minY; j <= maxY; j++)
            {
                // Point P correspond à i et j
                float alpha = (i - vB[0]) * (j - vC[1]) - (j - vB[1]) * (i - vC[0]);
                float beta = (i - vC[0]) * (j - vA[1]) - (j - vC[1]) * (i - vA[0]);
                float gamma = (i - vA[0]) * (j - vB[1]) - (j - vA[1]) * (i - vB[0]);
                float tot = alpha + beta + gamma;
                alpha /= tot;
                beta /= tot;
                gamma /= tot;
                widthTexture = (vtA[0]*alpha + vtB[0]*beta + vtC[0]*gamma)*texture.width();
                heigthTexture = (1- (vtA[1]*alpha + vtB[1]*beta + vtC[1]*gamma))*texture.height();
                TGAColor color = texture.get(widthTexture,heigthTexture);
                //std::cout << " vtA[0] "<< vtA[0] << " alpha "<< alpha << " vtB[0] "<< vtB[0] << " beta "<< beta <<  " vtC[0] "<< vtC[0] << " gamma "<< gamma << std::endl;
              //  std::cout << "w " << widthTexture << " h " << heigthTexture << std::endl;
                if (scalaireNLim < 0)
                {
                    if (alpha > -0.01 && beta > -0.01 && gamma > -0.01)
                    {
                        float k = 0;
                        k = (v[a][2]) * alpha + (v[b][2]) * beta + (v[c][2]) * gamma;
                        //std::cerr << k << " " << std::endl;
                        if (zbuffer[int(i + j * width)] < k)
                        {
                            //std::cout << "w " << widthTexture << " h " << heigthTexture << std::endl;
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
    constexpr int height = 980;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage texture(width, height, TGAImage::RGB);
    //char test = texture.read_tga_file("texture.tga");
    texture.read_tga_file("texture.tga");
    parserfile(width, height, framebuffer);
    triangle(width, height, framebuffer, texture);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}