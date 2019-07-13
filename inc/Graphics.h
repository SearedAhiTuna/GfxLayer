
#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

class Window;

class Graphics final
{
private:
    struct Request
    {
        Request()
        {
        }
        virtual ~Request() = default;
        virtual void run() = 0;

        std::condition_variable finishedCond;
        bool finished{};
    };

    template <class Lambda>
    struct CustomRequest : public Request
    {
        CustomRequest(Lambda& l) :
            _l(l)
        {
        }

        void run()
        {
            _l();
        }

        Lambda _l;
    };

    typedef std::function<void(Window*)> WindowCB;

    struct WindowData
    {
        Window* _window{};
        std::thread _callback;
        std::atomic<bool> _terminated{};

        WindowData(Window* window, const WindowCB& callback);
        ~WindowData();

        bool Terminated();
    };

private:
    Graphics(const unsigned int& timeoutMS = 0);

public:
    ~Graphics();

    void tick();
    void loop();

private:
    void frame();

public:

    bool finished() const;

    template <class Lambda>
    void request(Lambda l)
    {
        Request* req = new CustomRequest<Lambda>(l);
        request_int(req);
    }

    Window& createWindow(const int& w, const int& h, const std::string& title, const WindowCB& callback);

private:
    void request_int(Request* req);

private:
    std::list<Request*> _requests;
    std::mutex _reqMutex;

    std::list<std::shared_ptr<WindowData>> _windows;
    std::list<std::shared_ptr<WindowData>> _closedWindows;

    double _timeout{};

public:
    friend Graphics& initGraphics(const unsigned int& timeoutMS);
    friend Graphics& getGraphics();
    friend void destroyGraphics();
};

extern Graphics& initGraphics(const unsigned int& timeoutMS = 0);
extern Graphics& getGraphics();
extern void destroyGraphics();

extern bool onMainThread();
extern void assertMainThread();
extern void assertSubThread();

extern void throwGraphicsError(const std::string& msg);

#define GRAPHICS_CALL_BEGIN(...) { assertSubThread(); getGraphics().request([__VA_ARGS__]() {
#define GRAPHICS_CALL_END }); }