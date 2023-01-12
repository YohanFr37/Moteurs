#include "tgaimage.h"
#include <vector>
#include <tuple>
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

using namespace std;
vector<vector<float>> v1, f;
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
                v1.push_back(temp);
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

void triangleColoriage(int xA, int xB, int xC, int yA, int yB, int yC, TGAImage &framebuffer)
{
    const int a = xA;
    const TGAColor alea = {std::rand()%255, std::rand()%255, std::rand()%255, 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor cyan = {255, 255, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    vector<int> vA, vB, vC, vP;
    vA.push_back(xA);
    vA.push_back(yA);
    vB.push_back(xB);
    vB.push_back(yB);
    vC.push_back(xC);
    vC.push_back(yC);
    if (yA>yB) std::swap(vA, vB);
    if (yA>yC) std::swap(vA, vC);
    if (yB>yC) std::swap(vB, vC);
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
            float alpha, beta, gamma;
            vector<int> vPA, vPB, vPC;
            vPA.push_back(vP[0] - vA[0]);
            vPA.push_back(vP[1] - vA[1]);
            vPB.push_back(vP[0] - vB[0]);
            vPB.push_back(vP[1] - vB[1]);
            vPC.push_back(vP[0] - vC[0]);
            vPC.push_back(vP[1] - vC[1]);
            alpha = vPB[0] * vPC[1] - vPB[1] * vPC[0];
            beta = vPC[0] * vPA[1] - vPC[1] * vPA[0];
            gamma = vPA[0] * vPB[1] - vPA[1] * vPB[0];
            /*std::cout << " vA " << vA[0] << " " << vA[1] << std::endl;
            std::cout << " vB " << vB[0] << " " << vB[1] << std::endl;
            std::cout << " vC " << vC[0] << " " << vC[1] << std::endl;
            std::cout << " ij " << i << " " << j << std::endl;
            std::cout << " vP " << vP[0] << " " << vP[1] << std::endl;
            std::cout << " vPA " << vPA[0] << " " << vPA[1] << std::endl;
            std::cout << " vPB " << vPB[0] << " " << vPB[1] << std::endl;
            std::cout << " vPC " << vPC[0] << " " << vPC[1] << std::endl;
            std::cout << " alpha " << alpha << std::endl;
            std::cout << " beta " << beta << std::endl;
            std::cout << " gamma " << gamma << std::endl<< std::endl;*/
            vP.clear();
            if (alpha > 0 && beta > 0 && gamma > 0 || alpha < 0 && beta < 0 && gamma < 0)
            {
                framebuffer.set(i, j, red);
            }
        }
    }    
}

void triangle(const int width, const int height, TGAImage &framebuffer)
{
    int a = rand() % 255;
    const int b = a;
    const int c = 3;
    const TGAColor white = {255, 255, 255, 255};
    const TGAColor blue = {0, 0, 255, 255};
    const TGAColor red = {255, 0, 0, 255};
    const TGAColor green = {0, 255, 0, 255};
    for (int i = 0; i <= f.size()-1; i++)
    {
        int a = f[i][0] - 1;
        int b = f[i][1] - 1;
        int c = f[i][2] - 1;
        int x1 = (v1[a][0] + 1.) * width / 2.; // Position x du premier sommet du triangle
        int x2 = (v1[b][0] + 1.) * width / 2.; // Position y du premier sommet du triangle
        int x3 = (v1[c][0] + 1.) * width / 2.; // Position x du deuxième sommet du triangle etc.
        int y1 = (v1[a][1] + 1.) * height / 2.;
        int y2 = (v1[b][1] + 1.) * height / 2.;
        int y3 = (v1[c][1] + 1.) * height / 2.;
        // Co du centre de gravité du triangle
        int xg = (x1 + x2 + x3) / 3;
        int yg = (y1 + y2 + y3) / 3;
        // Trace les 3 lignes formant un triangle
        line(x1, y1, x2, y2, framebuffer, white);
        line(x1, y1, x3, y3, framebuffer, white);
        line(x2, y2, x3, y3, framebuffer, white);
        triangleColoriage(x1, x2, x3, y1, y2, y3, framebuffer);
    }
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
    // triangle(width, height, framebuffer);
    return 0;
}
