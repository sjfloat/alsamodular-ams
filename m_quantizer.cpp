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
#include "m_quantizer.h"
#include "port.h"

M_quantizer::M_quantizer(QWidget* parent, const char *name, SynthData *p_synthdata) 
              : Module(2, parent, name, p_synthdata) {

  QString qs;
  int l1;

  M_type = M_type_quantizer;
  setGeometry(MODULE_NEW_X, MODULE_NEW_Y, MODULE_QUANTIZER_WIDTH, MODULE_QUANTIZER_HEIGHT);
  port_M_in = new Port("In", PORT_IN, 0, this, synthdata); 
  port_M_in->move(0, 35);
  port_M_in->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_in);
  port_M_trigger = new Port("Trigger", PORT_IN, 1, this, synthdata); 
  port_M_trigger->move(0, 55);
  port_M_trigger->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_trigger);
  port_M_transpose = new Port("Transpose", PORT_IN, 2, this, synthdata); 
  port_M_transpose->move(0, 75);
  port_M_transpose->outTypeAcceptList.append(outType_audio);
  portList.append(port_M_transpose);
  port_out = new Port("Out", PORT_OUT, 0, this, synthdata);          
  port_out->move(width() - port_out->width(), 95);
  port_out->outType = outType_audio;
  portList.append(port_out);
  port_trigger_out = new Port("Trigger Out", PORT_OUT, 1, this, synthdata);          
  port_trigger_out->move(width() - port_trigger_out->width(), 115);
  port_trigger_out->outType = outType_audio;
  portList.append(port_trigger_out);
  quantum = QUANT_12;
