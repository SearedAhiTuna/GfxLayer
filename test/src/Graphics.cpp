
#include "Graphics.h"
#include "Libs.h"
#include "Window.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

#include <glm/gtc/constants.hpp>
#include "lodepng.h"

static std::mutex mainGraphicsMutex;
static Graphics* mainGraphics{};
static std::thread::id mainThreadID;

static void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    std::cerr << "GL CALLBACK: ";
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        std::cerr << "** GL ERROR **";
    }
    std::cerr << " type = " << type << ",";
    std::cerr << " severity = " << severity << ",";
    std::cerr << " message = " << message << std::endl;
}

const GLfloat PI = glm::pi<GLfloat>();

Graphics::WindowData::WindowData(Window* window, const WindowCB& callback) :
    _window(window)
{
    auto* t = &_terminated;
    _callback = std::thread(
        [t, window, callback]
        {
            callback(window);
            t->store(true);
            glfwPostEmptyEvent();
        }
    );
}

Graphics::WindowData::~WindowData()
{
    _callback.join();
    delete _window;
}

bool Graphics::WindowData::Terminated()
{
    return _terminated.load();
}

Graphics::Graphics(const unsigned int& timeoutMS)
{
    if (timeoutMS == 0)
        _timeout = 0;
    else
        _timeout = (double)timeoutMS / 1000;
}

Graphics::~Graphics()
{
}

void Graphics::tick()
{
    frame();
    glfwPollEvents();
}

void Graphics::loop()
{
    while (!finished())
    {
        frame();

        if (_timeout == 0)
            glfwWaitEvents();
        else
            glfwWaitEventsTimeout(_timeout);
    }
}

void Graphics::frame()
{
    // Update any windows
    for (auto it = _windows.begin(); it != _windows.end();)
    {
        WindowData& wd = *it->get();
        Window& w = *wd._window;

        if (!w._updated.load())
        {
            w.Update();
        }

        if (w.ShouldClose())
        {
            // Let the subthread know to close
            w._extShouldClose.store(true);

            // Move this window to the closed windows list
            _closedWindows.emplace_back(*it);
            it = _windows.erase(it);
        }
        else
            ++it;
    }

    // Process any requests for graphics operations
    std::unique_lock<std::mutex> reqLock(_reqMutex);
    for (auto it = _requests.begin(); it != _requests.end(); it = _requests.erase(it))
    {
        // Run the request
        (*it)->run();

        // Mark the request as finished
        (*it)->finished = true;
        (*it)->finishedCond.notify_one();

        // Yield the lock
        reqLock.unlock();
        reqLock.lock();
    }

    // Check for terminated subthreads
    for (auto it = _closedWindows.begin(); it != _closedWindows.end();)
    {
        WindowData& wd = *it->get();
        Window& w = *wd._window;

        if (wd.Terminated())
        {
            it = _closedWindows.erase(it);
        }
        else
            ++it;
    }
}

bool Graphics::finished() const
{
    return _windows.empty() && _closedWindows.empty();
}

Window& Graphics::createWindow(const int& w, const int& h, const std::string& title, const WindowCB& callback)
{
    // First window must be created on the main thread
    if (_windows.empty())
        assertMainThread();

    GLFWwindow* glfwWindow = nullptr;

    if (onMainThread())
    {
        // Create a GLFW window
        glfwWindow = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
        if (!glfwWindow)
            throwGraphicsError("Failed to create GLFW window");

        // Set window as current context
        glfwMakeContextCurrent(glfwWindow);

        // Initialize GLEW
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
            throwGraphicsError("Failed to initialize GLEW");

        // Enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(messageCallback, 0);

        // Enable depth buffer
        glEnable(GL_DEPTH_TEST);

        // Set sticky
        glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);

        // Dark blue background
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    }
    else
    {
        GRAPHICS_CALL_BEGIN(&glfwWindow, w, h, title)
            // Create a GLFW window
            glfwWindow = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
            if (!glfwWindow)
                throwGraphicsError("Failed to create GLFW window");

            // Set window as current context
            glfwMakeContextCurrent(glfwWindow);

            // Initialize GLEW
            glewExperimental = true;
            if (glewInit() != GLEW_OK)
                throwGraphicsError("Failed to initialize GLEW");

            // Enable debug output
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(messageCallback, 0);

            // Enable depth buffer
            glEnable(GL_DEPTH_TEST);

            // Set sticky
            glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);

            // Dark blue background
            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        GRAPHICS_CALL_END
    }

    Window* window = new Window(glfwWindow);
    _windows.emplace_back(new WindowData(window, callback));
    return *window;
}

