#include <stdio.h>
#include <string.h>

#ifdef PUFFIN_GLEWDOTFRAMEWORK
#include <GLEW/glew.h>
#else
#include <GL/glew.h>
#endif

GLfloat* pufLoadOBJ(char const* name,int* vertexCount)
{
    FILE* f;

    int vertexCounter = 0;
#ifdef PUFFIN_SQUAWK
    printf("\n%s\n","OBJ Loader:");
#endif
    if(f = fopen(name, "r")) //file opened successfully
    {
#ifdef PUFFIN_SQUAWK
        printf("Parsing mesh %s...\n", name);
#endif
        int a,b,c;  //temporary variables for scanf
        float f1,f2,f3;
        int d1,d2,d3,d4,d5,d6,d7,d8,d9;
        int vCount = 0; //vertex counter
        int nCount = 0; //vertex normal counter
        int tCount = 0; //vertex texture counter
        int fCount = 0; //face counter
        char buffer[256] = {0};

        while (fscanf(f,"%s", buffer) != EOF)  //scan through file, counting vertices, normals and faces
        {
            switch (buffer[0])
            {
                case 'v':  // vertex
                    switch (buffer[1])
                    {
                        case '\0':
                            vCount++;
                            break;
                        case 'n':
                            nCount++;
                            break;
                        case 't':
                            tCount++;
                            break;
                    }
                    break;
                case 'f':
                    switch (buffer[1])
                    {
                        case '\0':
                            fscanf(f,"%s", buffer);
                            if (strstr(buffer, "//")) // v//vn
                            {

                                fscanf(f,"%d//%d",&a,&b);
                                fscanf(f,"%d//%d",&a,&b);
                                fCount++;
                                while (fscanf(f,"%d//%d",&a,&b) == 2) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d/%d", &a,&b,&c) == 3) // v/vt/vn
                            {
                                fscanf(f,"%d/%d/%d",&a,&b,&c);
                                fscanf(f,"%d/%d/%d",&a,&b,&c);
                                fCount++;
                                while (fscanf(f,"%d/%d/%d",&a,&b,&c) == 3) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d", &a,&b) == 2) // v/vt
                            {
                                fscanf(f,"%d/%d",&a,&b);
                                fscanf(f,"%d/%d",&a,&b);
                                fCount++;
                                while (fscanf(f,"%d/%d",&a,&b) == 2) // loop until /n
                                {
                                    fCount++;
                                }
                            }
                            else    // v
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
        rewind(f);
        GLfloat vertexList[vCount][3];

        if (nCount == 0) // empty normalList entry if no normal coords exist
            nCount = 1;
        GLfloat normalList[nCount][3];
        normalList[0][0] = 0.0f;
        normalList[0][1] = 0.0f;
        normalList[0][2] = 0.0f;

        if (tCount == 0) // empty texList entry if no texture coords exist
            tCount = 1;
        GLfloat texList[tCount][3];
        texList[0][0] = 0.0f;
        texList[0][1] = 0.0f;
        texList[0][2] = 0.0f;

        GLint faceList[fCount][3];
        GLint faceTex[fCount][3];
        GLint faceNormals[fCount][3];
#ifdef PUFFIN_SQUAWK
        printf("Vertices: %i\n",vCount);
        printf("Triangles: %i\n",fCount);
#endif
        int i = 0;
        int j = 0;
        int k = 0;
        int l = 0;
        while (fscanf(f,"%s", buffer) != EOF)  //scan through file, reading vertex coords into vertexList, textures into texList, normals into normalList, triangle vertex coords into faceList, triangle normals into faceNormals, triangle textures into faceTex
        {
            switch (buffer[0])
            {
                case 'v':  // vertex
                    switch (buffer[1])
                    {
                        case '\0':


                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            fscanf(f,"%f",&f3);
                            vertexList[i][0] = f1;
                            vertexList[i][1] = f2;
                            vertexList[i][2] = f3;
#ifdef PUFFIN_SQUAWK
                            printf("Vertex %i: %f, %f, %f\n", i, vertexList[i][0],vertexList[i][1],vertexList[i][2]);
#endif
                            i++;
                            break;

                        case 'n':
                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            fscanf(f,"%f",&f3);
                            normalList[k][0] = f1;
                            normalList[k][1] = f2;
                            normalList[k][2] = f3;
#ifdef PUFFIN_SQUAWK
							printf("Vertex normal %i: %f, %f, %f\n", k, normalList[k][0],normalList[k][1],normalList[k][2]);
#endif
							k++;
                            break;

                        case 't':
                            fscanf(f,"%f",&f1);
                            fscanf(f,"%f",&f2);
                            //fscanf(f,"%f",&f3);
                            texList[l][0] = f1;
                            texList[l][1] = f2;
                            texList[l][2] = 0.0f;
#ifdef PUFFIN_SQUAWK
							printf("Vertex tex %i: %f, %f, %f\n", l, texList[l][0],texList[l][1],texList[l][2]);
#endif
							l++;
                            break;
                    }
                break;

                case 'f':
                    switch (buffer[1])
                    {
                        case '\0':
                            fscanf(f,"%s", buffer);
                            if (strstr(buffer, "//")) // v//vn
                            {

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
#ifdef PUFFIN_SQUAWK
                                printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
								j++;

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
#ifdef PUFFIN_SQUAWK
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
									d3 = d2;
                                    d9 = d8;
                                    j++;
                                }
                            }
                            else if (sscanf(buffer, "%d/%d/%d", &a,&b,&c) == 3) // v/vt/vn
                            {
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
#ifdef PUFFIN_SQUAWK
								printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
								j++;
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
#ifdef PUFFIN_SQUAWK
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
									d3 = d2;
                                    d6 = d5;
                                    d9 = d8;
                                    j++;

                                }
                            }
                            else if (sscanf(buffer, "%d/%d", &a,&b) == 2) // v/vt
                            {
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
#ifdef PUFFIN_SQUAWK
                                printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
								j++;
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
#ifdef PUFFIN_SQUAWK
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
                                    printf("Triangle %i texture: %d, %d, %d\n", j, faceTex[j][0],faceTex[j][1],faceTex[j][2]);
                                    printf("Triangle %i normals: %d, %d, %d\n", j, faceNormals[j][0],faceNormals[j][1],faceNormals[j][2]);
#endif
									d3 = d2;
                                    d6 = d5;
                                    j++;

                                }
                            }
                            else    // v
                            {

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
#ifdef PUFFIN_SQUAWK
								printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
#endif
								j++;

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
#ifdef PUFFIN_SQUAWK
									printf("Triangle %i: %d, %d, %d\n", j, faceList[j][0],faceList[j][1],faceList[j][2]);
#endif
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


        static GLfloat* combinedList;
        combinedList = (GLfloat*)malloc(sizeof(GLfloat)*(fCount*3*3*3)); //every triangle has 3 vertices * pos, normal, UV  * 3 coordinates
        //combinedList = new GLfloat[fCount*3*3*3];

        for (i=0; i < fCount; i++)  //combine lists
        {
            for(int j = 0; j < 3; j++)
            {
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
#ifdef PUFFIN_SQUAWK
				printf("f%i-v%i: %f,%f,%f\n",i,j,combinedList[i*27+j*9+0],combinedList[i*27+j*9+1],combinedList[i*27+j*9+2]);
                printf("f%i-t%i: %f,%f,%f\n",i,j,combinedList[i*27+j*9+3],combinedList[i*27+j*9+4],combinedList[i*27+j*9+5]);
                printf("f%i-n%i: %f,%f,%f\n",i,j,combinedList[i*27+j*9+6],combinedList[i*27+j*9+7],combinedList[i*27+j*9+8]);
#endif
            }

        }

    *vertexCount = vertexCounter;
    return combinedList;
    }
    else
    {
        printf("Failed to open mesh %s...\n",name);
        return NULL;
    }


}