/*
  QStrList *quantumNames = new QStrList(true);
  quantumNames->append("1/12");
  quantumNames->append("1/6");
  quantumNames->append("Major Scale");
  quantumNames->append("Minor Scale");
  quantumNames->append("Major Chord");
  quantumNames->append("Minor Chord");
  quantumNames->append("Major 7 Chord");
  quantumNames->append("Minor 7 Chord");
  quantumNames->append("Major 6 Chord");
  quantumNames->append("Minor 6 Chord");
  quantumNames->append("Pentatonic");
  configDialog->addComboBox(0, "Quantization", (int *)&quantum, quantumNames->count(), quantumNames);
*/
  EnumParameter *ep = new EnumParameter(this,"Quantization","",(int*)&quantum);
  ep->addItem(0,"1/12");
  ep->addItem(1,"1/6");
  ep->addItem(2,"Major Scale");
  ep->addItem(3,"Minor Scale");
  ep->addItem(4,"Major Chord");
  ep->addItem(5,"Minor Chord");
  ep->addItem(6,"Major 7 Chord");
  ep->addItem(7,"Minor 7 Chord");
  ep->addItem(8,"Major 6 Chord");
  ep->addItem(9,"Minor 6 Chord");
  ep->addItem(10,"Pentatonic");
  configDialog->addParameter(ep);
  ep->selectItem(QUANT_12);
  qs.sprintf("Quantizer ID %d", moduleID);
  configDialog->setCaption(qs);
  for (l1 = 0; l1 < synthdata->poly; l1++) {
    qsig[l1] = 0;
    trigCount[l1] = 0;
    trigger[l1] = 0;
  }
  for (l1 = 0; l1 < 12; l1++) {
    lut[0][l1] = l1;
    lut[1][l1] = (int)(l1 / 2) * 2;
  }
  
  lut[2][0] = 0;
  lut[2][1] = 0;
  lut[2][2] = 2;
  lut[2][3] = 2;
  lut[2][4] = 4;
  lut[2][5] = 5;
  lut[2][6] = 5;
  lut[2][7] = 7;
  lut[2][8] = 7;
  lut[2][9] = 9;
  lut[2][10] = 9;
  lut[2][11] = 11;
  lut[3][0] = 0;
  lut[3][1] = 0;
  lut[3][2] = 2;
  lut[3][3] = 3;
  lut[3][4] = 3;
  lut[3][5] = 5;
  lut[3][6] = 5;
  lut[3][7] = 7;
  lut[3][8] = 8;
  lut[3][9] = 8;
  lut[3][10] = 10;
  lut[3][11] = 10;
  lut[4][0] = 0;
  lut[4][1] = 0;
  lut[4][2] = 0;
  lut[4][3] = 0;
  lut[4][4] = 4;
  lut[4][5] = 4;
  lut[4][6] = 4;
  lut[4][7] = 7;
  lut[4][8] = 7;
  lut[4][9] = 7;
  lut[4][10] = 7;
  lut[4][11] = 7;
  lut[5][0] = 0;
  lut[5][1] = 0;
  lut[5][2] = 0;
  lut[5][3] = 3;
  lut[5][4] = 3;
  lut[5][5] = 3;
  lut[5][6] = 3;
  lut[5][7] = 7;
  lut[5][8] = 7;
  lut[5][9] = 7;
  lut[5][10] = 7;
  lut[5][11] = 7;
  lut[6][0] = 0;
  lut[6][1] = 0;
  lut[6][2] = 0;
  lut[6][3] = 0;
  lut[6][4] = 4;
  lut[6][5] = 4;
  lut[6][6] = 4;
  lut[6][7] = 7;
  lut[6][8] = 7;
  lut[6][9] = 7;
  lut[6][10] = 7;
  lut[6][11] = 11;
  lut[7][0] = 0;
  lut[7][1] = 0;
  lut[7][2] = 0;
  lut[7][3] = 3;
  lut[7][4] = 3;
  lut[7][5] = 3;
  lut[7][6] = 3;
  lut[7][7] = 7;
  lut[7][8] = 7;
  lut[7][9] = 7;
  lut[7][10] = 10;
  lut[7][11] = 10;
  lut[8][0] = 0;
  lut[8][1] = 0;
  lut[8][2] = 0;
  lut[8][3] = 0;
  lut[8][4] = 4;
  lut[8][5] = 4;
  lut[8][6] = 4;
  lut[8][7] = 7;
  lut[8][8] = 7;
  lut[8][9] = 9;
  lut[8][10] = 9;
  lut[8][11] = 9;
  lut[9][0] = 0;
  lut[9][1] = 0;
  lut[9][2] = 0;
  lut[9][3] = 3;
  lut[9][4] = 3;
  lut[9][5] = 3;
  lut[9][6] = 3;
  lut[9][7] = 7;
  lut[9][8] = 7;
  lut[9][9] = 9;
  lut[9][10] = 9;
  lut[9][11] = 9;
  lut[10][0] = 0;
  lut[10][1] = 0;
  lut[10][2] = 2;
  lut[10][3] = 2;
  lut[10][4] = 2;
  lut[10][5] = 5;
  lut[10][6] = 5;
  lut[10][7] = 7;
  lut[10][8] = 7;
  lut[10][9] = 9;
  lut[10][10] = 9;
  lut[10][11] = 9;
}

M_quantizer::~M_quantizer() {
}

void M_quantizer::paintEvent(QPaintEvent *ev) {
  
  QPainter p(this);
  QString qs;
  int l1;

  for (l1 = 0; l1 < 4; l1++) {
    p.setPen(QColor(195 + 20*l1, 195 + 20*l1, 195 + 20*l1));
    p.drawRect(l1, l1, width()-2*l1, height()-2*l1);
  }
  p.setPen(QColor(255, 255, 255));
  p.setFont(QFont("Helvetica", 10));
  p.drawText(10, 20, "Quantizer");
  p.setFont(QFont("Helvetica", 8)); 
  qs.sprintf("ID %d", moduleID);
  p.drawText(15, 32, qs);
}

