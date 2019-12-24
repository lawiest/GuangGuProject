#include <qglobal.h>
#include <QIcon>

#include "ribbonimages.h"

QIcon qtnIcon(ImagesId id, bool onlyLarge)
{
    QIcon icon;
    switch (id)
    {
        case Image_New:
                icon.addPixmap(QPixmap("./Resources/res/new32x32.png"));
            break;
        case Image_Open:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap("./Resources/res/open16x16.png"));
                icon.addPixmap(QPixmap("./Resources/res/open32x32.png"));
            break;
        case Image_Save:
            if (!onlyLarge)
                icon.addPixmap(QPixmap("./Resources/res/save16x16.png"));
            icon.addPixmap(QPixmap("./Resources/res/save32x32.png"));
            break;
        case Image_SaveAs:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap("./Resources/res/saveasfile16x16.png"));
                icon.addPixmap(QPixmap("./Resources/res/saveasfile32x32.png"));
            break;
        case Image_Clipboard:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap("./Resources/res/clipboard16x16.png"));
                icon.addPixmap(QPixmap("./Resources/res/clipboard32x32.png"));
            break;
        case Image_Close:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap("./Resources/res/close16x16.png"));
                icon.addPixmap(QPixmap("./Resources/res/close32x32.png"));
            break;
        case Image_Cut:
                icon.addPixmap(QPixmap("./Resources/res/cut16x16.png"));
            break;
        case Image_Undo :
                icon.addPixmap(QPixmap("./Resources/res/undo16x16.png"));
            break;
        case Image_Redo :
                icon.addPixmap(QPixmap("./Resources/res/redo16x16.png"));
            break;
        case Image_Print :
                if (!onlyLarge)
                    icon.addPixmap(QPixmap("./Resources/res/print16x16.png"));
                icon.addPixmap(QPixmap("./Resources/res/print32x32.png"));
            break;

        default:
                Q_ASSERT(false);
            break;
    }
    return icon;
}

QIcon qtnIcon(const QString& smallIcon, const QString& largeIcon)
{
  QIcon icon;
  if (!smallIcon.isEmpty())
    icon.addPixmap(QPixmap(smallIcon));
  if (!largeIcon.isEmpty())
    icon.addPixmap(QPixmap(largeIcon));
  return icon;
}
