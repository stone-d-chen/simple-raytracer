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

// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
// if (show_demo_window)
//     ImGui::ShowDemoWindow(&show_demo_window);


// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
{
    static float f = 0.0f;
    // static int counter = 0;

    ImGui::Begin("World Editor");                          // Create a window called "Hello, world!" and append into it.
    if(ImGui::Checkbox("factor", &change_factor))
        reset_contributions = true;

    // ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    // ImGui::Checkbox("Another Window", &show_another_window);


    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    if(ImGui::ColorEdit3("clear color", (float*)&(Materials[3].RefColor))) // Edit 3 floats representing a color
        reset_contributions = true;

    // if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //     counter++;
    // ImGui::SameLine();
    // ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

// 3. Show another simple window.
// if (show_another_window)
// {
//     ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//     ImGui::Text("Hello from another window!");
//     if (ImGui::Button("Close Me"))
//         show_another_window = false;
//     ImGui::End();
// }

