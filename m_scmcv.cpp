#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <dlfcn.h>
#include <qregexp.h>
#include <QTextStream>

#include <math.h>
#include <qwidget.h>
#include <qstring.h>
#include <qslider.h>   
#include <qcheckbox.h>  
#include <qlabel.h>


#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qpainter.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "midipushbutton.h"
#include "m_scmcv.h"
#include "port.h"

M_scmcv::M_scmcv(QWidget* parent, QString *p_sclname) 
  : Module(M_type_scmcv, 4, parent, "Scala MCV")
{
  QString qs;
  int l1;

  setGeometry(MODULE_NEW_X, MODULE_NEW_Y, MODULE_SCMCV_WIDTH, MODULE_SCMCV_HEIGHT);
  port_gate_out = new Port("Gate", PORT_OUT, 0, this);          
  port_gate_out->move(width() - port_gate_out->width(), 35);
  port_gate_out->outType = outType_audio;
  portList.append(port_gate_out);
  port_note_out = new Port("Freq", PORT_OUT, 1, this);          
  port_note_out->move(width() - port_note_out->width(), 55);
  port_note_out->outType = outType_audio;
  portList.append(port_note_out);
  port_velocity_out = new Port("Velocity", PORT_OUT, 2, this);          
  port_velocity_out->move(width() - port_velocity_out->width(), 75);
  port_velocity_out->outType = outType_audio;
  portList.append(port_velocity_out);
  port_trig_out = new Port("Trigger", PORT_OUT, 3, this);
  port_trig_out->move(width() - port_trig_out->width(), 95);
  port_trig_out->outType = outType_audio;
  portList.append(port_trig_out);

  QStringList channelNames;
  channelNames << "RESERVED FOR LATER USE";
  for (l1 = 1; l1 < 17; l1++) {
    qs.sprintf("RESERVED FOR LATER USE");
    channelNames << qs;
  }
  channel = 0;
  base = 0;
  pitch = 0;
  lastbase = 12;
  pitchbend = 0;
  for (l1 = 0; l1 < synthdata->poly; l1++) {
    lastfreq[l1] = 0;
    freq[l1] = 0;
  }
  for (l1 = 0; l1 < 12; l1++) {       
    scale_lut_isRatio[l1] = false;
    scale_lut[l1] = 100.0 + (float)l1 * 100.0;
  }
  scale_lut_isRatio[12] = true;
  scale_lut[12] = 2.0;
  scale_lut_length = 12;
  configDialog->addComboBox(0, " ", &channel, channelNames.count(), &channelNames);
  configDialog->addIntSlider(-60, 60, base, "Scale Offset", &base);
  configDialog->addIntSlider(-36, 36, pitch, "Note Offset", &pitch);
  configDialog->addSlider(-1, 1, pitchbend, "Pitch", &pitchbend);
  sclname = "No_Scale_loaded";
  configDialog->addLabel("   Scale: " + sclname);
  configDialog->addLabel("   ");
  configDialog->addPushButton("Load Scale");
  QObject::connect(configDialog->midiPushButtonList.at(0), SIGNAL(clicked()),
                   this, SLOT(openBrowser())); 
  fileDialog = NULL;
  dirpath.sprintf("%s", getenv("SCALA_PATH"));
  if (dirpath.length() < 1) {
    fprintf(stderr, "\nYou did not set the environment variable SCALA_PATH.\n");
    fprintf(stderr, "Assuming SCALA_PATH=/usr/share/scala\n");
    dirpath = "/usr/share/scala";   
  } else
    StdErr << "SCALA_PATH: " << dirpath << endl;

  if (p_sclname && !p_sclname->contains("No_Scale_loaded"))
    loadScale(dirpath + "/" + *p_sclname);
}

void M_scmcv::calcScale() {

  int l1, index;
  float base_cv, base_freq;

  lastbase = base;
  base_cv = base / 12.0 - 5.0;
  base_freq = synthdata->exp_table_ln2(8.0313842 + base_cv);
  fprintf(stderr, "base: %d, base_cv: %f, base_freq: %f\n", base, base_cv, base_freq);  
  scale_notes[0] = base_cv;
  index = 1;
  while (index < 128) {
    for (l1 = 0; l1 < scale_lut_length; l1++) {      
      if (scale_lut_isRatio[l1]) {
        scale_notes[index] = log(base_freq * scale_lut[l1])/M_LN2 - 8.0313842;
      } else {
        scale_notes[index] = base_cv + scale_lut[l1] / 1200.0;
      }
      index++;
      if (index > 127) break;
    }
    base_cv = scale_notes[index - 1];
    base_freq = synthdata->exp_table_ln2(8.0313842 + base_cv);
  }
}

