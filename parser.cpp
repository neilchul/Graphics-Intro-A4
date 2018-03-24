#include <fstream>
#include <iostream>
#include <cstdio>

#include "parser.h"

using namespace std;

//defining the constructor for the constructors
lightSource::lightSource(vec3 pos, vec3 Cl, vec3 Ca):pos(pos),Cl(Cl),Ca(Ca){}
sphere::sphere(vec3 center, float radius, vec3 Cr, vec3 Cp,float phong):center(center),radius(radius),Cr(Cr),Cp(Cp),phong(phong){}
triangle::triangle(vec3 a, vec3 b, vec3 c, vec3 Cr, vec3 Cp, float phong):a(a),b(b),c(c),Cr(Cr),Cp(Cp),phong(phong){}
plane::plane(vec3 n, vec3 q, vec3 Cr, vec3 Cp, float phong):n(n),q(q),Cr(Cr),Cp(Cp),phong(phong){}


 

//fuction that extract the shapes in a scene file and store them into their respective vector of each shapes
void parser::extractShapes(const char* filename){
    ifstream f (filename);

    const int BUFF_SIZE = 256; 
    char buffer [BUFF_SIZE];

    while(f){
        f.getline(buffer, BUFF_SIZE);

        string  line = string (buffer);
        
        //extract and store spheres
        if (line.find("sphere") != string::npos && line.find("#") == string::npos){
            float x,y,z;
            float cr1, cr2, cr3;
            float cp1, cp2, cp3;
            float radius, phong;
            

             //code for actually reading/storing the parameters 
            f.getline(buffer, BUFF_SIZE);                       //get the next line after reading "sphere"  
            sscanf(buffer, "%f %f %f\n", &x, &y, &z);           //read and store the center of the sphere into x y and z

            f.getline(buffer, BUFF_SIZE);                       //increment line again for radius
            sscanf(buffer, "%f\n", &radius);                    //read and store the raidius into r

            f.getline(buffer, BUFF_SIZE);                       
            sscanf(buffer, "%f %f %f\n", &cr1, &cr2, &cr3);     //extract diffuse

            f.getline(buffer, BUFF_SIZE);                       
            sscanf(buffer, "%f %f %f\n", &cp1, &cp2, &cp3);     //extract the specular

            f.getline(buffer, BUFF_SIZE);                       
            sscanf(buffer, "%f\n", &phong);                     //extract phong
            
            //int reflectMode = 0;
            //f.getline(buffer, BUFF_SIZE);                       
            //sscanf(buffer, "%d\n", &reflectMode);               //extract phong

          //  printf("sph: (%f,%f,%f) || (%f, %f, %f) \n", , Cr.y, sph.Cr.y, sph.Cp.x, sph.Cp.y, sph.Cp.z);

            //create a sphere and store it into the vectors of spheres
            sphere s = sphere(vec3(x,y,z), radius, vec3(cr1, cr2,cr3), vec3(cp1,cp2,cp3), phong);
            spheres.push_back(s);

        }

        //extract and store triangles
        else if (line.find("triangle") != string::npos && line.find("#") == string::npos){
            
            float x,y,z;
            vector<vec3> t;

            for (int i = 0; i < 5 ; i++){
                f.getline(buffer, BUFF_SIZE);
                sscanf(buffer, "%f %f %f\n", &x, &y, &z);
                t.push_back(vec3(x,y,z));
                
               
            }

            float phong;
            f.getline(buffer, BUFF_SIZE);
            sscanf(buffer, "%f\n", &phong);
               

            triangles.push_back(triangle(t[0], t[1], t[2], t[3], t[4], phong));
        }

        //extract planes
        else if (line.find("plane") != string::npos && line.find("#") == string::npos){
            float x,y,z;
            vector<vec3> pl;

            for (int i = 0; i < 4 ; i++){
                f.getline(buffer, BUFF_SIZE);
                sscanf(buffer, "%f %f %f\n", &x, &y, &z);
                pl.push_back(vec3(x,y,z));

            }
          
            float phong;
            f.getline(buffer, BUFF_SIZE);
            sscanf(buffer, "%f\n", &phong);               

            planes.push_back(plane(pl[0], pl[1], pl[2], pl[3], phong));
        }

        //extract light
        else if (line.find("light") != string::npos && line.find("#") == string::npos){
            
            float x,y,z;
            vector<vec3> li;
          
            for (int i = 0; i < 3 ; i++){
                f.getline(buffer, BUFF_SIZE);
                sscanf(buffer, "%f %f %f\n", &x, &y, &z);
                li.push_back(vec3(x,y,z));
            }
           
                
           lightSources.push_back(lightSource(li[0], li[1], li[2]));

        }

    }

     
}

void extractSphere(){
    
}

//getline
/*

*/
/*
int main (int argc, char* argv[]){
    
    if (argc <= 1){
        printf("Needs filename\n");
        return -1;
    }
    
  //  PrintLines(argv[1]);
    findNumber(argv[1]);
    return 0;
}*/