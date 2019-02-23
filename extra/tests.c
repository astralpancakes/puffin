void pufTestOpenGLImmediate()
{
	glClearColor(0,0,0,0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-2.0,2.0,-2.0,2.0,0.1,10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f,0.0f, -6.0f);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f,1.0f,1.0f);
        glVertex3f(0.0f,1.0f,0.0f);
        glVertex3f(-1.0f,-1.0f,0.0f);
        glVertex3f(1.0f,-1.0f,0.0f);
    glEnd();
}