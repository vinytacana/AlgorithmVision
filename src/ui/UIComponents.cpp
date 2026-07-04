#include "UIComponents.h"
#include "AlgorithmRegistry.h"

void setupTheme(bool darkMode) {
    if (darkMode) ImGui::StyleColorsDark();
    else ImGui::StyleColorsLight();
}

const char* getAlgorithmDescription(Algorithm algorithm) {
    return AlgorithmRegistry::getMetadata(algorithm).description;
}
