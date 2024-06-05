#include "A3GLWidget.h"

#include <iostream>

A3GLWidget::A3GLWidget (QWidget* parent) : MyGLWidget(parent)
{

}

A3GLWidget::~A3GLWidget()
{

}

void A3GLWidget::iniEscena ()
{
	creaBuffersModels();

	VaixellPos = glm::vec3(-10, 0.0, 0.0);



	centreEsc = glm::vec3 (0, 0, 0);

	radiEsc = 20;
    currentHour = 14;
    sunPosition = glm::vec3(0.0f, 40.0f, 0.0f);
    angle = 0.0f;
    angle2 = 40.0f;
    angleFoci = 0.0f;
    angleFarSegment = 0.0f;
    prevValor = 14;

    updateSunPosition();
}



void A3GLWidget::paintGL ()
{

#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

	projectTransform ();
	viewTransform ();

	glClearColor(0.8f, 0.8f, 1.0f, 1.f);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glBindVertexArray (VAO_models[VAIXELL]);
	modelTransformVaixell();
	glDrawArrays(GL_TRIANGLES, 0, models[VAIXELL].faces().size()*3);


	glBindVertexArray (VAO_models[MOLL]);
	modelTransformMoll();
	glDrawArrays(GL_TRIANGLES, 0, models[MOLL].faces().size()*3);


	glBindVertexArray (VAO_models[FAR_1]);
	modelTransformFar1();
	glDrawArrays(GL_TRIANGLES, 0, models[FAR_1].faces().size()*3);


	glBindVertexArray (VAO_models[FAR_2]);
	modelTransformFar2();
	glDrawArrays(GL_TRIANGLES, 0, models[FAR_2].faces().size()*3);


	glBindVertexArray (VAO_Terra);
	modelTransformTerra();
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glBindVertexArray(VAO_Mar);
	modelTransformMar();
	glUniform1i(glGetUniformLocation(program->programId(), "isSea"), GL_TRUE); 
	glUniform1f(glGetUniformLocation(program->programId(), "time"), temps);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform1i(glGetUniformLocation(program->programId(), "isSea"), GL_FALSE);
	

    glm::vec3 F1Pos = glm::vec3(0.363, 4.695, 0.357);
    glm::vec3 F2Pos = glm::vec3(-0.357, 4.695, -0.348);
    F1Pos = glm::vec3(View * Far2_TG * glm::vec4(F1Pos, 1.0));
    F2Pos = glm::vec3(View * Far2_TG * glm::vec4(F2Pos, 1.0));

    GLuint F1Loc = glGetUniformLocation(program->programId(), "F1Pos");
    glUniform3fv(F1Loc, 1, &F1Pos[0]);
    GLuint F2Loc = glGetUniformLocation(program->programId(), "F2Pos");
    glUniform3fv(F2Loc, 1, &F2Pos[0]);

    glm::vec3 DirF1 = glm::normalize(F2Pos - F1Pos);
    glm::vec3 DirF2 = -DirF1;

    GLuint DirF1Loc = glGetUniformLocation(program->programId(), "DirF1");
    glUniform3fv(DirF1Loc, 1, &DirF1[0]);
    GLuint DirF2Loc = glGetUniformLocation(program->programId(), "DirF2");
    glUniform3fv(DirF2Loc, 1, &DirF2[0]);


    glm::vec3 torchPositions[6] = {
        glm::vec3(-7.39f, 1.95f, -6.68f),
        glm::vec3(-9.95f, 1.95f, -0.56f),
        glm::vec3(-7.47f, 1.95f, 5.64f),
        glm::vec3(4.38f, 1.95f, 5.26f),
        glm::vec3(6.68f, 1.95f, 0.38f),
        glm::vec3(4.15f, 1.95f, -6.97f)
    };

    glm::mat4 torchTransform = glm::translate(glm::mat4(1.0f), VaixellPos);
    for (int i = 0; i < 6; ++i) {
        torchPositions[i] = glm::vec3(torchTransform * glm::vec4(torchPositions[i], 1.0f));
    }

    GLuint posFocusTorxesLoc = glGetUniformLocation(program->programId(), "posFocusTorxes");
    glUniform3fv(posFocusTorxesLoc, 6, &torchPositions[0][0]);

	glBindVertexArray(0);
}