void M_quantizer::generateCycle() {

  int l1, l2, quant, lutquant, transpose;
  float log2;

  if (!cycleReady) {
    cycleProcessing = true;
    for (l2 = 0; l2 < 2; l2++) {
      for (l1 = 0; l1 < synthdata->poly; l1++) {
        memcpy(lastdata[l2][l1], data[l2][l1], synthdata->cyclesize * sizeof(float));
      }
    }
    if (port_M_in->connectedPortList.count()) {
      in_M_in = (Module *)port_M_in->connectedPortList.at(0)->parentModule;
      if (!in_M_in->cycleProcessing) {
        in_M_in->generateCycle();
        inData = in_M_in->data[port_M_in->connectedPortList.at(0)->index];
      } else {
        inData = in_M_in->lastdata[port_M_in->connectedPortList.at(0)->index];
      }
    } else {
      in_M_in = NULL;
      inData = synthdata->zeroModuleData;
    }
    if (port_M_trigger->connectedPortList.count()) {
      in_M_trigger = (Module *)port_M_trigger->connectedPortList.at(0)->parentModule;
      if (!in_M_trigger->cycleProcessing) {
        in_M_trigger->generateCycle();
        triggerData = in_M_trigger->data[port_M_trigger->connectedPortList.at(0)->index];
      } else {
        triggerData = in_M_trigger->lastdata[port_M_trigger->connectedPortList.at(0)->index];
      }
    } else {
      in_M_trigger = NULL;
      triggerData = synthdata->zeroModuleData;
    }
    if (port_M_transpose->connectedPortList.count()) {
      in_M_transpose = (Module *)port_M_transpose->connectedPortList.at(0)->parentModule;
      if (!in_M_transpose->cycleProcessing) {
        in_M_transpose->generateCycle();
        transposeData = in_M_transpose->data[port_M_transpose->connectedPortList.at(0)->index];
      } else {
        transposeData = in_M_transpose->lastdata[port_M_transpose->connectedPortList.at(0)->index];
      }
    } else {
      in_M_transpose = NULL;
      transposeData = synthdata->zeroModuleData;
    }
    if (!in_M_trigger) {
        for (l1 = 0; l1 < synthdata->poly; l1++) {
          for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
            quant = (int)((100.0 + inData[l1][l2]) * 12.0);
            lutquant = lut[quantum][quant % 12] + (int)(quant / 12) * 12;
            if (qsig[l1] != lutquant) {
              qsig[l1] = lutquant;
              data[1][l1][l2] = 1.0;
              trigCount[l1] = 512;  
            } else {
              if (trigCount[l1] > 0) {
                data[1][l1][l2] = 1;  
                trigCount[l1]--;
              } else {
                data[1][l1][l2] = 0;
              }
            }  
            transpose = (int)(transposeData[l1][l2] * 12.0);
            data[0][l1][l2] = (float)qsig[l1] / 12.0 - 100 + (float)transpose / 12.0;
          }
         } 
    } else {
        for (l1 = 0; l1 < synthdata->poly; l1++) {
          for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
            if (!trigger[l1] && (triggerData[l1][l2] > 0.5)) {
              trigger[l1] = true;
              qsig[l1] = (int)((100.0 + inData[l1][l2]) * 12.0);
              data[1][l1][l2] = 1.0;
              trigCount[l1] = 512;
            } else {
              if (trigger[l1] && (triggerData[l1][l2] < 0.5)) {
                trigger[l1] = false;
              }
            }
            if (trigCount[l1] > 0) {
              data[1][l1][l2] = 1;
              trigCount[l1]--;
            } else {
              data[1][l1][l2] = 0;
            }
            transpose = (int)(transposeData[l1][l2] * 12.0);
            data[0][l1][l2] = (float)lut[quantum][qsig[l1] % 12] / 12.0 + qsig[l1] / 12 - 100  
                            + (float)transpose / 12.0;
          }   
        }
    }
  }
  cycleProcessing = false;
  cycleReady = true;
}

void M_quantizer::showConfigDialog() {
}
