// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

//-----

void MyGLWidget::initializeGL() {

    BL2GLWidget::initializeGL();

    ini_escena();

    ini_camera();

    glEnable(GL_DEPTH_TEST); // Z-buffer

    homer.angle = 0;
}


void MyGLWidget::ini_escena() {

    float minx, miny, minz, maxx, maxy, maxz;

    minx = -2.0; miny = -1.0; minz = -2.0;
    maxx = 2.0;  maxy = 1.0;  maxz = 2.0;

    escena.minim = glm::vec3(minx, miny, minz);     
    escena.maxim = glm::vec3(maxx, maxy, maxz);  
    escena.centre = glm::vec3((minx+maxx)/2.f, (miny+maxy)/2.f, (minz+maxz)/2.f);   // (min+max)/2.f
      

    escena.radi = glm::distance(escena.centre, escena.maxim);
}


void MyGLWidget::ini_camera() {

    cam3persona.FOVinicial = 2*(asin(float(escena.radi/(2*escena.radi)))); // FOV = 2*alfa  //sin(alfa) = R/2*R  //distancia = 2*R
    cam3persona.FOV   = cam3persona.FOVinicial;
    cam3persona.raw   = 1.0f;
    cam3persona.zNear = 2*escena.radi - escena.radi;
    cam3persona.zFar  = 2*escena.radi + escena.radi;
    cam3persona.OBS   = escena.centre + 2*escena.radi*glm::vec3(0., 0., 1.);  // VRP + d*v  (v vec normalitzat en la direcció)
    cam3persona.VRP   = escena.centre;
    cam3persona.UP    = glm::vec3(0, 1, 0);

    projectTransform();
    viewTransform();
}


void MyGLWidget::paintGL() {

    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Carreguem la transformació de model
    modelTransformHomer ();

    // Activem el VAO
    glBindVertexArray (VAO_Homer);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, homer.model.faces().size()*3);

    glBindVertexArray (0);


    // Carreguem la transformació de model
    modelTransformTerra ();

    // Activem el VAO
    glBindVertexArray (VAO_Terra);

    // pintem
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);

}


void MyGLWidget::resizeGL (int w, int h) {
    ample = w;
    alt = h;

    float ra = float(w)/float(h); // rav

    cam3persona.raw = ra;   
    if (ra < 1) {
        cam3persona.FOV = 2*atan(tan(cam3persona.FOVinicial/2)/ra);
    }
    
    projectTransform();
}


void MyGLWidget::modelTransformHomer() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, homer.angle, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(escala));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformTerra() {
    glm::mat4 transform (1.0f);
    transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, glm::vec3(0, -1, 0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform() {
    glm::mat4 Proj = glm::perspective(cam3persona.FOV, cam3persona.raw, cam3persona.zNear, cam3persona.zFar);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
    glm::mat4 View = glm::lookAt(cam3persona.OBS, cam3persona.VRP, cam3persona.UP);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent *event) {

    makeCurrent();

    switch (event -> key()) {

        case Qt::Key_S :  // escalar a més gran
            escala += 0.05;
            break;

        case Qt::Key_D :  // escalar a més petit
            escala -= 0.05;
            break;

        case Qt::Key_R :
            homer.angle += M_PI/4;
            break;

        default: event->ignore(); break;
    }
    update();
}


void MyGLWidget::calcularCapsa_Homer() {

    float minx, miny, minz, maxx, maxy, maxz;
    minx = maxx = homer.model.vertices()[0];
    miny = maxy = homer.model.vertices()[1];
    minz = maxz = homer.model.vertices()[2];

    for (unsigned int i = 3; i < homer.model.vertices().size(); i += 3) {
        minx = fmin(minx, homer.model.vertices()[i+0]);
        maxx = fmax(maxx, homer.model.vertices()[i+0]);

        miny = fmin(miny, homer.model.vertices()[i+1]);
        maxy = fmax(maxy, homer.model.vertices()[i+1]);

        minz = fmin(minz, homer.model.vertices()[i+2]);
        maxz = fmax(maxz, homer.model.vertices()[i+2]);
    }

    homer.minim = glm::vec3(minx, miny, minz);
    homer.maxim = glm::vec3(maxx, maxy, maxz);
    homer.centre = glm::vec3((homer.minim + homer.maxim) / 2.f);
    homer.centreBase = glm::vec3((minx + maxx)/2.f, miny, (minz + maxz)/2.f);
    homer.escala = 1.;
}


void MyGLWidget::creaBuffers() {

    homer.model.load("../../../models/HomerProves.obj");

    calcularCapsa_Homer(); 


    // Creació del VAO
    glGenVertexArrays(1, &VAO_Homer);
    glBindVertexArray(VAO_Homer);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.model.faces().size()*3*3, homer.model.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    // colors vertex
    GLuint VBO_color;
    glGenBuffers(1, &VBO_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.model.faces().size()*3*3, homer.model.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);


    // Desactivem el VAO
    glBindVertexArray(0);



    glm::vec3 Vertices[4];
    Vertices[0] = glm::vec3(-2.0, 0.0, -2.0); //posterior esquerra
    Vertices[1] = glm::vec3(2.0, 0.0, -2.0);  //posterior dreta
    Vertices[2] = glm::vec3(-2.0, 0.0, 2.0);  //frontal esq
    Vertices[3] = glm::vec3(2.0, 0.0, 2.0);   //frontal dreta

    // VAO Terra
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO_Terra;
    glGenBuffers(1, &VBO_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra);


    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);
    glm::vec3 Colors[4];
    Colors[0] = glm::vec3(128./255., 77./255., 46./255.); //color marro
    Colors[1] = glm::vec3(128./255., 77./255., 46./255.);
    Colors[2] = glm::vec3(128./255., 77./255., 46./255.);
    Colors[3] = glm::vec3(128./255., 77./255., 46./255.);

    GLuint VBO_color_Terra;
    glGenBuffers(1, &VBO_color_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_Terra);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);
}


void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}