void A3GLWidget::modelTransformFar1()
{
    glm::mat4 TG = glm::mat4(1.0f);
    TG = glm::translate(TG, glm::vec3(2, 1, 4));
    TG = glm::scale(TG, glm::vec3(escalaModels[FAR_1]));
    TG = glm::translate(TG, -centreBaseModels[FAR_1]);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}
void A3GLWidget::modelTransformFar2()
{
 
    Far2_TG = glm::mat4(1.0f);
    Far2_TG = glm::translate(Far2_TG, glm::vec3(2, 1, 4));
    Far2_TG = glm::rotate(Far2_TG, glm::radians(angleFarSegment), glm::vec3(0.0f, 1.0f, 0.0f)); 
    Far2_TG = glm::scale(Far2_TG, glm::vec3(escalaModels[FAR_1]));
    Far2_TG = glm::translate(Far2_TG, -centreBaseModels[FAR_1]);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Far2_TG[0][0]);
}
void A3GLWidget::modelTransformVaixell()
{
	Vaixell_TG = glm::mat4(1.0f);
	Vaixell_TG = glm::translate(Vaixell_TG, VaixellPos);
	Vaixell_TG = glm::scale(Vaixell_TG, glm::vec3(escalaModels[VAIXELL]) );
	Vaixell_TG = glm::translate(Vaixell_TG,
		                -glm::vec3(centreBaseModels[VAIXELL].x,
		                           0,
		                           centreBaseModels[VAIXELL].z)); 
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Vaixell_TG[0][0]);
}



void A3GLWidget::updateSunPosition()
{
    sunPosition = glm::vec3(View * glm::vec4(sunPosition, 1.0));
    sunPosition = glm::vec3(angle, angle2, 0.0);
    GLuint lightPosLoc = glGetUniformLocation(program->programId(), "SolPos");
       glUniform3fv(lightPosLoc, 1, &sunPosition[0]);
}


void A3GLWidget::canviacolorsTorxes(int valor){
    makeCurrent();

     for (int i = 0; i < 6; ++i) {
         if (valor <= 20) {
             torchColors[i] = glm::vec3(1.0f, 1.0f, 0.0f);  
             savecol = glm::vec3(1.0f, 1.0f, 0.0f);
         } else if (valor <= 40) {
             torchColors[i] = glm::vec3(1.0f, 0.0f, 0.0f);
             savecol = glm::vec3(1.0f, 0.0f, 0.0f);
         } else if (valor <= 60) {
             torchColors[i] = glm::vec3(1.0f, 0.2f, 0.5f);
             savecol = glm::vec3(1.0f, 0.2f, 0.5f);  
         } else if (valor <= 80) {
             torchColors[i] = glm::vec3(0.0f, 1.0f, 1.0f);
             savecol =glm::vec3(0.0f, 1.0f, 1.0f);
         } else if (valor <= 99) {
             torchColors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
             savecol = glm::vec3(1.0f, 1.0f, 1.0f); 
         }
     }

     GLuint torchColorsLoc = glGetUniformLocation(program->programId(), "torchColors");
     glUniform3fv(torchColorsLoc, 6, &torchColors[0][0]);
    val = valor;
     update();
 }


void A3GLWidget::seleccionaTorxa(int num){
    makeCurrent();
        selectedTorchIndex = num - 1;
        update();

}

