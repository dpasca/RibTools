//==================================================================
// dlog.h
//
// Logging and function tracing.
//==================================================================

#ifndef DLOG_H
#define DLOG_H

// DLOG(...) macros
#define D_LOGGINGx

// DFUNCTION() and DRETURN() macros
#define D_TRACKINGx

//==================================================================

#if defined(D_LOGGING)
# define DLOG(_fmt_, ...)                                               \
    {                                                                   \
        DUT::Logger::GetInstance()->Log(__FILE__, __LINE__, __FUNCTION__, \
                                        _fmt_, ##__VA_ARGS__);          \
    }
#else
# define DLOG(...)
#endif

#define DPRINT(_fmt_, ...)                                              \
    {                                                                   \
        DUT::Logger::GetInstance()->Log(__FILE__, __LINE__, __FUNCTION__, \
                                        _fmt_, ##__VA_ARGS__);           \
    }

#if defined(D_TRACKING)
# define DFUNCTION()                            \
    DUT::FunctionTracker _d_function_tracker(__FILE__, __LINE__, __FUNCTION__)
# define DRETURN(...)                            \
    _d_function_tracker.SetReturnLine(__LINE__); \
    return __VA_ARGS__
#else
# define DFUNCTION()
# define DRETURN(...) return __VA_ARGS__
#endif

namespace DUT
{

class Logger
{
public:
    static Logger *GetInstance();

    void Log(const char *file, int line, const char *func,
             const char *fmt, ...);
    void PushFn();
    void PopFn();
protected:
    Logger();

    static Logger *s_instance;
    int            m_indentLevel;
};

class FunctionTracker
{
public:
    FunctionTracker(const char *file, int line, const char *func);
    ~FunctionTracker();
    void SetReturnLine(int line);
protected:
    const char   *m_file;
    int           m_entryLine;
    const char   *m_func;
    int           m_exitLine;
};

} // namespace DUT

#endif // DLOG_H
