#include "Logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QObject>
#include <wtypes.h>
#include <shlobj.h>

QFile* Logger::logFile = Q_NULLPTR;
bool Logger::isInit = false;
QHash<QtMsgType, QString> Logger::contextNames = {
	{QtMsgType::QtDebugMsg,		" Debug  "},
	{QtMsgType::QtInfoMsg,		"  Info  "},
	{QtMsgType::QtWarningMsg,	"Warning "},
	{QtMsgType::QtCriticalMsg,	"Critical"},
	{QtMsgType::QtFatalMsg,		" Fatal  "}
};

// Define the minimum log level
static QtMsgType MIN_LOG_LEVEL = QtWarningMsg;



QString getLogFilePath() {
	TCHAR buffer[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer))) {

        QString path = QString::fromWCharArray(buffer) + "\\ExplorerSvgSupport";
		QDir directory = QDir(path);

		if (!directory.exists(path))
			directory.mkpath(path);

		return path + "\\log.txt";
	}
	return "C:\\ExplorerSvgSupportFallback.log"; // Fallback in case of failure
}

void setMinLogLevelFromEnv()
{
	char buffer[256];
	DWORD bufferSize = sizeof(buffer);

	DWORD dataSize = GetEnvironmentVariableA("MIN_LOG_LEVEL", buffer, bufferSize);
	if (dataSize == 0) {
		qDebug() << "Environment variable MIN_LOG_LEVEL is not set. Using default log level.";
		return;
    }
    else if (dataSize > bufferSize) {
		qDebug() << "Value of environment variable MIN_LOG_LEVEL is too long. Using default log level.";
		return;
    }
	char *envLevel = buffer;

	QString level = QString::fromUtf8(envLevel).toUpper();

	if (level == "DEBUG") {
		MIN_LOG_LEVEL = QtDebugMsg;
	}
	else if (level == "WARNING") {
		MIN_LOG_LEVEL = QtWarningMsg;
	}
	else if (level == "CRITICAL") {
		MIN_LOG_LEVEL = QtCriticalMsg;
	}
	else if (level == "FATAL") {
		MIN_LOG_LEVEL = QtFatalMsg;
	}
	else if (level == "INFO") {
		MIN_LOG_LEVEL = QtInfoMsg;
	}
	else {
		qWarning() << "Invalid MIN_LOG_LEVEL value:" << level << ". Using default (Warning).";
	}
}

void Logger::init() {
	if (isInit) {
		return;
	}

	// Set the logging level
	setMinLogLevelFromEnv();

	// Create log file
	logFile = new QFile;
	logFile->setFileName(getLogFilePath());
	logFile->open(QIODevice::Append | QIODevice::Text);

	// Redirect logs to messageOutput
	qInstallMessageHandler(Logger::messageOutput);

	// Clear file contents
	logFile->resize(0);

	Logger::isInit = true;
}

void Logger::clean() {
	// Restore default message handler before cleaning up
	qInstallMessageHandler(nullptr);

	if (logFile != Q_NULLPTR) {
		logFile->close();
		delete logFile;
		logFile = Q_NULLPTR;
	}
}

void Logger::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {

	if (type < MIN_LOG_LEVEL) {
		return; // Ignore messages below the threshold
	}

	QString log = QObject::tr("%1 | %2 | %3 | %4 | %5 | %6\n").
		arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).
		arg(Logger::contextNames.value(type)).
		arg(context.line).
		arg(QString(context.file).
			section('\\', -1)).			// File name without file path											
		arg(QString(context.function).
			section('(', -2, -2).		// Function name only
			section(' ', -1).
			section(':', -1)).
		arg(msg);

	logFile->write(log.toLocal8Bit());
	logFile->flush();
}