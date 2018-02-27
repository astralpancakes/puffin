#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>


GLfloat* pufLoadOBJ(char const* name,int* vertexCount)
{
    FILE* f;
    int vertexCounter = 0;
    
    if((f = fopen(name, "r"))) // file opened successfully
    {

        //printf("Puffin OBJ loader loading file %s...\n", name);

        // temporary variables for scanf
        int a,b,c;  
        float f1,f2,f3;
        int d1,d2,d3,d4,d5,d6,d7,d8,d9;
        // vertex/normal/texcoord/face counters
        int vCount = 0;
        int nCount = 0;
        int tCount = 0;
        int fCount = 0;
        char buffer[256] = {0};

// Do a first pass scan through the OBJ file and count the vertex, normal and texture coordinates and faces

        while (fscanf(f,"%s", buffer) != EOF)
        {
            switch (buffer[0])
            {
                case 'v':  // found a v, for vertex, then...
                    switch (buffer[1]) // ...check the next character:
                    {
                        case '\0':  // null means we have "v", or vertex coordinate
                            vCount++;
                            break;
                        case 'n':
                            nCount++; // n means "vn", or vertex normal
                            break;
                        case 't':
                            tCount++; // t means "vt", or vertex texture coordinate
                            break;
                    }
                    break;
                case 'f': // found a f, for face
                    switch (buffer[1])
                    {
                        case '\0':   // check the next character to make sure it's null and nothing weird
                            fscanf(f,"%s", buffer); // scan forward, then check what we scanned...
                            if (strstr(buffer, "//")) // if strstr() does not return NULL we found //, which means the next coordinates are of the format v//vn
                            {
                                // assume there will be at least two more coordinates (making a triangle)
                                fscanf(f,"%d//%d",&a,&b);
                                fscanf(f,"%d//%d",&a,&b);
                                fCount++;
                                // there might even be a fourth coordinate (two triangles, or a quad)
                                while (fscanf(f,"%d//%d",&a,&b) == 2) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d/%d", &a,&b,&c) == 3) // three values mean the coordinates are of the form v/vt/vn
                            {
                                fscanf(f,"%d/%d/%d",&a,&b,&c);
                                fscanf(f,"%d/%d/%d",&a,&b,&c);
                                fCount++;
                                while (fscanf(f,"%d/%d/%d",&a,&b,&c) == 3) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d", &a,&b) == 2) // two values (but no //) means we have v/vt
                            {
                                fscanf(f,"%d/%d",&a,&b);
                                fscanf(f,"%d/%d",&a,&b);
                                fCount++;
                                while (fscanf(f,"%d/%d",&a,&b) == 2) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else // one value means v
                            {
                                fscanf(f,"%d",&a);
                                fscanf(f,"%d",&a);
                                fCount++;
                                while (fscanf(f,"%d",&a) > 0) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            break;
                    }
                    break;
            }
        }
        rewind(f); // PULL UP MY SELECTOR!

        GLfloat vertexList[vCount][3];  // make an array for our vertices, three coordinates for each

        if (nCount == 0) // we need at least one set of normal coordinates, so we make sure we get some
            nCount = 1;

        // make an array for the normal coordinates, and set the first ones to zero (if there are any in the OBJ file, these will be overwritter)
        GLfloat normalList[nCount][3]; 
        normalList[0][0] = 0.0f;
        normalList[0][1] = 0.0f;
        normalList[0][2] = 0.0f;

        if (tCount == 0) // we make sure we get some texture coordinates too
            tCount = 1;

        // and make an array for the texture coordinates, with the first ones set to zero, at least for now
        
        GLfloat texList[tCount][3];
        texList[0][0] = 0.0f;
        texList[0][1] = 0.0f;
        texList[0][2] = 0.0f;

        // make some arrays for reading in the coordinate indices for our faces

        GLint faceList[fCount][3];
        GLint faceTex[fCount][3];
        GLint faceNormals[fCount][3];

        //printf("Vertices: %i\n",vCount);
        //printf("Triangles: %i\n",fCount);

        // Scan through file a second time, reading... 
        // - vertex coords into vertexList, 
        // - vertex texture coords into texList, 
        // - vertex normals into normalList, 
        // - triangle vertex value indices into faceList, 
        // - triangle normal value indices faceNormals and 
        // - triangle textures value indices into faceTex

        int i = 0;
        int j = 0;
        int k = 0;
        int l = 0;

        while (fscanf(f,"%s", buffer) != EOF)
        {
            switch (buffer[0])
            {
                case 'v':  // found a v, for vertex, then...
                    switch (buffer[1]) // ...check the next character
                    {
                        case '\0':  // NULL means vertex coordinate, we put that in vertexList

                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            fscanf(f,"%f",&f3);
                            vertexList[i][0] = f1;
                            vertexList[i][1] = f2;
                            vertexList[i][2] = f3;

                            //printf("Vertex %i: %f, %f, %f\n", i, vertexList[i][0],vertexList[i][1],vertexList[i][2]);

                            i++;
                            break;

                        case 'n': // n means vertex normal, we put that in normalList
                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            fscanf(f,"%f",&f3);
                            normalList[k][0] = f1;
                            normalList[k][1] = f2;
                            normalList[k][2] = f3;

							//printf("Vertex normal %i: %f, %f, %f\n", k, normalList[k][0],normalList[k][1],normalList[k][2]);

							k++;
                            break;

                        case 't': // t means texture coordinate, we put that in texList (and assume the texture is always two-dimensional, so the third value is zero)
                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            //fscanf(f,"%f",&f3);
                            texList[l][0] = f1;
                            texList[l][1] = f2;
                            texList[l][2] = 0.0f;

							//printf("Vertex tex %i: %f, %f, %f\n", l, texList[l][0],texList[l][1],texList[l][2]);

							l++;
                            break;
                    }
                break;

                case 'f': // On to the faces
                    switch (buffer[1])
                    {
                        case '\0': 
                            fscanf(f,"%s", buffer);
                            if (strstr(buffer, "//")) // v//vn
                            {
                                // read face and normal indices from buffer, make the texture indices zero
                                sscanf(buffer,"%d//%d",&d1,&d7);
                                fscanf(f,"%d//%d",&d2,&d8);
                                fscanf(f,"%d//%d",&d3,&d9);
                                faceList[j][0] = d1;
                                faceList[j][1] = d2;
                                faceList[j][2] = d3;
                                faceTex[j][0] = 0;
                                faceTex[j][1] = 0;
                                faceTex[j][2] = 0;
                                faceNormals[j][0] = d7;
                                faceNormals[j][1] = d8;
                                faceNormals[j][2] = d9;

								/*
                                printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
								*/
								
								j++;

                                // if there's a fourth set of index values, read that too
                                while (fscanf(f,"%d//%d",&d2, &d8) == 2) 
                                {
                                    faceList[j][0] = d1;
                                    faceList[j][1] = d3;
                                    faceList[j][2] = d2;
                                    faceTex[j][0] = 0;
                                    faceTex[j][1] = 0;
                                    faceTex[j][2] = 0;
                                    faceNormals[j][0] = d7;
                                    faceNormals[j][1] = d8;
                                    faceNormals[j][2] = d9;

									/*
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
									*/
									
									d3 = d2;
                                    d9 = d8;
                                    j++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d/%d", &a,&b,&c) == 3) // v/vt/vn
                            {
                                // read it all in
                                sscanf(buffer,"%d/%d/%d",&d1,&d4,&d7);
                                fscanf(f,"%d/%d/%d",&d2,&d5,&d8);
                                fscanf(f,"%d/%d/%d",&d3,&d6,&d9);
                                faceList[j][0] = d1;
                                faceList[j][1] = d2;
                                faceList[j][2] = d3;
                                faceTex[j][0] = d4;
                                faceTex[j][1] = d5;
                                faceTex[j][2] = d6;
                                faceNormals[j][0] = d7;
                                faceNormals[j][1] = d8;
                                faceNormals[j][2] = d9;

								/*
								printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
								*/
								
								j++;

                                // and the fourth one too, if we find one
                                while (fscanf(f,"%d/%d/%d",&d2,&d5, &d8) == 3)
                                {
                                    faceList[j][0] = d1;
                                    faceList[j][1] = d3;
                                    faceList[j][2] = d2;
                                    faceTex[j][0] = d4;
                                    faceTex[j][1] = d6;
                                    faceTex[j][2] = d5;
                                    faceNormals[j][0] = d7;
                                    faceNormals[j][1] = d9;
                                    faceNormals[j][2] = d8;

									/*
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
									*/
									
									d3 = d2;
                                    d6 = d5;
                                    d9 = d8;
                                    j++;

                                }
                            }
                            else if (sscanf(buffer, "%d/%d", &a,&b) == 2) // v/vt
                            {
                                // read vertex and texture coordinate indices in, make the normal indices zero
                                sscanf(buffer,"%d/%d",&d1,&d4);
                                fscanf(f,"%d/%d",&d2,&d5);
                                fscanf(f,"%d/%d",&d3,&d6);
                                faceList[j][0] = d1;
                                faceList[j][1] = d2;
                                faceList[j][2] = d3;
                                faceTex[j][0] = d4;
                                faceTex[j][1] = d5;
                                faceTex[j][2] = d6;
                                faceNormals[j][0] = 0;
                                faceNormals[j][1] = 0;
                                faceNormals[j][2] = 0;

								/*
                                printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
								*/
								
								j++;
                                // and the fourth one, if it's there
                                while (fscanf(f,"%d/%d",&d2,&d5) == 2)
                                {
                                    faceList[j][0] = d1;
                                    faceList[j][1] = d3;
                                    faceList[j][2] = d2;
                                    faceTex[j][0] = d4;
                                    faceTex[j][1] = d6;
                                    faceTex[j][2] = d5;
                                    faceNormals[j][0] = 0;
                                    faceNormals[j][1] = 0;
                                    faceNormals[j][2] = 0;

									/*
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
									*/
									
									d3 = d2;
                                    d6 = d5;
                                    j++;

                                }
                            }
                            else    // v
                            {
                                // vertex coordinate indices only, read them in, make texture and normal indices zero
                                sscanf(buffer,"%d",&d1);
                                fscanf(f,"%d",&d2);
                                fscanf(f,"%d",&d3);
                                faceList[j][0] = d1;
                                faceList[j][1] = d2;
                                faceList[j][2] = d3;
                                faceTex[j][0] = 0;
                                faceTex[j][1] = 0;
                                faceTex[j][2] = 0;
                                faceNormals[j][0] = 0;
                                faceNormals[j][1] = 0;
                                faceNormals[j][2] = 0;

								//printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);

								j++;

                                // if the fourth one is there, read that also
                                while (fscanf(f,"%d",&d2) == 1)
                                {
                                    faceList[j][0] = d1;
                                    faceList[j][1] = d3;
                                    faceList[j][2] = d2;
                                    faceTex[j][0] = 0;
                                    faceTex[j][1] = 0;
                                    faceTex[j][2] = 0;
                                    faceNormals[j][0] = 0;
                                    faceNormals[j][1] = 0;
                                    faceNormals[j][2] = 0;

									//printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);

									d3 = d2;
                                    j++;
                                }
                            }
                            break;
                    }
                break;
            }
        }
        fclose(f);

/* Combine all the previously created list into a big list of vertex, texture and normal coordinates for each face. This is pretty hairy... */

        static GLfloat* combinedList;
        combinedList = (GLfloat*)malloc(sizeof(GLfloat)*(fCount*3*3*3)); // every triangle has 3 vertices * pos/normal/UV  * 3 coordinates

        for (i=0; i < fCount; i++)  // for each face...
        {
            int j;
            for(j = 0; j < 3; j++) // ...and each vertex in that face...
            {
                // ...write all the coordinates
                combinedList[i*27+j*9+0] = vertexList[faceList[i][j]-1][0];
                combinedList[i*27+j*9+1] = vertexList[faceList[i][j]-1][1];
                combinedList[i*27+j*9+2] = vertexList[faceList[i][j]-1][2];
                combinedList[i*27+j*9+3] = texList[faceTex[i][j]-1][0];
                combinedList[i*27+j*9+4] = texList[faceTex[i][j]-1][1];
                combinedList[i*27+j*9+5] = texList[faceTex[i][j]-1][2];
                combinedList[i*27+j*9+6] = normalList[faceNormals[i][j]-1][0];
                combinedList[i*27+j*9+7] = normalList[faceNormals[i][j]-1][1];
                combinedList[i*27+j*9+8] = normalList[faceNormals[i][j]-1][2];
                vertexCounter++;				
            }

        }

    *vertexCount = vertexCounter;
    return combinedList;
    }
    else
    {
        printf("Puffin OBJ loader failed to open file %s\n",name);
        return NULL;
    }


}
