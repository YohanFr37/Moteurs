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
vector<vector<float>> v, f;
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 1er
    /*
    for(float t = 0.; t < 1.; t+=.1){
        //Coordonées barycentriques
        int x = x0*(1.-t) + x1*t;
        int y = y0*(1.-t) + y1*t;
        image.set(x, y, color);
    }
     */
    // 2eme
    /*
    for(int x=x0; x<=x1; x++){
        float t = (x-x0)/static_cast<float>(x1-x0);
        int y = y0 + (y1-y0)*t;
        image.set(x, y, color);
    }
     */
    // 3eme
    /*
    bool steep = false;
    if(std::abs(x0-x1)<std::abs(y0-y1)){
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if(x0>x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for(int x=x0; x<=x1; x++){
        float t = (x-x0)/static_cast<float>(x1-x0);
        int y = y0 + (y1-y0)*t;
        if(steep){
            image.set(x, y, color);
        }
        else{
            image.set(y, x, color);
        }
    }
     */
    // 4eme il reset une division a opti
    /*
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error>.5) {
            y += (y1>y0?1:-1);
            error -= 1.;
        }
    }
     */
    // 5eme et dernier
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
    float tableautVecteur[3][2048];
    float tableauFace[3][4096];
    int nbVecteur = 0;
    int nbFace = 0;
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
                    // tableautVecteur[i][nbVecteur] = std::stof(token);
                }
                v.push_back(temp);
                nbVecteur++;
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
                    // Séparer de la ligne f en fonction des / et prise en compte uniquement de la première v[a]eur
                    pos = tempoLine.find(delimiter2);
                    token = tempoLine.substr(0, pos);
                    temp.push_back(std::stof(token));
                    // tableauFace[i][nbFace] = std::stof(token);
                    // std::cout << tableauFace[i][nbFace] << std::endl;
                }
                f.push_back(temp);
                nbFace++;
            }
        }
    }
}

void rasterize(vector<int> vA, vector<int> vB, TGAImage &framebuffer, TGAColor color, int ybuffer[])
{
    if (vA[0] > vB[0])
    {
        std::swap(vA, vB);
    }
    for (int i = vA[0]; i <= vB[0]; i++)
    {
        float t = (i - vA[0]) / (float)(vB[0] - vA[0]);
        int y = vA[1] * (1. - t) + vB[1] * t;
        if (ybuffer[i] < y)
        {
            //std::cout << vA[0] << " ybuffer " << ybuffer[i] << " y " << y << std::endl;
            ybuffer[i] = y;
            //std::cout << vA[0] << " ybuffer " << ybuffer[i] << " y " << y << std::endl;
            framebuffer.set(i, 11, color);
        }
    }
}
// Fonction dessinant les segments du triangle
void triangle(const int width, const int height, TGAImage &framebuffer)
{
    int ybuffer[width];
    for (int i = 0; i < width; i++)
    {
        ybuffer[i] = -1;
    }
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    vector<int> vA1 = {20, 34}, vA2 = {744, 400}, vB1 = {120, 434}, vB2 = {444, 400}, vC1 = {300, 463}, vC2 = {594, 200}, vD1 = {10, 10}, vD2 = {790, 10};

    line(vA1[0], vA1[1], vA2[0], vA2[1], framebuffer, red);
    line(vB1[0], vB1[1], vB2[0], vB2[1], framebuffer, blue);
    line(vC1[0], vC1[1], vC2[0], vC2[1], framebuffer, green);
    rasterize(vA1, vA2, framebuffer, red, ybuffer);
    rasterize(vB1, vB2, framebuffer, blue, ybuffer);
    rasterize(vC1, vC2, framebuffer, green, ybuffer);
    for (int i = 0; i <= f.size() - 1; i++)
    {
        const TGAColor alea = {std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), 255};
        int a = f[i][0] - 1;
        int b = f[i][1] - 1;
        int c = f[i][2] - 1;
        
        vector<int> vA = {static_cast<int>((v[a][0] + 1) * width / 2), static_cast<int>((v[a][1] + 1) * height / 2)}; // Coordonnées x et y du premier sommet du triangle
        vector<int> vB = {static_cast<int>((v[b][0] + 1) * width / 2), static_cast<int>((v[b][1] + 1) * height / 2)};
        vector<int> vC = {static_cast<int>((v[c][0] + 1) * width / 2), static_cast<int>((v[c][1] + 1) * height / 2)};
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
                float alpha = (i - vB[0]) * (j - vC[1]) - (j - vB[1]) * (i - vC[0]);
                float beta = (i - vC[0]) * (j - vA[1]) - (j - vC[1]) * (i - vA[0]);
                float gamma = (i - vA[0]) * (j - vB[1]) - (j - vA[1]) * (i - vB[0]);
                if (scalaireNLim < 0)
                {
                    if (alpha > -0.01 && beta > -0.01 && gamma > -0.01 || alpha < 0.01 && beta < 0.01 && gamma < 0.01)
                    {
                        framebuffer.set(i, j, back);
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
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor red = {0, 0, 255, 255};
    const TGAColor blue = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    TGAImage framebuffer(width, height, TGAImage::RGB);
    parserfile(width, height, framebuffer);
    triangle(width, height, framebuffer);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
