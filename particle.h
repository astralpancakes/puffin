//
//  particle.h
//  Puffin
//
//  Created by Peter Schulman on 2/13/13.
//
//

#ifndef Puffin_particle_h
#define Puffin_particle_h

typedef struct
{
    GLfloat position[3];
    GLfloat direction[3];
    GLint age;
}
PUFparticle;

typedef struct
{
    PUFparticle* particles;
    int particleCount;
}
PUFparticlesys;

void pufParticlesysCreate();
void pufParticlesysUpdate();

#endif
