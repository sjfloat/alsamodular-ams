#ifndef PORT_H
#define PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qwidget.h>
#include <qlist.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qpoint.h>
#include "synthdata.h"

#define PORT_DEFAULT_WIDTH  80
#define PORT_DEFAULT_HEIGHT 15

enum dirType { PORT_IN, PORT_OUT };
enum outTypeEnum { outType_audio, outType_control };

class Port : public QWidget
{
  Q_OBJECT

  private:
    QString portName;
    QPopupMenu *contextMenu;
    SynthData *synthdata;
    int portWidth;

  public:
    Port(const QString &p_portName, dirType p_dir, int p_index, QWidget* parent=0, SynthData *p_synthdata=0, int p_portWidth = PORT_DEFAULT_WIDTH, int p_color = 0);
    ~Port();
    int connectTo(Port *port);

  private:
    void showContextMenu(QPoint pos);

  public:
    QWidget *parentModule;
    QList<Port> connectedPortList;
    dirType dir;
    int index, fontColor;
    outTypeEnum outType;
    bool highlighted;
    QValueList<outTypeEnum> outTypeAcceptList;

  signals:
    void portClicked();
    void portDisconnected();
    
  protected:
    virtual void paintEvent(QPaintEvent *ev);
    virtual void mousePressEvent (QMouseEvent* );
    virtual void mouseReleaseEvent (QMouseEvent* );
    virtual void mouseMoveEvent (QMouseEvent* );
    
  public slots:
    void disconnectClicked();  
    void checkConnectionStatus();
};
  
#endif
      