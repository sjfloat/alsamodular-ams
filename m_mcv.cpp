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
#include "m_mcv.h"
#include "port.h"

M_mcv::M_mcv(QWidget* parent, const char *name, SynthData *p_synthdata) 
              : Module(3, parent, name, p_synthdata) {

  QString qs;
  int l1;

  M_type = M_type_mcv;
  setGeometry(MODULE_NEW_X, MODULE_NEW_Y, MODULE_MCV_WIDTH, MODULE_MCV_HEIGHT);
  port_gate_out = new Port("Gate", PORT_OUT, 0, this, synthdata);          
  port_gate_out->move(width() - port_gate_out->width(), 35);
  port_gate_out->outType = outType_audio;
  portList.append(port_gate_out);
  port_note_out = new Port("Freq", PORT_OUT, 1, this, synthdata);          
  port_note_out->move(width() - port_note_out->width(), 55);
  port_note_out->outType = outType_audio;
  portList.append(port_note_out);
  port_velocity_out = new Port("Velocity", PORT_OUT, 2, this, synthdata);
  port_velocity_out->move(width() - port_velocity_out->width(), 75);
  port_velocity_out->outType = outType_audio;
  portList.append(port_velocity_out);
  qs.sprintf("MCV ID %d", moduleID);
  configDialog->setCaption(qs);

  channel=0;
  EnumParameter * ep = new EnumParameter( this,"MIDI Channel", "", &channel);
  for (l1 = 1; l1 < 17; l1++) {
    qs.sprintf("%4d", l1);
    ep->addItem(l1,qs);
  }
  ep->addItem(0,"Any");
  configDialog->addParameter(ep);
  ep->selectItem(0);


  pitch = 48;
  for (l1 = 0; l1 < synthdata->poly; l1++) {
    lastfreq[l1] = 0;
    freq[l1] = 0;
  }
  IntParameter * ip = new IntParameter(this,"Note Offset", "", 0, 84, &pitch);
  configDialog->addParameter(ip);
  ip->setValue(48);

  pitchbend = 0;
  FloatParameter * fp = new FloatParameter(this,"Pitch","",-1.0,1.0, &pitchbend);
  configDialog->addParameter(fp);
}

M_mcv::~M_mcv() {

}

void M_mcv::paintEvent(QPaintEvent *ev) {

  QPainter p(this);
  QString qs;
  int l1;

  for (l1 = 0; l1 < 4; l1++) {
    p.setPen(QColor(195 + 20*l1, 195 + 20*l1, 195 + 20*l1));
    p.drawRect(l1, l1, width()-2*l1, height()-2*l1);
  }
  p.setPen(QColor(255, 255, 255));
  p.setFont(QFont("Helvetica", 10));
  p.drawText(10, 20, "MCV");
  p.setFont(QFont("Helvetica", 8));
  qs.sprintf("ID %d", moduleID);
  p.drawText(15, 32, qs);
}

void M_mcv::noteOnEvent(int osc) {

}

void M_mcv::noteOffEvent(int osc) {

}

void M_mcv::generateCycle() {

  int l1, l2;
  float df, gate, velocity, log2;

  if (!cycleReady) {
    cycleProcessing = true;
    log2 = log(2.0);
    for (l1 = 0; l1 < synthdata->poly; l1++) {
      gate = ((synthdata->channel[l1] == channel-1)||(channel == 0)) && (float)synthdata->notePressed[l1];
      lastfreq[l1] = freq[l1];
      freq[l1] = pitchbend + float(synthdata->notes[l1]-pitch) / 12.0;
      if (freq[l1] < 0) freq[l1] = 0;
      velocity = (float)synthdata->velocity[l1] / 127.0;
      if ((freq[l1] == lastfreq[l1]) || (freq[l1] == 0) || (lastfreq[l1] == 0)) {
        for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
          data[0][l1][l2] = gate;
          data[1][l1][l2] = freq[l1];
          data[2][l1][l2] = velocity;
        }
      } else {
        df = (freq[l1] - lastfreq[l1]) / (float)MODULE_MCV_RESPONSE;
        for (l2 = 0; l2 < MODULE_MCV_RESPONSE; l2++) {
          data[0][l1][l2] = gate;
          data[1][l1][l2] = lastfreq[l1] + df * (float)l2;
          data[2][l1][l2] = velocity;
        }
        for (l2 = MODULE_MCV_RESPONSE; l2 < synthdata->cyclesize; l2++) {
          data[0][l1][l2] = gate;
          data[1][l1][l2] = freq[l1];
          data[2][l1][l2] = velocity;
        }
      }
    }
  }
  cycleProcessing = false;
  cycleReady = true;
}

void M_mcv::showConfigDialog() {
}
