// menus.h: Menus for a simple text editor.
#ifndef MENUS_H
#define MENUS_H
#include <QtWidgets>

class Menus : public QMainWindow {
   Q_OBJECT
public:
   Menus(QWidget *parent = nullptr);
private slots:
   void open();
   void save();
   void info()const;
private:
   QTextEdit *pTextBox;
};
#endif // MENUS_H
