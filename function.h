// function.h: Functions of x and y.
#ifndef FUNCTION_H
#define FUNCTION_H
#include "edit3d.h"

class Function : public Edit3D {
   Q_OBJECT
public:
   Function(QWidget *parent = nullptr);
protected:
   QString xText = "-1 (0.1) +1",
      yText = "-3 (0.3) +3",
      zText = "x * x + 0.25 * y * y";
   bool starting = true;
   string buf;
   bool showFunData = true;
   QAction *newFunctionAct, *showFunDataAct;
   void paintEvent(QPaintEvent *e)override;
   void addQuad(int A, int B, int C, int D);
protected slots:
   void newFunction();
   void showFunctionData();
};

class Parser {
public:
   string buf;
   int buflen;
   qreal x, y, lastNum;
   int pos;
   bool ok;
   void setBuffer(string s);
   void generate(Function *fun, qreal xa, qreal dx, qreal xb,
      qreal ya, qreal dy, qreal yb);
   qreal expression();
   qreal term();
   qreal factor();
   bool number();
   char nextDigit(); // '?' means "no digit"
   bool peek(const string &s);
};

class FuncDialog : public QDialog {
   Q_OBJECT
public:
   FuncDialog(QWidget *parent = 0);
   qreal xa, dx, xMax, ya, dy, yMax;
   QString qbuf, qX, qY;
   QLabel *zLabel, *xLabel, *yLabel;
   QLineEdit *zLineEdit, *xLineEdit, *yLineEdit;
   QPushButton *cancelButton;
   QPushButton *okButton;
signals:
   void funcNext(const QString &str, Qt::CaseSensitivity cs);
private slots:
   void okClicked();
   void enableFuncButton(const QString &text);
};
#endif // FUNCTION_H
