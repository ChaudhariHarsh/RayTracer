#include<iostream>
#include<limits>
#include<fstream>
#include<random>
#include<glm/glm.hpp>
#include<glm/gtx/norm.hpp>
#include "ray.hpp"
#include "hitable_list.hpp"
#include "sphere.hpp"
#include "camera.h"

using namespace std;
using namespace glm;

vec3 random_point_in_unit_sphere()
{
    vec3 p;
    do
    {
        p = 2.0f*vec3(drand48(), drand48(), drand48()) - vec3(1.0f);
    }
    while(length2(p) >= 1.0f);
    return p;
}

vec3 color(const ray& r, hitable *world, int numBounces)
{
    hit_record rec;
    if((numBounces>0) && world->hit(r, 0.001, numeric_limits<float>::max(), rec))
    {
        vec3 target = rec.p + rec.normal + random_point_in_unit_sphere();
        return 0.5f*color(ray(rec.p, target - rec.p), world, numBounces - 1); 
        //0.5f*vec3(rec.normal.x + 1.0f, rec.normal.y + 1.0f, rec.normal.z + 1.0f);
    }
    else
    {
        vec3 unit_direction = normalize(r.direction());
        float t = 0.5*(unit_direction.y + 1.0);

        //return blend between blue and white
        return (1.0f-t)*vec3(1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}

int main()
{
    ofstream fout;
    fout.open("picture.ppm");

    int nx=800;     //x-resolution
    int ny=400;     //y-resolution
    int ns = 20;   //number of samples per pixel

    fout<<"P3\n"<< nx << " " << ny << "\n255\n";
    vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    vec3 horizontal(4.0f, 0.0f, 0.0f);
    vec3 vertical(0.0f, 2.0f, 0.0f);
    vec3 origin(0.0f);

    hitable *list[4];
    list[3] = new sphere(vec3(-0.3f, 0.4f, -1.1f), 0.2);
    list[0] = new sphere(vec3(0.0f, -100.5f, -1.0f), 100.0);
    list[2] = new sphere(vec3(0.0f, 0.0f, -1.0f), 0.5);
    list[1] = new sphere(vec3(0.3f, 0.4f, -0.9f), 0.2);
    hitable *world = new hitable_list(list, 4);

    Camera cam;
    for(int j=ny-1; j>=0; j--)
    {
        for(int i=0;i < nx; ++i)
        {
            //Take multiple samples per pixel, for antialiasing
            vec3 col = vec3(0.0f, 0.0f, 0.0f);
            for(int sample = 0; sample < ns; ++sample)
            {
                float u = float(i + drand48())/float(nx);
                float v = float(j + drand48())/float(ny);
                ray r=cam.get_ray(u, v);
                //color takes a ray, a world, and tells us color of where it hits
                col += color(r, world, 30);

            }
            col /= float(ns);
            int ir = int(255.99*col.r);
            int ig = int(255.99*col.g);
            int ib = int(255.99*col.b);
            fout<< ir << " " << ig << " " << ib << "\n";
        }
    }
    return 0;
}
