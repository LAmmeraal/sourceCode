// function.cpp: Functions of x and y.
#include "function.h"

Function::Function(QWidget *parent) {
   setWindowTitle("Function z = f(x, y)");
   newFunctionAct = new QAction("New function data", this);
   newFunctionAct->setShortcut(Qt::CTRL | Qt::Key_N);
   connect(newFunctionAct, &QAction::triggered, this,
      &Function::newFunction);
   fileMenu->insertAction(openAct, newFunctionAct); // On top
   fileMenu->removeAction(openAct);
   fileMenu->removeAction(importAct);
   showFunDataAct = new QAction("Show function data", this);
   connect(showFunDataAct, &QAction::triggered, this,
      &Function::showFunctionData);
   viewMenu->addAction(showFunDataAct);
   showFunDataAct->setCheckable(true);
   showFunDataAct->setChecked(true);
   fileName = "function.dat"; // Used for any output file
}

void Function::showFunctionData() {
   showFunData = !showFunData;
   showFunDataAct->setChecked(showFunData);
   repaint();
}

void Function::newFunction() {
   if (thread.isRunning())
      interruptProcess();
   FuncDialog dialog;
   dialog.setGeometry(100, 100, 500, 300);
   dialog.xLineEdit->setText(xText);
   dialog.yLineEdit->setText(yText);
   dialog.zLineEdit->setText(zText);
   dialog.exec();
   xText = dialog.qX;
   yText = dialog.qY;
   zText = dialog.qbuf;
   string sX = xText.toUtf8().constData(),
      sY = yText.toUtf8().constData();

   Parser parser;
   parser.ok = true;
   parser.setBuffer(sX);
   qreal xa = parser.factor();
   int i = sX.find('(');
   sX = sX.substr(i + 1);
   parser.setBuffer(sX);
   qreal dx = parser.factor();
   i = sX.find(')');
   sX = sX.substr(i + 1);
   parser.setBuffer(sX);
   qreal xb = parser.factor();

   parser.setBuffer(sY);
   qreal ya = parser.factor();
   i = sY.find('(');
   sY = sY.substr(i + 1);
   parser.setBuffer(sY);
   qreal dy = parser.factor();
   i = sY.find(')');
   sY = sY.substr(i + 1);
   parser.setBuffer(sY);
   qreal yb = parser.factor();

   if (!parser.ok || xa + dx > xb || dx <= 0 ||
      ya + dy > yb || dy <= 0) {
      simpleMessage("Syntax error", "Error in input data for x or y.");
      return;
   }
   string buf = zText.toUtf8().constData();
   parser.setBuffer(buf);
   world.clear();
   polyList.clear();
   edges.clear();
   selection.clear();
   parser.generate(this, xa, dx, xb, ya, dy, yb);
   if (!parser.ok || parser.pos != parser.buflen) {
      simpleMessage("Syntax error", "f(x,y): error after position " +
         QString::number(parser.pos));
      return;
   }
   boundsCenterDistance();
   drawImage();
}

void Function::addQuad(int A, int B, int C, int D) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polygon.nrs.push_back(D);
   polyList.push_back(polygon);
}

void Function::paintEvent(QPaintEvent *e) {
   if (thread.isRunning())
      return;
   if (starting) { // Do this only once
      starting = false;
      newFunction();
   }
   else
      Edit3D::paintEvent(e);
   if (showFunData && !interrupted) {
      QPainter pnt(this);
      pnt.drawText(10, 80, zText);
      pnt.drawText(10, 100, xText);
      pnt.drawText(10, 120, yText);
   }
}

void addTriangle(Function *fun, int A, int B, int C) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   fun->polyList.push_back(polygon);
}

void addLine(Function *fun, int A, int B) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   fun->polyList.push_back(polygon);
}

string deleteSpace(const string &s) {
   string result;
   for (int i = 0; i < s.length(); i++) {
      char ch = s[i];
      if (!isspace(ch))
         result += ch;
   }
   return result;
}

void Parser::setBuffer(string s) {
   buf = deleteSpace(s);
   pos = 0;
   buflen = buf.length();
}

void Parser::generate(Function *fun, qreal xa, qreal dx, qreal xb,
   qreal ya, qreal dy, qreal yb) {
   int nx = round((xb - xa) / dx),
      ny = round((yb - ya) / dy);
   qreal za = 1e30F, zb = -1e30F;
   fun->world.push_back(Point3D(1e30, 0, 0));

   for (int j = 0; j <= ny; j++) {
      y = ya + j * dy;
      for (int i = 0; i <= nx; i++) {
         x = xa + i * dx;
         pos = 0;
         qreal z = expression();
         // Vertex nr for (x, y, z) is j * (nx + 1) + i + 1
         if (!ok) 
            return;
         if (z < za) za = z;
         if (z > zb) zb = z;
         fun->world.push_back(Point3D(x, y, z));
      }
      if (!ok) 
         return;    
   }
   // x, y and z axes:
   qreal dz = (zb - za) / 10,
      xa1 = min(xa - 2 * dx, 0.0), xb1 = max(xb + 2 * dx, 0.0),
      ya1 = min(ya - 2 * dy, 0.0), yb1 = max(yb + 2 * dy, 0.0),
      za1 = min(za - 2 * dz, 0.0), zb1 = max(zb + 2 * dz, 0.0);
   fun->world.push_back(Point3D(xa1, 0, 0));
   fun->world.push_back(Point3D(xb1, 0, 0));

   fun->world.push_back(Point3D(0, ya1, 0));
   fun->world.push_back(Point3D(0, yb1, 0));

   fun->world.push_back(Point3D(0, 0, za1));
   fun->world.push_back(Point3D(0, 0, zb1));

   for (int i = 0; i < nx; i++) {
      for (int j = 0; j < ny; j++) {
         int k = j * (nx + 1) + i + 1,
            k1 = k + 1, m = k + nx + 1, m1 = m + 1;
         addTriangle(fun, k, -m1, k1);
         addTriangle(fun, k1, m1, -k);
         addTriangle(fun, k, -m1, m);
         addTriangle(fun, m, m1, -k);
      }
   }
   int k = (nx + 1) * (ny + 1);
   addLine(fun, k + 1, k + 2); // x-axis
   addLine(fun, k + 3, k + 4); // y-axis
   addLine(fun, k + 5, k + 6); // z-axis
}

