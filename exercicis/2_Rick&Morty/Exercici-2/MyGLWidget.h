#include "LL2GLWidget.h"

#include "model.h"

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) {}
    ~MyGLWidget();


  public slots:
    void animacio();

  protected:

    virtual void initializeGL();

    virtual void iniEscena();

    virtual void iniCamera();

    virtual void iniCamera2Seguretat();

    virtual void paintGL();

    virtual void RickTransform();

    virtual void MortyTransform();

    virtual void VideoCameraTransform();

    virtual void viewTransform();

    virtual void viewTransformCamera2Seguretat();
    
    virtual void projectTransform();

    virtual void projectTransformCam2Seguretat();

    virtual void resizeGL(int w, int h);

    virtual void carregaShaders();

    virtual void reset();


    virtual void keyPressEvent (QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);


    float angleVideocamera;
    bool camSeguretatActiva;

    GLuint filtreLoc;

    bool RickInvisible;

    QTimer timer;
    bool timer_activat;
    bool sentitEsqVideocamera;


    // ===== Structs =====

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
      float distancia;
      glm::vec3 OBS;
      glm::vec3 VRP;
      glm::vec3 UP;
      float psi;    // Ψ
      float theta;  // θ
      float phi;    // φ
    };

    Camera cam3persona;

    Camera camSeguretat;




  private:

    int printOglError(const char file[], int line, const char func[]);
    
};
