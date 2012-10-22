/*
 * LoggerBase.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: kyle
 */

#include "LoggerBase.hpp"

namespace mulog {

#if MULOG_FEATURE_QT
void LoggerBase::issue(Severity s, const QString & msg) {
#if MULOG_OS_WINDOWS
	issue(s, msg.toStdWString());
#else
	issue(s, msg.toStdString());
#endif
}

LoggerBase::~LoggerBase() {
	// TODO Auto-generated destructor stub
}

} /* namespace mulog */
