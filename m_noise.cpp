#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <qwidget.h>
#include <qstring.h>
#include <qslider.h>   
#include <qcheckbox.h>  
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qpainter.h>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "m_noise.h"
#include "port.h"

M_noise::M_noise(QWidget* parent, const char *name, SynthData *p_synthdata) 
              : Module(3, parent, name, p_synthdata) {

  QString qs;
  int l1, l2;
  long t;

  M_type = M_type_noise;
  rate = 5;
  level = 0.5;
  count = 0;
  setGeometry(MODULE_NEW_X, MODULE_NEW_Y, MODULE_NOISE_WIDTH, MODULE_NOISE_HEIGHT);
  port_white = new Port("White", PORT_OUT, 0, this, synthdata);          
  port_white->move(width() - port_white->width(), 35);
  port_white->outType = outType_audio;
  portList.append(port_white);
  port_pink = new Port("Pink", PORT_OUT, 1, this, synthdata);          
  port_pink->move(width() - port_pink->width(), 55);
  port_pink->outType = outType_audio;
  portList.append(port_pink);
  port_random = new Port("Random", PORT_OUT, 2, this, synthdata);          
  port_random->move(width() - port_random->width(), 75);
  port_random->outType = outType_audio;
  portList.append(port_random);
  qs.sprintf("Noise ID %d", moduleID);
  configDialog->setCaption(qs);
  configDialog->addSlider(0, 10, rate, "Random Rate", &rate);
  configDialog->addSlider(0, 1, level, "Random Level", &level);
  for (l1 = 0; l1 < synthdata->poly; l1++) {
    r[l1] = 0;
    for (l2 = 0; l2 < 3; l2++) {
      buf[l2][l1] = 0;
    }
  }
  t = time(NULL) % 1000000;
  srand(abs(t - 10000 * (t % 100)));
}

M_noise::~M_noise() {

}

void M_noise::paintEvent(QPaintEvent *ev) {
  
  QPainter p(this);
  QString qs;
  int l1;

  for (l1 = 0; l1 < 4; l1++) {
    p.setPen(QColor(195 + 20*l1, 195 + 20*l1, 195 + 20*l1));
    p.drawRect(l1, l1, width()-2*l1, height()-2*l1);
  }
  p.setPen(QColor(255, 255, 255));
  p.setFont(QFont("Helvetica", 10));
  p.drawText(10, 20, "Noise");
  p.setFont(QFont("Helvetica", 8)); 
  qs.sprintf("ID %d", moduleID);
  p.drawText(15, 32, qs);
}

void M_noise::noteOnEvent(int osc) {

}

void M_noise::noteOffEvent(int osc) {

}

void M_noise::generateCycle() {

  int l1, l2, l3, random_rate;
  float white_noise;

  if (!cycleReady) {
    cycleProcessing = true;
    for (l2 = 0; l2 < 3; l2++) {
      for (l1 = 0; l1 < synthdata->poly; l1++) {
        memcpy(lastdata[l2][l1], data[l2][l1], synthdata->cyclesize * sizeof(float));
      }
    }
    random_rate = (int)(5000.0 * (float)rate + 100.0);
    for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
      count++; 
      if (count > random_rate) {
        count = 0;
        for (l3 = 0; l3 < synthdata->poly; l3++) {
          r[l3] = level * (2.0 * (float)rand() / (float)RAND_MAX - 1.0);
        }
      }
      for (l1 = 0; l1 < synthdata->poly; l1++) {  
        white_noise = 2.0 * (float)rand() / (float)RAND_MAX - 1.0;
        data[0][l1][l2] = white_noise;
        buf[0][l1] = 0.99765 * buf[0][l1] + white_noise * 0.099046;
        buf[1][l1] = 0.963 * buf[1][l1] + white_noise * 0.2965164;
        buf[2][l1] = 0.57 * buf[2][l1] + white_noise * 1.0526913;
        data[1][l1][l2] = buf[0][l1] + buf[1][l1] + buf[2][l1] + white_noise * 0.1848;
        data[2][l1][l2] = r[l1];
      }
    }
  }
  cycleProcessing = false;
  cycleReady = true;
}

void M_noise::showConfigDialog() {
}