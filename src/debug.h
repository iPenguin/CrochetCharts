#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

#define sws_debug(message) \
( \
    (qDebug() << Q_FUNC_INFO << ":" << message), \
    (void)0 \
)

#define sws_warn(message) \
( \
    (qWarning() << Q_FUNC_INFO << ":" << message), \
    (void)0 \
)

#define sws_critical(message) \
( \
    (qCritical() << Q_FUNC_INFO << ":" << message), \
    (void)0 \
)

#define sws_fatal(message) \
( \
    (qFatal() << Q_FUNC_INFO << ":" << message), \
    (void)0 \
)

#endif // DEBUG_H