void A3GLWidget::apagaTorxa() {
    makeCurrent();
        if (torchColors[selectedTorchIndex] == glm::vec3(0.0f, 0.0f, 0.0f)) {

            torchColors[selectedTorchIndex] = savecol;

        } else {

            torchColors[selectedTorchIndex] = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        GLint colorLoc = glGetUniformLocation(program->programId(), "torchColors");
        glUniform3fv(colorLoc, 6, &torchColors[0][0]);
        update();


}

void A3GLWidget::actualizaposicioSol(int valor)
{
    makeCurrent();

    if (valor > prevValor)
    {

        if ((9.0f <= currentHour && currentHour <= 13.0f) || currentHour == 8.0f)
        {
            currentHour += 1;
            angle -= 6.667;
            angle2 += 6.6667;
        }
        else if (14.0f <= currentHour && currentHour <= 19.0f)
        {
            currentHour += 1;
            angle -= 6.6667;
            angle2 -= 6.6667;
        }
    }
    else if (valor < prevValor)
    {

        if (9.0f <= currentHour && currentHour <= 14.0f)
        {
            currentHour -= 1;
            angle += 6.6667;
            angle2 -= 6.6667;
        }
        else if ((14.0f < currentHour && currentHour <= 19.0f) || (currentHour == 20))
        {
            currentHour -= 1;
            angle += 6.6667;
            angle2 += 6.6667;
        }
    }

    prevValor = valor;

    updateSunPosition();
    update();
}



void A3GLWidget::keyPressEvent(QKeyEvent* event)
 {
     makeCurrent();

     switch (event->key()) {
         case Qt::Key_S:
             VaixellPos[2] += 1;
             break;
         case Qt::Key_W:
             VaixellPos[2] -= 1;
             break;
         case Qt::Key_Up:
             if ((9.0f <= currentHour && currentHour <= 13.0f) || (currentHour == 8.0f))
             {
                 currentHour += 1;
                 angle -= 6.667;
                 angle2 += 6.6667;
                 updateSunPosition();
             }

             if (14.0f <= currentHour && currentHour <= 19.0f)
             {
                 currentHour += 1;
                 angle -= 6.6667;
                 angle2 -= 6.6667;
                 updateSunPosition();
             }
             break;
         case Qt::Key_Down:
         if (9.0f <= currentHour && currentHour <= 14.0f)
         {
             currentHour -= 1;
             angle += 6.6667;
             angle2 -= 6.6667;
             updateSunPosition();
         }

         if ((14.0f < currentHour && currentHour <= 19.0f) || (currentHour == 20))
         {
             currentHour -= 1;
             angle += 6.6667;
             angle2 += 6.6667;
             updateSunPosition();
         }
         break;
         case Qt::Key_A:
             angleFarSegment -= 1.0f;
             angleFoci -= 1.0f;
             break;
         case Qt::Key_D:
             angleFarSegment += 1.0f;
             angleFoci += 1.0f;
             break;

         default:
             event->ignore();
             break;
     }
    emit WidgetSolupdate(currentHour);
     update();
 }

void A3GLWidget::carregaShaders()
{

	QOpenGLShader fs (QOpenGLShader::Fragment, this);
	QOpenGLShader vs (QOpenGLShader::Vertex, this);

	fs.compileSourceFile("./shaders/basicLlumShader.frag");
	vs.compileSourceFile("./shaders/basicLlumShader.vert");

	program = new QOpenGLShaderProgram(this);

	program->addShader(&fs);
	program->addShader(&vs);

	program->link();
	program->bind();

	vertexLoc = glGetAttribLocation (program->programId(), "vertex");
	normalLoc = glGetAttribLocation (program->programId(), "normal");
	matambLoc = glGetAttribLocation (program->programId(), "matamb");
	matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
	matspecLoc = glGetAttribLocation (program->programId(), "matspec");
	matshinLoc = glGetAttribLocation (program->programId(), "matshin");

	transLoc = glGetUniformLocation (program->programId(), "TG");
	projLoc = glGetUniformLocation (program->programId(), "proj");
	viewLoc = glGetUniformLocation (program->programId(), "view");
	normalMatrixLoc = glGetUniformLocation (program->programId(), "normalMatrix");
        posFocusTorxesLoc = glGetUniformLocation(program->programId(), "posFocusTorxes");
	

    
    GLuint ambientLightColorLoc = glGetUniformLocation(program->programId(), "llumAmbient");
       glUniform3f(ambientLightColorLoc, 0.1f, 0.1f, 0.1f);
    
    GLuint lightColorLoc = glGetUniformLocation(program->programId(), "solColor");
        glUniform3f(lightColorLoc, 0.6f, 0.6f, 0.6f);
    
    GLuint F1ColorLoc = glGetUniformLocation(program->programId(), "FColor");
        glUniform3f(F1ColorLoc, 0.8f,0.8f, 0.8f);
    
    for (int i = 0; i < 6; ++i) {
            torchColors[i] = glm::vec3(1.0f, 1.0f, 0.0f); 
     }
    torchColorLoc = glGetUniformLocation(program->programId(), "torchColors");
        glUniform3fv(torchColorLoc, 6, &torchColors[0][0]);

}
