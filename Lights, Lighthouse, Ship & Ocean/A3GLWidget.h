
#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"
#include "MyGLWidget.h"

class A3GLWidget : public MyGLWidget
{
  Q_OBJECT

#define NUM_TORXES 6
#define HORA_MIN 8
#define HORA_MAX 20
#define NUM_FOCUS_FAR 2

  public:
    A3GLWidget (QWidget *parent=0);
    ~A3GLWidget ();

public slots:
        void actualizaposicioSol(int valor);
        void seleccionaTorxa(int num);
        void canviacolorsTorxes(int valor);
        void apagaTorxa();
    signals:
        void WidgetSolupdate(int valor);
protected:

    void modelTransformFar1();
    void modelTransformFar2();
    void modelTransformVaixell();
    void iniEscena ();
    void carregaShaders();
    void updateSunPosition();


    virtual void paintGL ( );
    virtual void keyPressEvent (QKeyEvent *event);
   
    GLuint normalMatrixLoc;
    GLuint posFocusTorxesLoc;
    GLuint torchColorLoc;
   
    glm::vec3 VaixellPos;
    glm::vec3 SolPos;
    glm::vec3 sunPosition;

   
    glm::vec3 savecol; 
    glm::vec3 torchColors[6]; 

    glm::mat4 Vaixell_TG, Far2_TG;

   
    float currentHour;
    float angle; 
    float angle2;
    float angleFarSegment;
    float angleFoci; 
    int prevValor; 
    int selectedTorchIndex;
    int val = 1; 
};

