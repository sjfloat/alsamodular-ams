#ifndef M_NOISE2_H
#define M_NOISE2_H

#include <qwidget.h>
#include "synthdata.h"
#include "module.h"
#include "port.h"

#define MODULE_NOISE2_WIDTH                90
#define MODULE_NOISE2_HEIGHT               75

enum Noises {WHITE,RAND,PINK};
class M_noise2 : public Module
{
  Q_OBJECT

  private:
    Noises NoiseType;
    int count;
    float rate, level;
    float buf[3], r;
    Port *port_white, *port_pink, *port_random;
    float randmax; 
  public:
    M_noise2(QWidget* parent=0);

  public slots:
    void generateCycle();
    void showConfigDialog();
};
  
#endif