void Graphics::request_int(Request* req)
{
    std::unique_lock<std::mutex> lk(_reqMutex);

    _requests.push_back(req);

    glfwPostEmptyEvent();

    while (!req->finished)
        req->finishedCond.wait(lk);

    delete req;
}

Graphics& initGraphics(const unsigned int& timeoutMS)
{
    std::lock_guard<std::mutex> lk(mainGraphicsMutex);

    if (!mainGraphics)
    {
        // Create the graphics singleton
        mainGraphics = new Graphics(timeoutMS);

        // Initialize GLFW
        if (glfwInit() != GLFW_TRUE)
            throwGraphicsError("Failed to initialize GLFW");

        // Set the main thread
        mainThreadID = std::this_thread::get_id();
    }

    return *mainGraphics;
}

Graphics& getGraphics()
{
    std::lock_guard<std::mutex> lk(mainGraphicsMutex);

    if (!mainGraphics)
    {
        throwGraphicsError("Call to getGraphics when graphics have not been initialized");
    }

    return *mainGraphics;
}

void destroyGraphics()
{
    std::lock_guard<std::mutex> lk(mainGraphicsMutex);

    if (mainGraphics)
    {
        delete mainGraphics;
        mainGraphics = nullptr;
    }
}

bool onMainThread()
{
    std::lock_guard<std::mutex> lk(mainGraphicsMutex);
    return (mainGraphics != nullptr) && mainThreadID == std::this_thread::get_id();
}

void assertMainThread()
{
    if (!onMainThread())
    {
        throwGraphicsError("Assertion failed: This is not the main thread");
    }
}

void assertSubThread()
{
    if (onMainThread())
    {
        throwGraphicsError("Assertion failed: This is not a sub thread");
    }
}

void sleepMS(unsigned int t)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

class GraphicsError : public std::runtime_error
{
public:
    GraphicsError(const std::string& msg) :
        runtime_error(msg)
    {
        std::cerr << "Graphics Error: " << msg << std::endl;
        std::cerr.flush();
    }
};

void throwGraphicsError(const std::string& msg)
{
    throw GraphicsError(msg);
}

static std::mutex glTextureMutex;

struct TexInfo
{
    GLuint id;
    unsigned int refCount;

    TexInfo() :
        id(0),
        refCount(0)
    {
    }

    TexInfo(const GLuint& _id) :
        id(_id),
        refCount(1)
    {
    }
};

static std::map<std::string, TexInfo> texDB;
static std::map<GLuint, std::string> texNameDB;

static GLuint loadPNG(const std::string& fn, const bool& alpha)
{
    LodePNGColorType colorType = alpha ? LCT_RGBA : LCT_RGB;

    std::vector<unsigned char> imgData;
    unsigned int w, h;

    lodepng::decode(imgData, w, h, fn, colorType, 8);

    unsigned char* dataArr = imgData.data();

    std::cout << "Decoded PNG file: " << w << "x" << h << std::endl;

    for (int i = 0; i < imgData.size() && i < 24; ++i)
    {
        std::cout << (int)imgData.data()[i] << " ";
    }
    std::cout << "\n";

    std::lock_guard<std::mutex> lk(glTextureMutex);

    GLuint textureID;
    GRAPHICS_CALL_BEGIN(&textureID, alpha, w, h, dataArr)
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, w, h); GL_CHECK
        //glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, imgData.data()); GL_CHECK

        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, w, h, 0,
                     alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, dataArr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GRAPHICS_CALL_END

    return textureID;
}

unsigned int loadTexture(const std::string& fn, const bool& alpha)
{
    if (texDB.count(fn))
    {
        ++texDB[fn].refCount;
        return texDB[fn].id;
    }

    std::string ext = fn.substr(fn.find_last_of('.'));

    GLuint textureID = 0;

    if (ext == ".png")
    {
        textureID = loadPNG(fn, alpha);
    }
    else
    {
        throwGraphicsError("Unrecognized image file format: " + ext);
    }

    texDB.emplace(fn, textureID);
    texNameDB[textureID] = fn;
    return textureID;
}

void freeTexture(const unsigned int& id)
{
    std::lock_guard<std::mutex> lk(glTextureMutex);

    if (texNameDB.count(id) == 0)
    {
        throwGraphicsError("Attempted to free uninitialized texture: " + id);
    }

    const std::string& fn = texNameDB[id];
    TexInfo& info = texDB[fn];

    if (--info.refCount == 0)
    {
        texDB.erase(fn);
        texNameDB.erase(id);

        GRAPHICS_CALL_BEGIN(&id)
            glDeleteTextures(1, &id);
        GRAPHICS_CALL_END
    }
}