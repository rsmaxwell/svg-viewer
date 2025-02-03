#pragma once

#include <QDebug>
#include <QFile>
#include <QHash>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(general)


class Logger {
private:
	// The file object where logs are written to.
	static QFile* logFile;

	// Whether the logger has being initialized.
	static bool isInit;

	// The different type of contexts.
	static QHash<QtMsgType, QString> contextNames;

public:
	// Initializes the logger.
	static void init();

	// Cleans up the logger.
	static void clean();

	// The function which handles the logging of text.
	static void messageOutput(QtMsgType type, const QMessageLogContext& context,
		const QString& msg);
};