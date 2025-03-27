#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "gpupixel.h"
// ImGui相关头文件
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace gpupixel;


std::shared_ptr<BeautyFaceFilter> beautyFaceFilter;
std::shared_ptr<FaceReshapeFilter> faceReshapeFilter;
std::shared_ptr<gpupixel::LipstickFilter> lipstickFilter;
std::shared_ptr<gpupixel::BlusherFilter> blusherFilter;
std::shared_ptr<SourceImage> sourceImage;
std::shared_ptr<SinkRawData> sinkRawData;
std::shared_ptr<SinkRender> sinkRender;

float beautyIntensity = 0;
float whitenIntensity = 0;
float faceSlimIntensity = 0;
float eyeEnlargeIntensity = 0;
float lipstickIntensity = 0;
float blusherIntensity = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

 void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

int main()
{
    glfwInit();
    GLFWwindow* window = GPUPixelContext::getInstance()->GetGLContext();
  
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    gladLoadGL();
    glfwMakeContextCurrent(window);

   glfwShowWindow(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // 启用键盘控制
    
    // 设置ImGui风格
    ImGui::StyleColorsDark();
    
    // 设置平台/渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // create filter
    // ----
    lipstickFilter = LipstickFilter::create();
    blusherFilter = BlusherFilter::create();
    faceReshapeFilter = FaceReshapeFilter::create();
    
    //  filter pipline
    // ----
    sourceImage = SourceImage::create("demo.png");
    sinkRender = std::make_shared<SinkRender>();

    sourceImage->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstickFilter->SetFaceLandmarks(landmarks);
       blusherFilter->SetFaceLandmarks(landmarks);
       faceReshapeFilter->SetFaceLandmarks(landmarks);
     });

    beautyFaceFilter = BeautyFaceFilter::create();
 
    sourceImage->addSink(lipstickFilter)
                ->addSink(blusherFilter)
                ->addSink(faceReshapeFilter)
                ->addSink(beautyFaceFilter)
                ->addSink(sinkRender);
                    
    // 
    sinkRender->onSizeChanged(1280, 720);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // 检测ESC键退出
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        // 开始ImGui帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // ImGui窗口
        ImGui::Begin("美颜控制面板");
        
        // 美颜强度滑块
        if (ImGui::SliderFloat("磨皮强度", &beautyIntensity, 0.0f, 10.0f)) {
            beautyFaceFilter->setBlurAlpha(beautyIntensity/10);
        }
        
        // 美白强度滑块
        if (ImGui::SliderFloat("美白强度", &whitenIntensity, 0.0f, 10.0f)) {
            beautyFaceFilter->setWhite(whitenIntensity/20);
        }
        
        // 瘦脸强度滑块
        if (ImGui::SliderFloat("瘦脸强度", &faceSlimIntensity, 0.0f, 10.0f)) {
            faceReshapeFilter->setFaceSlimLevel(faceSlimIntensity/200);
        }
        
        // 大眼强度滑块
        if (ImGui::SliderFloat("大眼强度", &eyeEnlargeIntensity, 0.0f, 10.0f)) {
            faceReshapeFilter->setEyeZoomLevel(eyeEnlargeIntensity/100);
        }
        
        // 口红强度滑块
        if (ImGui::SliderFloat("口红强度", &lipstickIntensity, 0.0f, 10.0f)) {
            lipstickFilter->setBlendLevel(lipstickIntensity/10);
        }
        
        // 腮红强度滑块
        if (ImGui::SliderFloat("腮红强度", &blusherIntensity, 0.0f, 10.0f)) {
            blusherFilter->setBlendLevel(blusherIntensity/10);
        }
        
        ImGui::End();
        
        // 
        // -----
        sourceImage->Render();
        
        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    // 清理ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    
}
