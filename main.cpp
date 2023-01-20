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
        { // equivalent to myfile.good()
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
                    // Séparer de la ligne f en fonction des / et prise en compte uniquement de la première valeur
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

// Fonction coloriant l'intérieur du triangle
void triangleColoriage(vector<int> v1,vector<int> v2,vector<int> v3, vector<float> vAl, vector<float> vBl, vector<float> vCl, TGAImage &framebuffer)
{
    const TGAColor alea = {std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), std::uint8_t(rand() % 255), 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor cyan = {255, 255, 0, 255};
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor green = {0, 255, 0, 255};
    //vA = v1, vB = v2, vC = v3
    vector<int> vA = v1, vB = v2, vC = v3, vP;
    vector<int> vAB = {vA[0] - vB[0],vA[1] - vB[1]}, vBC = {vB[0] - vC[0],vB[1] - vC[1]}, vAC = {vA[0] - vC[0],vA[1] - vC[1]};
    //Vecteurs pour la lumière
    vector<float> N = {(vBl[1] - vAl[1]) * (vCl[2] - vAl[2]) -  (vBl[2] - vAl[2]) * (vCl[1] - vAl[1]),
   - (vBl[0] - vAl[0]) * (vCl[2] - vAl[2]) + (vBl[2] - vAl[2]) * (vCl[0] - vAl[0]),
   (vBl[0] - vAl[0]) * (vCl[1] - vAl[1]) - (vBl[1] - vAl[1]) * (vCl[0] - vAl[0])   };
    vector<float> lum = {0,0,-1};
    if (v1[1] > v2[1])
        std::swap(vA, vB);
    if (v1[1] > v3[1])
        std::swap(vA, vC);
    if (v2[1] > v3[1])
        std::swap(vB, vC);
    
    //Produit scalaire
    float normeN = sqrtf(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
    float normeLim = sqrtf(lum[0] * lum[0] + lum[1] * lum[1] + lum[2] * lum[2]);
    float scalaireNLim = N[0] * lum[0] + N[1] * lum[1] + N[2] * lum[2];
    const int intensiteB = (scalaireNLim / normeN * normeLim) * 255;
    const std::uint8_t intensite = -(scalaireNLim / normeN * normeLim) * 255;

    const TGAColor back = {intensite, intensite, intensite, 255};
    //std::cout << " normeN " << normeN << " normeLim " << normeLim << " scalaireNLim " << scalaireNLim << " intensiteB " << intensiteB << std::endl;
    //std::cout << " vCt[0] " << vCt[0] << " vCt[1] " << vCt[1] << " vCt[2] " << vCt[2] << " N[0] " << N[0] << " N[1] " << N[1] << " N[2] " << N[2] << " lum[0] " << lum[0] << " lum[1] " << lum[1] << " lum[2] " << lum[2] << std::endl;
    int minX = std::min({vA[0], vB[0], vC[0]});
    int maxX = std::max({vA[0], vB[0], vC[0]});
    int minY = std::min({vA[1], vB[1], vC[1]});
    int maxY = std::max({vA[1], vB[1], vC[1]});
    for (int i = minX; i <= maxX; i++)
    {
        for (int j = minY; j <= maxY; j++)
        {
            vP.push_back(i);
            vP.push_back(j);
            vector<int> vPA = {vP[0] - vA[0],vP[1] - vA[1]}, vPB = {vP[0] - vB[0],vP[1] - vB[1]}, vPC = {vP[0] - vC[0],vP[1] - vC[1]};
            float alpha = vPB[0] * vPC[1] - vPB[1] * vPC[0];
            float beta = vPC[0] * vPA[1] - vPC[1] * vPA[0];
            float gamma = vPA[0] * vPB[1] - vPA[1] * vPB[0];
            /*std::cout << " vA " << vA[0] << " " << vA[1] << std::endl;*/
            vP.clear();
            if (scalaireNLim < 0){
                if (alpha > -0.01 && beta > -0.01 && gamma > -0.01 || alpha < 0.01 && beta < 0.01 && gamma < 0.01){
                    framebuffer.set(i, j, back);
                }
            }
        }
    }
}

// Fonction dessinant les segments du triangle
void triangle(const int width, const int height, TGAImage &framebuffer)
{
    vector<int> vA, vB, vC;
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    /*for (int i = 0; i <= f.size() - 1; i++)
    {
        int a = f[i][0] - 1;
        int b = f[i][1] - 1;
        int c = f[i][2] - 1;
        vector<int> v1 = {static_cast<int>((v[a][0] + 1) * width / 2),static_cast<int>((v[a][1] + 1) * height / 2)}; //Coordonnées x et y du premier sommet du triangle
        vector<int> v2 = {static_cast<int>((v[b][0] + 1) * width / 2),static_cast<int>((v[b][1] + 1) * height / 2)};
        vector<int> v3 = {static_cast<int>((v[c][0] + 1) * width / 2),static_cast<int>((v[c][1] + 1) * height / 2)};
        vector<float> vA = v[a], vB = v[b], vC = v[c];
        // Co du centre de gravité du triangle
        // Trace les 3 lignes formant un triangle
        //line(v1[0], v1[1], v2[0], v2[1], framebuffer, blue);
        //line(v1[0], v1[1], v3[0], v3[1], framebuffer, white);
        //line(v2[0], v2[1], v3[0], v3[1], framebuffer, red);
        triangleColoriage(v1,v2,v3, vA, vB, vC, framebuffer);
    }*/
    line(20, 34, 744, 400, framebuffer, red);
    line(120, 434,444, 400, framebuffer, green);
    line(330, 463, 594, 200, framebuffer, blue);
    line(10, 10, 790, 10, framebuffer, blue);
}

int main()
{
    constexpr int width = 1024;
    constexpr int height = 980;
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor green = {0, 255, 0, 255};
    TGAImage framebuffer(width, height, TGAImage::RGB);

    parserfile(width, height, framebuffer);
    triangle(width, height, framebuffer);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
