#ifndef DEBUG_H
#define DEBUG_H

#include <QString>
#include <QDebug>

QString debugFunctionName(QString name);

/*!
 * \def DEBUG(message)
 * Outputs a debug message showing the calling function and \a message.
 */
#define DEBUG(message) \
( \
(qDebug() << debugFunctionName(Q_FUNC_INFO) << ":" << message), \
(void)0 \
)

/*!
 * \def WARN(message)
 * Outputs a warning message showing the calling function and \a message.
 */
#define WARN(message) \
( \
(qWarning() << debugFunctionName(Q_FUNC_INFO) << ":" << message), \
(void)0 \
)

/*!
 * \def CRITICAL(message)
 * Outputs a critical message showing the calling function and \a message.
 */
#define CRITICAL(message) \
( \
(qCritical() << debugFunctionName(Q_FUNC_INFO) << ":" << message), \
(void)0 \
)

/*!
 * \def FATAL(message)
 * Outputs a fatal message showing the calling function and \a message and then core dump.
 */
#define FATAL(message) \
( \
(qFatal("%s : %s", debugFunctionName(Q_FUNC_INFO).toStdString().c_str(), QString(message).toStdString().c_str())), \
(void)0 \
)

#endif // DEBUG_H