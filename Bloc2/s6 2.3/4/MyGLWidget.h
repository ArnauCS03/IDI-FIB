// MyGLWidget.h
#include "BL2GLWidget.h"
#include "model.h"
#include <QMouseEvent> // nou

class MyGLWidget : public BL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL2GLWidget(parent) {}
    ~MyGLWidget();

  protected:

    virtual void initializeGL();

    virtual void ini_escena();

    virtual void ini_camera();

    virtual void paintGL();

    virtual void pintaPatricio2();

    virtual void pintaPatricio3();

    virtual void resizeGL(int width, int height);

    virtual void modelTransformPatricio1();

    virtual void modelTransformPatricio2();

    virtual void modelTransformPatricio3();

    virtual void modelTransformTerra();

    virtual void projectTransform();

    virtual void viewTransform();

    virtual void keyPressEvent(QKeyEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void calcularCapsa_Patricio();

    virtual void creaBuffers();

    virtual void carregaShaders();


    GLuint projLoc;
    GLuint viewLoc;

    GLuint VAO_Terra;

    
    // ============ Structs ============

   struct Figura {
      Model model;
      glm::vec3 minim;
      glm::vec3 maxim;
      glm::vec3 centre;
      glm::vec3 centreBase;
      float amplada;
      float profunditat;
      float alcada;
      float escala;
      float angle; 
      GLuint VAO;
    };

    Figura patricio;
  
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
      float left;
      float right;  
      float bottom;
      float top;
      float zNear;
      float zFar;
      float distancia;
      glm::vec3 OBS;
      glm::vec3 VRP;
      glm::vec3 UP;
      bool prespectiva;
      float psi;    // Ψ
      float theta;  // θ
      float phi;    // φ
    };

    Camera cam3persona;


    struct Ratoli {
      bool inici_accio;
      glm::vec2 pos_inicial;    // pos x i y d'on hem clicat 
      float sensitibity;        // suavitzador del moviment amb el ratolí [opcional]
      float invertir_moviment;  //[] amb shift + click  el movient del ratoli es invertit 
      bool modo_freestyle;      //[]
    };
    
    Ratoli ratoli;

 
  private:

    int printOglError(const char file[], int line, const char func[]);

};
