#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
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
#include "m_vca.h"
#include "port.h"

M_vca::M_vca(bool p_expMode, QWidget* parent, const char *name, SynthData *p_synthdata) 
              : Module(1, parent, name, p_synthdata) {

  QString qs;

  M_type = M_type_vca;
  setGeometry(MODULE_NEW_X, MODULE_NEW_Y, MODULE_VCA_WIDTH, MODULE_VCA_HEIGHT);
  gain1 = 0;
  gain2 = 0;
  in1 = 1.0;
  in2 = 1.0;
  out = 1.0;
  expMode = p_expMode;
  port_M_gain1 = new Port("Gain 0", PORT_IN, 0, this, synthdata); 
  port_M_gain1->move(0, 35);
  port_M_gain1->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_gain1);
  port_M_gain2 = new Port("Gain 1", PORT_IN, 1, this, synthdata); 
  port_M_gain2->move(0, 55);
  port_M_gain2->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_gain2);
  port_M_in1 = new Port("In 0", PORT_IN, 2, this, synthdata); 
  port_M_in1->move(0, 75);
  port_M_in1->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_in1);
  port_M_in2 = new Port("In 1", PORT_IN, 3, this, synthdata); 
  port_M_in2->move(0, 95);
  port_M_in2->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_in2);
  port_out = new Port("Out", PORT_OUT, 0, this, synthdata);          
  port_out->move(width() - port_out->width(), 115);
  port_out->outType = outType_audio;
  portList.append(port_out);
  if (expMode) qs.sprintf("Exp. VCA ID %d", moduleID);
  else         qs.sprintf("Lin. VCA ID %d", moduleID);
  configDialog->setCaption(qs);
  if (expMode) {
    configDialog->addSlider(0, 10, gain1, "Gain", &gain1, true);
    configDialog->addSlider(0, 10, gain2, "Gain 1", &gain2, true);
    configDialog->addSlider(0, 2, in1, "In 0", &in1, true);
    configDialog->addSlider(0, 2, in2, "In 1", &in2, true);
    configDialog->addSlider(0, 2, out, "Output level", &out, true);
  } else {
    configDialog->addSlider(0, 10, gain1, "Gain", &gain1);
    configDialog->addSlider(0, 10, gain2, "Gain 1", &gain2);
    configDialog->addSlider(0, 2, in1, "In 0", &in1);
    configDialog->addSlider(0, 2, in2, "In 1", &in2);
    configDialog->addSlider(0, 2, out, "Output level", &out);
  }
}

M_vca::~M_vca() {
}

void M_vca::generateCycle() {

  int l1, l2;
  float exp0;

  if (!cycleReady) {
    cycleProcessing = true;

    gainData1 = port_M_gain1->getinputdata ();
    gainData2 = port_M_gain2->getinputdata ();
    inData1 = port_M_in1->getinputdata ();
    inData2 = port_M_in2->getinputdata ();

    if (expMode) {
      exp0 = synthdata->exp_table(0);
      for (l1 = 0; l1 < synthdata->poly; l1++) {
        for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
          data[0][l1][l2] = (gain1 
                             + synthdata->exp_table(gainData1[l1][l2]) - exp0
                             + gain2 * (synthdata->exp_table(gainData2[l1][l2]-1.0)-exp0))
                          * out * (in1 * inData1[l1][l2] + in2 * inData2[l1][l2]);
        }
      }  
    } else {
      for (l1 = 0; l1 < synthdata->poly; l1++) {
        for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
          data[0][l1][l2] = (gain1 + gainData1[l1][l2] + gain2 * gainData2[l1][l2]) 
                          * out * (in1 * inData1[l1][l2] + in2 * inData2[l1][l2]);
        }
      }   
    }
  }
  cycleProcessing = false;
  cycleReady = true;
}

void M_vca::showConfigDialog() {
}
