while (SDL_PollEvent(&event))
{
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT)
        is_running = false;
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        is_running = false;
}
ImGui_ImplSDLRenderer2_NewFrame();
ImGui_ImplSDL2_NewFrame();
ImGui::NewFrame();

{
    static float f = 0.0f;

    ImGui::Begin("World Editor");                          // Create a window called "Hello, world!" and append into it.
    if(ImGui::Checkbox("factor", &change_factor))
        reset_contributions = true;

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    if(ImGui::ColorEdit3("clear color", (float*)&(Materials[3].RefColor))) // Edit 3 floats representing a color
        reset_contributions = true;

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}