/* This application is based on the following tutorial by Steven Lambert:
 * "Why You Should be Using Time-based Animation and How to Implement it"
 *     http://blog.sklambert.com/using-time-based-animation-implement/
 */
#include "fps_anim.h"
#include "time_anim.h"
#include "time_anim_improved.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Frame-based VS Time-based Animation");

    QPushButton reset_bt(&window);
    reset_bt.setText("Reset");

    FrameAnimation fps_widget(&window);
    QObject::connect(&reset_bt, SIGNAL(clicked()), &fps_widget, SLOT(reset()));

    TimeAnimation time_widget(&window);
    QObject::connect(&reset_bt, SIGNAL(clicked()), &time_widget, SLOT(reset()));

    TimeAnimationImproved time_imp_widget(&window);
    QObject::connect(&reset_bt, SIGNAL(clicked()), &time_imp_widget, SLOT(reset()));

    QVBoxLayout main_layout(&window);    
    main_layout.setAlignment(Qt::AlignCenter);
    main_layout.addWidget(&reset_bt, 0, Qt::AlignRight);
    main_layout.addWidget(&fps_widget);
    main_layout.addWidget(&time_widget);
    main_layout.addWidget(&time_imp_widget);

    window.setLayout(&main_layout);
    window.show();

    return app.exec();
}
