// MyGLWidget.h

    protected:

      virtual void ini_escena();

      virtual void ini_camera();

      virtual void calcularCapsa_Homer();


      // ============ Structs ============

      struct Figura {
        Model model;
        glm::vec3 minim;
        glm::vec3 maxim;
        glm::vec3 centre;
        glm::vec3 centreBase;
        float escala;
        float angle;
      };

      Figura homer;


      struct Escena {
        glm::vec3 minim;
        glm::vec3 maxim;
        glm::vec3 centre;
        float radi;
      };

      Escena escena;


      struct Camera {
        float FOV;
        float FOVinicial;
        float raw;
        float zNear;
        float zFar;
        glm::vec3 OBS;
        glm::vec3 VRP;
        glm::vec3 UP;
      };

      Camera cam3persona;




// MyGLWidget.cpp

void MyGLWidget::initializeGL() {

    BL2GLWidget::initializeGL();

    ini_escena();

    ini_camera();

    glEnable(GL_DEPTH_TEST); // activar Z-buffer

    homer.angle = 0;
}



void MyGLWidget::ini_escena() {

    float minx, miny, minz, maxx, maxy, maxz;

    minx = -2.0; miny = -1.0; minz = -2.0;
    maxx = 2.0;  maxy = 1.0;  maxz = 2.0;

    escena.minim = glm::vec3(minx, miny, minz);
    escena.maxim = glm::vec3(maxx, maxy, maxz);
    escena.centre = glm::vec3((minx + maxx)/2.f, (miny + maxy)/2.f, (minz + maxz)/2.f);   // El centre sempre es (min+max)/2.f

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

    homer.model.load("../../../models/HomerProves.obj"); // .. és com fer # cd .. (vas a la carpeta anterior), ruta relativa
                                                    // des de la carpeta del projecte vas a la dels models
    calcularCapsa_Homer();

    //...
    //...
}