qreal Parser::expression() {
   qreal x = term();
   for (;;) {
      if (!ok)
         return 0;
      if (peek("+")) x += term(); else
         if (peek("-")) x -= term(); else break;
   }
   return x;
}

qreal Parser::term() {
   qreal x = factor();
   for (;;) {
      if (!ok)
         return 0;
      if (peek("*"))
         x *= factor();
      else
         if (peek("/"))
            x /= factor();
         else
            break;
   }
   return x;
}

qreal Parser::factor() {
   qreal v = 0;
   if (peek("-")) return -factor();
   if (peek("+")) return factor();
   if (peek("x")) return x;
   if (peek("y")) return y;
   if (peek("(")) {
      v = expression();
      if (!peek(")")) {
         ok = false;
         return 0;
      }
      return v;
   }
   if (peek("cos(")) {
      qreal arg = expression();
      v = cos(arg);
      if (peek(")"))
         return v;
      else {
         ok = false;
         return 0;
      }
   }
   if (peek("sin(")) {
      qreal arg = expression();
      v = sin(arg);
      if (peek(")"))
         return v;
      else {
         ok = false;
         return 0;
      }
   }
   if (peek("sqrt(")) {
      qreal arg = expression();
      v = sqrt(arg);
      if (peek(")"))
         return v;
      else {
         ok = false;
         return 0;
      }
   }
   if (peek("exp(")) {
      qreal arg = expression();
      v = exp(arg);
      if (peek(")"))
         return v;
      else {
         ok = false;
         return 0;
      }
   }
   if (peek("pow(")) {
      qreal base = expression();
      if (!peek(",")) {
         ok = false;
         return 0;
      }
      qreal exponent = expression();
      if (peek(")"))
         return pow(base, exponent);
      ok = false;
      return 0;
   }
   if (number()) return lastNum;
   ok = false;
   return 0;
}

char Parser::nextDigit() {
   if (pos < buflen && isdigit(buf[pos])) {
      return buf[pos++];
   }
   return '?'; // no digit as next char
}

bool Parser::number() {
   qreal x = 0;
   bool periodPresent = false, numFound = false;
   qreal scale = 1;

   // Accept 0, 1,...,9, and at most one period (.):
   for (;;) {
      char ch = buf[pos];
      if (isdigit(ch)) {
         numFound = true;
         pos++;
         x = 10 * x + ch - '0';
         if (periodPresent)
            scale *= 10;
      }
      else
         if (!periodPresent && peek("."))
            periodPresent = true;
         else
            break;
   }
   if (!numFound)
      return false;
   if (periodPresent)
      x /= scale;
   lastNum = x;
   return true;
}

bool Parser::peek(const string &s) {
   int len = s.length();
   if (pos + len > buflen)
      return false;
   if (buf.substr(pos, len) == s) {
      pos += len;
      return true;
   }
   return false;
}

FuncDialog::FuncDialog(QWidget *parent)
   : QDialog(parent) {
   zLabel = new QLabel("z = f(x, y) = ");
   zLineEdit = new QLineEdit;
   zLabel->setBuddy(zLineEdit);

   xLabel = new QLabel("xMin (deltaX) xMax");
   xLineEdit = new QLineEdit;
   xLabel->setBuddy(xLineEdit);

   yLabel = new QLabel("yMin (deltaY) yMax");
   yLineEdit = new QLineEdit;
   yLabel->setBuddy(yLineEdit);

   okButton = new QPushButton("OK");
   okButton->setDefault(true);
   okButton->setEnabled(false);

   cancelButton = new QPushButton("Cancel");

   connect(zLineEdit, SIGNAL(textChanged(const QString &)),
      this, SLOT(enableFuncButton(const QString &)));

   connect(okButton, SIGNAL(clicked()),
      this, SLOT(okClicked()));
   connect(cancelButton, SIGNAL(clicked()),
      this, SLOT(close()));

   QHBoxLayout *layout1 = new QHBoxLayout;
   QHBoxLayout *layout2 = new QHBoxLayout;
   QHBoxLayout *layout3 = new QHBoxLayout;
   QHBoxLayout *layout4 = new QHBoxLayout;

   layout1->addWidget(zLabel);
   layout1->addWidget(zLineEdit);

   layout2->addWidget(xLabel);
   layout2->addWidget(xLineEdit);

   layout3->addWidget(yLabel);
   layout3->addWidget(yLineEdit);

   layout4->addWidget(cancelButton);
   layout4->addWidget(okButton);

   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addLayout(layout1);
   mainLayout->addLayout(layout2);
   mainLayout->addLayout(layout3);
   mainLayout->addLayout(layout4);

   setLayout(mainLayout);
   setWindowTitle("Function and domain");
   setFixedHeight(sizeHint().height());
}

void FuncDialog::enableFuncButton(const QString &text) {
   okButton->setEnabled(!text.isEmpty());
}

void FuncDialog::okClicked() {
   qX = xLineEdit->text(), qY = yLineEdit->text();
   qbuf = zLineEdit->text();
   this->done(1);
}

