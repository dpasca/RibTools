
#include "dlog.h"
#include "DTypes.h"
#include "DUtils_Base.h"
#include <stdarg.h>

#if defined(ANDROID)
# include <pthread.h>
#endif

//#define USE_SIMPLE_LOG

namespace DUT
{

Logger      *Logger::s_instance = 0;

Logger *Logger::GetInstance()
{
    if (s_instance == 0)
    {
        s_instance = new Logger();
    }

    return s_instance;
}

Logger::Logger() :
    m_indentLevel(0)
{
}

void Logger::Log(const char *file, int line, const char *func,
                 const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);

    char buffer[1024];

#if defined(USE_SIMPLE_LOG)

    int basenameIdx = strlen(file)-1;
    while (basenameIdx > 0 && file[basenameIdx-1] != '/')
    {
        --basenameIdx;
    }

    int len = snprintf(
                    buffer, sizeof(buffer),
                    "%-26s %4i %-24s # ",
                    file+basenameIdx,
                    line < 0 ? 0 : line,
                    func );

    if ( len >= 0 )
        vsnprintf(buffer+len, sizeof(buffer)-len, fmt, vl);
#else

    // File name and line no

    int threadid = 0;
#if defined(ANDROID)
    threadid = pthread_self();
#endif

    static const int maxFileNameLen = 28;
    int basenameIdx = strlen(file)-1;
    while (basenameIdx > 0 && file[basenameIdx-1] != '/')
    {
        --basenameIdx;
    }

    snprintf(buffer, _countof(buffer)-2, "(%d)%s:%d", threadid, file+basenameIdx, line);
    int posn = strlen(buffer);

    // Whitespace out to a fixed point (or remove some of the filename)

    int indent = 0;
    if (posn > maxFileNameLen)
    {
        basenameIdx += (posn - maxFileNameLen);
        snprintf(buffer, _countof(buffer)-2, "(%d)%s:%d", threadid, file+basenameIdx, line);
        posn = strlen(buffer);
    }
    else
    {
        indent += maxFileNameLen-posn;
    }
    while (indent > 0 && posn < (int )(_countof(buffer)-2))
    {
        buffer[posn++] = ' ';
        --indent;
    }

    buffer[posn++] = ':';

    // Indentation

    indent = m_indentLevel;
    while (indent > 0 && posn < (int )(_countof(buffer)-2))
    {
        buffer[posn++] = ' ';
        --indent;
    }
    int remaining = _countof(buffer)-2-posn;

    // Function name

    snprintf(buffer+posn, remaining, "(%s)", func);
    posn = strlen(buffer);
    remaining = _countof(buffer)-2-posn;

    // Message

    vsnprintf(buffer+posn, remaining, fmt, vl);
    posn = strlen(buffer);
    //remaining = _countof(buffer)-2-posn;

    if (buffer[posn-1] != '\n')
    {
        buffer[posn] = '\n';
        buffer[posn+1] = '\0';
    }
#endif

    DVerbose(buffer);
}

void Logger::PushFn()
{
    m_indentLevel++;
    if (m_indentLevel < 0)
    {
        DVerbose("logging error.  m_indentLevel %d", m_indentLevel);
        m_indentLevel = 0;
    }
}

void Logger::PopFn()
{
    m_indentLevel--;
    if (m_indentLevel < 0)
    {
        DVerbose("logging error.  m_indentLevel %d", m_indentLevel);
        m_indentLevel = 0;
    }
}

//==================================================================

FunctionTracker::FunctionTracker(const char *file, int line, const char *func) :
    m_file(file),
    m_entryLine(line),
    m_func(func),
    m_exitLine(-1)
{
    Logger *l = Logger::GetInstance();

    l->PushFn();
    l->Log(m_file, m_entryLine, m_func, "<- Entering Function");
}

FunctionTracker::~FunctionTracker()
{
    Logger *l = Logger::GetInstance();

    l->Log(m_file, m_exitLine, m_func, "<- Leaving Function");
    l->PopFn();
}

void FunctionTracker::SetReturnLine(int exitLine)
{
    m_exitLine = exitLine;
}

} // namespace DUT
