//////////////////////////////////////////////////////////////////////////
/// file:       Lis/Logger.h
///
/// summary:    Declares the logger
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>

namespace Lis
{
//////////////////////////////////////////////////////////////////////////
enum LogLevel
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERROR,
    LOG_NONE
};

//////////////////////////////////////////////////////////////////////////
class Logger
{
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Helper class to collect the whole message via '<<' operator
    ///   and put it into the logging streams. Instances of this class
    ///   could be obtained via Logger::Debug(), Logger::Info() and
    ///   Logger::Error() methods only.
    /// </summary>
    //////////////////////////////////////////////////////////////////////////
    class MessagePrinter
    {
        friend class Logger;
        Logger& m_parent;
        LogLevel m_level;
        std::unique_ptr<std::ostringstream> m_buf;

    protected:
        MessagePrinter(Logger& parent, LogLevel level)
            : m_parent(parent), m_level(level), m_buf(new std::ostringstream())
        {}

    public:
        //////////////////////////////////////////////////////////////////////////
        /// Default move constructor is enabled on purpose to allow proper work of
        /// the Logger's methods Debug(), Info() and Error()
        //////////////////////////////////////////////////////////////////////////
        MessagePrinter(MessagePrinter&&) = default;

        //////////////////////////////////////////////////////////////////////////
        /// Message is put to the logging stream on object destruction (at the
        /// end of the expression evaluation at the instantiation point)
        //////////////////////////////////////////////////////////////////////////
        ~MessagePrinter()
        {
            m_parent.PutMessage(m_level, m_buf->str());
        }

        //////////////////////////////////////////////////////////////////////////
        /// The only useful method here: put something into the stream
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MessagePrinter& operator << (const T& data)
        {
            *m_buf << data;
            return *this;
        }
    };

public:
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Get the logger instance
    /// </summary>
    //////////////////////////////////////////////////////////////////////////
    static Logger& GetInstance();

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Put the debug message into the log
    /// </summary>
    //////////////////////////////////////////////////////////////////////////
    MessagePrinter Debug()
    {
        return MessagePrinter(*this, LOG_DEBUG);
    }

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Put the informational message into the log
    /// </summary>
    //////////////////////////////////////////////////////////////////////////
    MessagePrinter Info()
    {
        return MessagePrinter(*this, LOG_INFO);
    }

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Put the error message into the log
    /// </summary>
    //////////////////////////////////////////////////////////////////////////
    MessagePrinter Error()
    {
        return MessagePrinter(*this, LOG_ERROR);
    }

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Set console logging level
    /// </summary>
    ///
    /// <param name="level"> The level of the console logging </param>
    //////////////////////////////////////////////////////////////////////////
    void EnableConsoleChannel(LogLevel level);

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Set file logging level
    /// </summary>
    ///
    /// <param name="cleanOld"> overwrite the previous log file if true </param>
    /// <param name="level"> The level of the console logging </param>
    //////////////////////////////////////////////////////////////////////////
    void EnableFileChannel(bool overwrite = true, LogLevel level = LOG_DEBUG);

protected:
    void PutMessage(LogLevel level, const std::string& message);

private:
    Logger();
    LogLevel m_consoleLevel;
    LogLevel m_fileLevel;
    std::ofstream m_logFile;
};
} // namespace Lis
