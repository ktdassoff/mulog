/*
 * Logger.hpp
 *
 *  Created on: Oct 22, 2012
 *      Author: K. T. Dassoff
 */

#ifndef MULOG_LOGGER_HPP_
#define MULOG_LOGGER_HPP_

#include <mulog_config.hpp>

#include <stdint.h>
#include <string>
#include <ostream>

#if MULOG_FEATURE_QT
#include <QtCore/QString>
#include <QtCore/QLatin1String>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#endif

namespace mulog {

enum class Severity : uint_fast8_t {
	VerboseDebug = 0,
	Debug = 1,
	VerboseInfo = 2,
	Info = 3,
	Warning = 4,
	Error = 5,
	Critical = 6,
	Catastrophic = 7
};
constexpr bool isActive(Severity requested, Severity filter) { return static_cast<uint_fast8_t>(requested) >= static_cast<uint_fast8_t>(filter); }

class LoggerBase {
	Severity m_filter;
public:
	Severity severity() const { return m_filter; }
	void set_severity(Severity val) { m_filter = val; }
	bool will_issue(Severity requested) const { return isActive(requested, m_filter); }

	// Basic calls
	virtual void issue(Severity s, const std::string & msg) = 0;
	virtual void issue(Severity s, const std::wstring & msg) = 0;
#if MULOG_FEATURE_QT
	virtual void issue(Severity s, const QString & msg);
#endif

	// C++ ostream formatting
	//std::ostream & issue(Severity s);
	//std::wostream & issuew(Severity s);

#if MULOG_FEATURE_QT
	// Qt TextStream formatting
	//QTextStream & issueq(Severity s);
#endif

	// Shortcut functions for each severity type
	template<class STRTYPE>
	void vdbg(const STRTYPE & msg) { issue(Severity::VerboseDebug, msg); }
	template<class STRTYPE>
	void dbg(const STRTYPE & msg) { issue(Severity::Debug, msg); }
	template<class STRTYPE>
	void vinfo(const STRTYPE & msg) { issue(Severity::VerboseInfo, msg); }
	template<class STRTYPE>
	void info(const STRTYPE & msg) { issue(Severity::Info, msg); }
	template<class STRTYPE>
	void warn(const STRTYPE & msg) { issue(Severity::Warning, msg); }
	template<class STRTYPE>
	void err(const STRTYPE & msg) { issue(Severity::Error, msg); }
	template<class STRTYPE>
	void critical(const STRTYPE & msg) { issue(Severity::Critical, msg); }
	template<class STRTYPE>
	void catasrophic(const STRTYPE & msg) { issue(Severity::Catastrophic, msg); }

//	void vdbg() { issue(Severity::VerboseDebug); }
//	void dbg() { issue(Severity::Debug); }
//	void vinfo() { issue(Severity::VerboseInfo); }
//	void info() { issue(Severity::Info); }
//	void warn() { issue(Severity::Warning); }
//	void err() { issue(Severity::Error); }
//	void critical() { issue(Severity::Critical); }
//	void catasrophic() { issue(Severity::Catastrophic); }
//
//	void vdbgw() { issuew(Severity::VerboseDebug); }
//	void dbgw() { issuew(Severity::Debug); }
//	void vinfow() { issuew(Severity::VerboseInfo); }
//	void infow() { issuew(Severity::Info); }
//	void warnw() { issuew(Severity::Warning); }
//	void errw() { issuew(Severity::Error); }
//	void criticalw() { issuew(Severity::Critical); }
//	void catasrophicw() { issuew(Severity::Catastrophic); }
//
//#if MULOG_FEATURE_QT
//	void vdbgq() { issueq(Severity::VerboseDebug); }
//	void dbgq() { issueq(Severity::Debug); }
//	void vinfoq() { issueq(Severity::VerboseInfo); }
//	void infoq() { issueq(Severity::Info); }
//	void warnq() { issueq(Severity::Warning); }
//	void errq() { issueq(Severity::Error); }
//	void criticalq() { issueq(Severity::Critical); }
//	void catasrophicq() { issueq(Severity::Catastrophic); }
//#endif

	virtual ~LoggerBase();
};

} /* namespace mulog */
#endif /* MULOG_LOGGER_HPP_ */
