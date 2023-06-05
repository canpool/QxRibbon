#include "aboutdialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <time.h>

static time_t getDateFromMacro(const char *time)
{
    char s_month[5];
    int month, day, year;
    struct tm t;
    memset(&t, 0, sizeof(t));
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf_s(time, "%s %d %d", s_month, (unsigned int)sizeof(s_month) - 1, &day, &year);

    month = (strstr(month_names, s_month) - month_names) / 3;

    t.tm_mon = month;   // [0,11]
    t.tm_mday = day;
    t.tm_year = year - 1900;
    t.tm_isdst = -1;

    return mktime(&t);
}

static QString compilerString()
{
#if defined(Q_CC_CLANG) // must be before GNU, because clang claims to be GNU too
    QString isAppleString;
#if defined(__apple_build_version__) // Apple clang has other version numbers
    isAppleString = QLatin1String(" (Apple)");
#endif
    return QLatin1String("Clang " ) + QString::number(__clang_major__) + QLatin1Char('.')
            + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    return QLatin1String("GCC " ) + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
    if (_MSC_VER > 1999)
        return QLatin1String("MSVC <unknown>");
    if (_MSC_VER >= 1920)
        return QLatin1String("MSVC 2019");
    if (_MSC_VER >= 1910)
        return QLatin1String("MSVC 2017");
    if (_MSC_VER >= 1900)
        return QLatin1String("MSVC 2015");
#endif
    return QLatin1String("<unknown compiler>");
}

AboutDialog::AboutDialog(const QString &softlogo, const QString &softname, const QString &softver,
                         const QString &softdesc, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About %1").arg(softname));

    QGridLayout *layout = new QGridLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    QString versionString = tr("%1 %2").arg(softname, softver);
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QDateTime dt = QDateTime::fromTime_t((uint)getDateFromMacro(__DATE__));
#else
    QDateTime dt = QDateTime::fromMSecsSinceEpoch((qint64)getDateFromMacro(__DATE__) * 1000);
#endif
    QString buildDateInfo = tr("<br/>Built on %1 %2<br/>").arg(dt.toString("yyyy-MM-dd"), __TIME__);
    QString buildCompatibilityString = tr("Based on Qt %1 (%2, %3 bit)").arg(QLatin1String(qVersion()),
                                                                             compilerString(),
                                                                             QString::number(QSysInfo::WordSize));
    const QString description = tr(
        "<h3>%1</h3>"
        "%2<br/>"
        "%3"
        "<br/>"
        "%4")
        .arg(versionString,
             buildCompatibilityString,
             buildDateInfo,
             softdesc);

    QLabel *copyRightLabel = new QLabel(description);
    copyRightLabel->setWordWrap(true);
    copyRightLabel->setOpenExternalLinks(true);
    copyRightLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton *closeButton = buttonBox->button(QDialogButtonBox::Close);
    buttonBox->addButton(closeButton, QDialogButtonBox::ButtonRole(QDialogButtonBox::RejectRole | QDialogButtonBox::AcceptRole));
    connect(buttonBox , &QDialogButtonBox::rejected, this, &QDialog::reject);

    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(softlogo);
    layout->addWidget(logoLabel , 0, 0, 1, 1);
    layout->addWidget(copyRightLabel, 0, 1, 4, 4);
    layout->addWidget(buttonBox, 4, 0, 1, 5);
}

AboutDialog::~AboutDialog()
{
}
