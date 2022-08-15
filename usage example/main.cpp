#include "widget.h"

#include <QApplication>

#include <QString>
#include <QDebug>

void test(const QString &&s) {

    QString ss = std::move(s);

    qDebug() << ss << '\t' << s << '\t' << s.size();

    /*
    https://stackoverflow.com/questions/64185958/if-using-qstring-after-being-stdmoved-defined
This statement, from the C++ standard, refers to the classes from the C++ library, as noted. However, it is a fairly easy bet that the Qt library seeks to comply to the same standards here, and this applies to the Qt library as well. It's a very safe bet to make (and it happens to be true, too).
"Valid but unspecified" state means that the moved-from object is still a valid, well-formed object. Because it's unspecified you have no guarantees, whatsoever, that if you call its size() method that you'll get any particular value. It could be 0 (probably), or it could be something else. However, since it's a valid object, whatever you get from a size() of a moved-from QString, if you then proceed and check what's in the string, you'll see the same exact number of character in there.
And, since it's a valid QString object you are free to completely replace its contents by assigning something to it, leaving the object is a fully-specified, valid state.
*/

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //убираем зернистось текстуры (добавляем больше сэмплов в отрисовку):
//    QSurfaceFormat format;
//    format.setSamples(16);
//    format.setDepthBufferSize(24);
//    QSurfaceFormat::setDefaultFormat(format);

    Widget w;
    w.show();

//test("hello");

    return a.exec();
}