void M_scmcv::generateCycle() {

  int l1, l2, index;
  float df, gate, velocity;

  if (!cycleReady) {
    cycleProcessing = true;
    if (base != lastbase) {
      calcScale();
    }
    for (l1 = 0; l1 < synthdata->poly; l1++) {
      gate = ((synthdata->channel[l1] == channel-1)||(channel == 0)) && (synthdata->noteCounter[l1] < 1000000);
      lastfreq[l1] = freq[l1];
      index = synthdata->notes[l1] + pitch;
      
      freq[l1] = ((index >=0) && (index < 128)) ? pitchbend + scale_notes[index] : 0;
//      if (freq[l1] < 0) freq[l1] = 0;
      velocity = (float)synthdata->velocity[l1] / 127.0;
      memset(data[3][l1], 0, synthdata->cyclesize * sizeof(float));
//      data[3][l1][0] = trig[l1];
      data[3][l1][15] = synthdata->noteCounter[l1] == 0; // Added for interpolated input ports (e.g. m_vcenv.cpp)
      if ((freq[l1] == lastfreq[l1]) || (freq[l1] == 0) || (lastfreq[l1] == 0)) {
        for (l2 = 0; l2 < synthdata->cyclesize; l2++) {
          data[0][l1][l2] = gate;
          data[1][l1][l2] = freq[l1];
          data[2][l1][l2] = velocity;
        }
      } else {
        df = (freq[l1] - lastfreq[l1]) / (float)MODULE_SCMCV_RESPONSE;
        for (l2 = 0; l2 < MODULE_SCMCV_RESPONSE; l2++) {
          data[0][l1][l2] = gate;
          data[1][l1][l2] = lastfreq[l1] + df * (float)l2;
          data[2][l1][l2] = velocity;
        }
        for (l2 = MODULE_SCMCV_RESPONSE; l2 < synthdata->cyclesize; l2++) {
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

void M_scmcv::openBrowser() {

  if (!fileDialog) {
    fileDialog = new QFileDialog(NULL, tr("Load Scala"), dirpath, "Scala files (*.scl)");
    QObject::connect(fileDialog, SIGNAL(currentChanged(const QString &)), this, SLOT(loadScale(const QString &)));
  }
  fileDialog->show();
}

void M_scmcv::loadScale(const QString &p_sclname) {

  QString qs, qs2, qs3;
  int index, n;

  sclname = p_sclname;  
  QFile qfile(sclname);
  if (!qfile.open(QIODevice::ReadOnly)) {
    QMessageBox::information( this, "AlsaModularSynth", "Could not load Scala file "+sclname);
    sclname = "No_Scale_loaded";
    return;
  }
  configDialog->labelList.at(0)->setText("   Scale: " + sclname);
  QTextStream stream(&qfile);
  while (!stream.atEnd()) {
    qs = stream.readLine(); 
    if (!qs.contains("!")) 
      break;
  }
  configDialog->labelList.at(0)->setText("   " + qs);
  StdErr << "Scale: " << qs << endl;
  while (!stream.atEnd()) {  
    qs = stream.readLine();
    if (!qs.contains("!"))
      break;
  }
  index = 0;
  while (!stream.atEnd() && (index < 128)) {
    qs = stream.readLine();
    if (qs.contains("!")) {
      continue;
    }
    qs2 = qs.simplified();
    if (qs2.contains(".")) {
      if ((n = qs2.indexOf(" ")) > 0) {
        qs = qs2.left(n); 
      } else {
        qs = qs2;
      }
      scale_lut_isRatio[index] = false;
      scale_lut[index] = qs.toFloat();
      index++;
    } else {
      scale_lut_isRatio[index] = true;
      if (qs.contains("/")) {
        qs = qs2.left(qs2.indexOf("/"));
        qs3 = qs2.mid(qs2.indexOf("/") + 1);
        if ((n = qs3.indexOf(" ")) > 0) {
          qs2 = qs3.left(n); 
        } else {
          qs2 = qs3;
        }
        scale_lut[index] = qs.toFloat() / qs2.toFloat();
      } else {
        if ((n = qs2.indexOf(" ")) > 0)
          qs = qs2.left(n); 
        else
          qs = qs2;
        scale_lut[index] = qs.toFloat();
      }
      index++;
    }
  }
  scale_lut_length = index;
  calcScale();
}
