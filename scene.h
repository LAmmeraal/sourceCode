// scene.h: Line remains in center of resized window.
#ifndef SCENE_H
#define SCENE_H
#include <QtWidgets>

class Scene : public QMainWindow {
   Q_OBJECT
public:
   Scene(QWidget *parent = nullptr);
protected:
   QGraphicsScene scene;
   QGraphicsView view;
};
#endif // SCENE_H
