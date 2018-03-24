
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

struct lightSource{
    lightSource(vec3 pos, vec3 Cl, vec3 Ca);
    vec3 pos, Cl, Ca;
};
struct sphere{
    sphere(vec3 center, float radius, vec3 Cr, vec3 Cp,float phong);
    vec3 center;
    float radius;
    vec3 Cr, Cp;
    float phong; 
    int relfectMode;

};

struct triangle{
    triangle(vec3 a, vec3 b, vec3 c, vec3 Cr, vec3 Cp, float phong);
    vec3 a;
    vec3 b;
    vec3 c;
  
    vec3 Cr, Cp;
    float phong; 
    int relfectMode;
};

struct plane{
    plane(vec3 n, vec3 q, vec3 Cr, vec3 Cp, float phong);
    vec3 n;
    vec3 q;
//    vec3 color;

    vec3 Cr, Cp;
    float phong; 
    int relfectMode;
};


class parser{
    

public:
    vector<sphere> spheres;
    vector<triangle> triangles;
    vector<plane> planes;
    vector<lightSource> lightSources;
    void extractShapes(const char*);
 


    
};